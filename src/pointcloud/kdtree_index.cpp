#include <core_library/print.hpp>
#include <core_library/stack.hpp>
#include <pointcloud/kdtree_index.hpp>
#include <QtGlobal>

#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>

KDTreeIndex::KDTreeIndex()
{
}

KDTreeIndex::~KDTreeIndex()
{
}

KDTreeIndex::point_index_t KDTreeIndex::pick_point(cone_t cone, const uint8_t* coordinates, uint stride, KDTreeIndex::point_index_t fallback) const
{
  point_index_t best_point = fallback;
  float distance_of_best_point = std::numeric_limits<float>::infinity();

  struct stack_entry_t
  {
    subtree_t subtree;
    aabb_t aabb;
  };

  Stack<stack_entry_t> stack;
  stack.reserve(tree.size());

  stack.push(stack_entry_t{whole_tree(), total_aabb});

  while(!stack.is_empty())
  {
    const stack_entry_t current = stack.pop();

    float near_distance;
    float far_distance;
    if(!ray.intersects_aabb(current.aabb, &near_distance, &far_distance) || near_distance>distance_of_best_point)
      continue;

    point_index_t current_point = tree[current.subtree.root()];
    glm::vec3 current_coordinate = coordinate_for_index(current_point, coordinates, stride);

    float t_nearest;
    float distance = ray.distance_to(current_coordinate, &t_nearest);
    if(distance < t_nearest)
    {
      float current_distance = glm::distance(ray.origin, current_coordinate);
      if(distance_of_best_point > current_distance)
      {
        distance_of_best_point = current_distance;
        best_point = current_point;
      }
    }

    TODO: add left and right subtrees to the stack
  }

  return best_point;
}

size_t KDTreeIndex::root_point() const
{
  return range_t{0, this->tree.size()}.median();
}

bool KDTreeIndex::has_children(size_t point) const
{
  subtree_t subtree = traverse_kd_tree_to_point(point, [](subtree_t){});

  return subtree.range.is_leaf() == false;
}

std::pair<size_t, size_t> KDTreeIndex::children_of(size_t point) const
{
  subtree_t subtree = traverse_kd_tree_to_point(point, [](subtree_t){});

  return std::make_pair(subtree.left_subtree().root(), subtree.right_subtree().root());
}

size_t KDTreeIndex::parent_of(size_t point) const
{
  size_t parent = point;
  traverse_kd_tree_to_point(point, [&parent](subtree_t tree){
    parent = tree.root();
  });

  return parent;
}

std::pair<aabb_t, aabb_t> KDTreeIndex::aabbs_split_by(size_t point, const uint8_t* coordinates, uint stride) const
{
  auto coordinate_for_index = [coordinates, stride, this](size_t entry_index, uint8_t dimension) -> float {
    return component_for_index(entry_index, dimension, coordinates, stride);
  };

  aabb_t aabb = total_aabb;

  subtree_t tree = traverse_kd_tree_to_point(point, [&aabb, point, coordinate_for_index](subtree_t tree){
    const uint8_t split_dimension = tree.split_dimension;
    const size_t root = tree.root();
    if(point < root)
      aabb.max_point[split_dimension] = glm::min(aabb.max_point[split_dimension], coordinate_for_index(root, split_dimension));
    else
      aabb.min_point[split_dimension] = glm::max(aabb.min_point[split_dimension], coordinate_for_index(root, split_dimension));
  });

  const uint8_t split_dimension = tree.split_dimension;
  std::pair<aabb_t, aabb_t> aabbs_split_by_point = std::make_pair(aabb, aabb);

  if(tree.is_leaf())
    return aabbs_split_by_point;

  aabbs_split_by_point.first.max_point[split_dimension] = glm::min(aabb.max_point[split_dimension], coordinate_for_index(point, split_dimension));
  aabbs_split_by_point.second.min_point[split_dimension] = glm::max(aabb.min_point[split_dimension], coordinate_for_index(point, split_dimension));

  return aabbs_split_by_point;
}

glm::vec3 KDTreeIndex::point_coordinate(size_t point, const uint8_t* coordinates, uint stride) const
{
  return coordinate_for_index(point, coordinates, stride);
}

void KDTreeIndex::clear()
{
  tree.clear();
}

