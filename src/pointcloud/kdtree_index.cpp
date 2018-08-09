#include <pointcloud/kdtree_index.hpp>
#include <QtGlobal>

#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>

KDTreeIndex::KDTreeIndex()
{
}

KDTreeIndex::~KDTreeIndex()
{
}

void KDTreeIndex::clear()
{
  tree.clear();
}

void KDTreeIndex::build(const uint8_t* coordinates, size_t num_points, uint stride)
{
  auto coordinate_for_index = [coordinates, stride](size_t index, uint8_t dimension) -> float {
    float coordinate;
    std::memcpy(&coordinate, coordinates + index*stride + dimension*sizeof(float), sizeof(float));
    return coordinate;
  };

  tree.resize(num_points);

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

  while(!stack.empty())
  {
    const range_t current_tree = (stack.end()-1)->subtree;
    const uint8_t dimension = (stack.end()-1)->dimension;
    stack.pop_back();

    boost::sort::block_indirect_sort(
          tree.data()+current_tree.begin,
          tree.data()+current_tree.end,
          [dimension, coordinate_for_index](size_t a, size_t b){return coordinate_for_index(a, dimension) < coordinate_for_index(b, dimension);});

    if(!current_tree.is_leaf())
    {
      const uint8_t next_dimension = (dimension + 1) % 3;
      const range_t left_subtree = current_tree.left_subtree();
      const range_t right_subtree = current_tree.right_subtree();
      if(!left_subtree.is_empty())
        stack.push_back(stack_element_t{left_subtree, next_dimension});
      if(!right_subtree.is_empty())
        stack.push_back(stack_element_t{right_subtree, next_dimension});
    }
  }
}

bool KDTreeIndex::range_t::is_empty() const
{
  Q_ASSERT(begin <= end);

  return begin==end;
}

bool KDTreeIndex::range_t::is_leaf() const
{
  return end-begin <= 1;
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
  return range_t{median(), end};
}
