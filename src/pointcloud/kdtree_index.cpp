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
  if(tree.empty())
    return fallback;

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


    // intersectiong a cone with an aabb is too complicated, instead we get the closest ray within the cone to the aabb center
    ray_t ray_for_intersection_test = cone.closest_ray_towards(current.aabb.center_point());

    float near_distance;
    float far_distance;
    if(!ray_for_intersection_test.intersects_aabb(current.aabb, &near_distance, &far_distance) || near_distance>distance_of_best_point)
      continue;

    point_index_t current_point = tree[current.subtree.root()];
    glm::vec3 current_coordinate = coordinate_for_index(current_point, coordinates, stride);

    Q_ASSERT(current.aabb.contains(current_coordinate));

    if(cone.contains(current_coordinate))
    {
      // TODO inread of getting the distace of the point to the cone origin, first determine the distance to the center ray of the cone, than add the distance along the center ray?

      float current_distance = glm::distance(cone.origin, current_coordinate);
      if(distance_of_best_point > current_distance)
      {
        distance_of_best_point = current_distance;
        best_point = current_point;
      }
    }

    std::pair<aabb_t, aabb_t> sub_aabbs = current.aabb.split(current.subtree.split_dimension, current_coordinate);

    if(!current.subtree.is_leaf())
    {
      stack.push(stack_entry_t{current.subtree.left_subtree(), sub_aabbs.first});
      stack.push(stack_entry_t{current.subtree.right_subtree(), sub_aabbs.second});
    }
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
  auto coordinate_for_index = [coordinates, stride, this](size_t entry_index) -> glm::vec3 {
    return KDTreeIndex::coordinate_for_index(entry_index, coordinates, stride);
  };

  aabb_t aabb = total_aabb;

  subtree_t tree = traverse_kd_tree_to_point(point, [&aabb, point, coordinate_for_index](subtree_t tree){
    size_t root = tree.root();
    glm::vec3 current_coordinate = coordinate_for_index(root);

    std::pair<aabb_t, aabb_t> aabbs_split_by_point = aabb.split(tree.split_dimension, current_coordinate);

    Q_ASSERT(aabb.contains(current_coordinate));

    if(point < root)
      aabb = aabbs_split_by_point.first;
    else
      aabb = aabbs_split_by_point.second;
  });

  if(tree.is_leaf())
    return std::make_pair(aabb, aabb);
  else
    return aabb.split(tree.split_dimension, coordinate_for_index(point));
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
  auto coordinate_for_index = [coordinates, stride, this](size_t entry_index) -> glm::vec3 {
    return KDTreeIndex::coordinate_for_index(entry_index, coordinates, stride);
  };

  struct stack_entry_t
  {
    aabb_t aabb;
    subtree_t subtree;
  };

  Stack<stack_entry_t> stack;
  stack.reserve(num_points/2);

  stack.push(stack_entry_t{total_aabb, whole_tree()});

  while(!stack.is_empty())
  {
    const stack_entry_t stack_entry = stack.pop();
    const subtree_t subtree = stack_entry.subtree;
    const aabb_t aabb = stack_entry.aabb;

    const size_t root_index = subtree.root();
    const glm::vec3 split = coordinate_for_index(root_index);
    const uint8_t split_dimension = subtree.split_dimension;
    std::pair<aabb_t, aabb_t> split_aabb = aabb.split(split_dimension, split);

    Q_ASSERT(aabb.contains(split));

    aabb_t left_aabb = split_aabb.first;
    aabb_t right_aabb = split_aabb.second;

    for(size_t i=subtree.range.begin; i<root_index; ++i)
    {
      const glm::vec3 p = coordinate_for_index(i);
      Q_ASSERT(left_aabb.contains(p));
      Q_ASSERT(p[split_dimension] <= split[split_dimension]);
    }

    for(size_t i=root_index; i<subtree.range.end; ++i)
    {
      const glm::vec3 p = coordinate_for_index(i);
      Q_ASSERT(right_aabb.contains(p));
      Q_ASSERT(coordinate_for_index(i)[split_dimension] >= split[split_dimension]);
    }


    // TODO: don't check for each subtree, whether is is a leaf. instead, check for the current tree, whether it is a leaf
    subtree_t left = subtree.left_subtree();
    if(!subtree.is_leaf())
      stack.push(stack_entry_t{left_aabb, left});
    subtree_t right = subtree.right_subtree();
    if(!right.is_leaf())
      stack.push(stack_entry_t{right_aabb, right});
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