void KDTreeIndex::build(aabb_t total_aabb, const uint8_t* coordinates, size_t num_points, uint stride, std::function<bool(size_t, size_t)> feedback)
{
  auto coordinate_for_index = [coordinates, stride](point_index_t point_index, uint8_t dimension) -> float {
    return component_for_index(point_index, dimension, coordinates, stride);
  };

  tree.resize(num_points);
  this->total_aabb = total_aabb;

  // Fill the array with the coordinates in original order
  for(size_t i=0; i<num_points; ++i)
    tree[i] = point_index_t(i);

  Stack<subtree_t> stack;
  stack.reserve(num_points/2);

  stack.push(whole_tree());

  size_t num_processed_points = 0;
  size_t feedback_count_down;

  auto reset_feedback_countdown = [&feedback_count_down, num_points]() {
    feedback_count_down = num_points;
  };
  reset_feedback_countdown();

  while(!stack.is_empty())
  {
    const subtree_t current_tree = stack.pop();
    const uint8_t dimension = current_tree.split_dimension;
    num_processed_points++;

    boost::sort::block_indirect_sort(
          tree.data()+current_tree.range.begin,
          tree.data()+current_tree.range.end,
          [dimension, coordinate_for_index](point_index_t a, point_index_t b){return coordinate_for_index(a, dimension) < coordinate_for_index(b, dimension);});

    const subtree_t left_subtree = current_tree.left_subtree();
    if(!left_subtree.is_leaf())
      stack.push(left_subtree);
    else
      num_processed_points += left_subtree.range.size();

    const subtree_t right_subtree = current_tree.right_subtree();
    if(!right_subtree.is_leaf())
      stack.push(right_subtree);
    else
      num_processed_points += right_subtree.range.size();

    if(Q_UNLIKELY(feedback_count_down < current_tree.range.size()))
    {
      reset_feedback_countdown();
      if(!feedback(num_processed_points, num_points))
      {
        tree.clear();
        return;
      }
    }else
    {
      feedback_count_down -= current_tree.range.size();
    }
  }

#ifndef NDEBUG
  validate_tree(coordinates, num_points, stride);
#endif
}

bool KDTreeIndex::is_initialized() const
{
  return !tree.empty();
}

KDTreeIndex::subtree_t KDTreeIndex::traverse_kd_tree_to_point(size_t point, std::function<void(subtree_t)> visitor) const
{
  subtree_t subtree = whole_tree();

  size_t subtree_root = subtree.root();
  while(point != subtree_root)
  {
    visitor(subtree);

    if(point < subtree_root)
      subtree = subtree.left_subtree();
    else
      subtree = subtree.right_subtree();
    subtree_root = subtree.root();
  }

  return subtree;
}

KDTreeIndex::subtree_t KDTreeIndex::whole_tree() const
{
  return subtree_t{range_t{0, this->tree.size()}, 0};
}

void KDTreeIndex::validate_tree(const uint8_t* coordinates, size_t num_points, uint stride)
{

  Stack<subtree_t> stack;
  stack.reserve(num_points/2);

  stack.push(whole_tree());

  while(!stack.is_empty())
  {
    const subtree_t subtree = stack.pop();

    auto coordinate_for_index = [coordinates, stride, subtree, this](size_t entry_index) -> float {
      return component_for_index(entry_index, subtree.split_dimension, coordinates, stride);
    };

    const size_t root_index = subtree.root();
    const float split = coordinate_for_index(root_index);

    for(size_t i=subtree.range.begin; i<root_index; ++i)
      Q_ASSERT(coordinate_for_index(i) <= split);

    for(size_t i=root_index; i<subtree.range.end; ++i)
      Q_ASSERT(coordinate_for_index(i) >= split);

    subtree_t left = subtree.left_subtree();
    if(!left.is_leaf())
      stack.push(left);
    subtree_t right = subtree.right_subtree();
    if(!right.is_leaf())
      stack.push(right);
  }
}

float KDTreeIndex::component_for_index(point_index_t point_index, uint8_t dimension, const uint8_t* coordinates, uint stride)
{
  float coordinate;
  std::memcpy(&coordinate, coordinates + size_t(point_index)*stride + dimension*sizeof(float), sizeof(float));
  return coordinate;
}

float KDTreeIndex::component_for_index(size_t entry_index, uint8_t dimension, const uint8_t* coordinates, uint stride) const
{
  return component_for_index(tree[entry_index], dimension, coordinates, stride);
}

glm::vec3 KDTreeIndex::coordinate_for_index(point_index_t point_index, const uint8_t* coordinates, uint stride)
{
  glm::vec3 coordinate;
  std::memcpy(&coordinate, coordinates + size_t(point_index)*stride, 3*sizeof(float));
  return coordinate;
}

glm::vec3 KDTreeIndex::coordinate_for_index(size_t entry_index, const uint8_t* coordinates, uint stride) const
{
  return coordinate_for_index(tree[entry_index], coordinates, stride);
}

bool KDTreeIndex::range_t::is_empty() const
{
  return size() == 1;
}

bool KDTreeIndex::range_t::is_leaf() const
{
  return size() <= 1;
}

size_t KDTreeIndex::range_t::size() const
{
  Q_ASSERT(begin <= end);
  return end-begin;
}

size_t KDTreeIndex::range_t::median() const
{
  return (end - begin) / 2 + begin;
}

KDTreeIndex::range_t KDTreeIndex::range_t::left_subtree() const
{
  return range_t{begin, median()};
}

KDTreeIndex::range_t KDTreeIndex::range_t::right_subtree() const
{
  return range_t{median()+1, end};
}

KDTreeIndex::subtree_t KDTreeIndex::subtree_t::subtree(KDTreeIndex::range_t range) const
{
  const uint8_t new_split_dimension = (split_dimension + 1) % 3;

  const size_t root = this->root();
  Q_ASSERT(root<range.begin || range.end<=root);

  return subtree_t{range, new_split_dimension};
}
