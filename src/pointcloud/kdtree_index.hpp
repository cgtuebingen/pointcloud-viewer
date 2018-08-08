#ifndef POINTCLOUD_KDTREE_INDEX_HPP
#define POINTCLOUD_KDTREE_INDEX_HPP

#include <core_library/types.hpp>

class KDTreeIndex
{
public:
  KDTreeIndex();
  ~KDTreeIndex();

  void clear();

  void build(const int8_t* coordinates, int stride);
};

#endif // POINTCLOUD_KDTREE_INDEX_HPP
