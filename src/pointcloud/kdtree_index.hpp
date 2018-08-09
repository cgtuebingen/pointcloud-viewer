#ifndef POINTCLOUD_KDTREE_INDEX_HPP
#define POINTCLOUD_KDTREE_INDEX_HPP

#include <core_library/types.hpp>
#include <glm/glm.hpp>

#include <vector>

class KDTreeIndex
{
public:
  KDTreeIndex();
  ~KDTreeIndex();

  void clear();

  void build(const uint8_t* coordinates, size_t num_points, uint stride);

private:
  struct range_t
  {
    size_t begin, end;

    bool is_empty() const;
    bool is_leaf() const;
    size_t median() const;

    range_t left_subtree() const;
    range_t right_subtree() const;
  };

  std::vector<size_t> tree;

  static size_t median(range_t subtree);
};

#endif // POINTCLOUD_KDTREE_INDEX_HPP
