#include <core_library/print.hpp>
#include <pointcloud/kdtree_index.hpp>
#include <QtGlobal>

#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>

inline float component_for_index(size_t point_index, uint8_t dimension, const uint8_t* coordinates, uint stride)
{
  float coordinate;
  std::memcpy(&coordinate, coordinates + point_index*stride + dimension*sizeof(float), sizeof(float));
  return coordinate;
}

inline glm::vec3 coordinate_for_index(size_t point_index, const uint8_t* coordinates, uint stride)
{
  glm::vec3 coordinate;
  std::memcpy(&coordinate, coordinates + point_index*stride, 3*sizeof(float));
  return coordinate;
}

KDTreeIndex::KDTreeIndex()
{
}

KDTreeIndex::~KDTreeIndex()
{
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
  auto coordinate_for_index = [coordinates, stride](size_t point_index, uint8_t dimension) -> float {
    return component_for_index(point_index, dimension, coordinates, stride);
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
  auto coordinate_for_index = [coordinates, stride](size_t point_index, uint8_t dimension) -> float {
    return component_for_index(point_index, dimension, coordinates, stride);
  };

  tree.resize(num_points);
  this->total_aabb = total_aabb;

  // Fill the array with the coordinates in original order
  for(size_t i=0; i<num_points; ++i)
    tree[i] = i;

  struct stack_element_t
  {
    range_t subtree;
    uint8_t dimension;
  };

  std::vector<stack_element_t> stack;
  stack.reserve(num_points/2);

  stack.push_back(stack_element_t{range_t{0, num_points}, 0});

  size_t num_processed_points = 0;
  size_t feedback_count_down;

  auto reset_feedback_countdown = [&feedback_count_down, num_points]() {
    feedback_count_down = num_points;
  };
  reset_feedback_countdown();

  while(!stack.empty())
  {
    const range_t current_tree = (stack.end()-1)->subtree;
    const uint8_t dimension = (stack.end()-1)->dimension;
    stack.pop_back();
    num_processed_points++;

    boost::sort::block_indirect_sort(
          tree.data()+current_tree.begin,
          tree.data()+current_tree.end,
          [dimension, coordinate_for_index](size_t a, size_t b){return coordinate_for_index(a, dimension) < coordinate_for_index(b, dimension);});

    const uint8_t next_dimension = (dimension + 1) % 3;
    const range_t left_subtree = current_tree.left_subtree();
    if(!left_subtree.is_leaf())
      stack.push_back(stack_element_t{left_subtree, next_dimension});
    else
      num_processed_points += left_subtree.size();

    const range_t right_subtree = current_tree.right_subtree();
    if(!right_subtree.is_leaf())
      stack.push_back(stack_element_t{right_subtree, next_dimension});
    else
      num_processed_points += right_subtree.size();

    if(Q_UNLIKELY(feedback_count_down < current_tree.size()))
    {
      reset_feedback_countdown();
      if(!feedback(num_processed_points, num_points))
      {
        tree.clear();
        return;
      }
    }else
    {
      feedback_count_down -= current_tree.size();
    }
  }
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
