#ifndef POINTCLOUD_KDTREE_INDEX_HPP
#define POINTCLOUD_KDTREE_INDEX_HPP

#include <core_library/types.hpp>
#include <geometry/aabb.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <functional>

class KDTreeIndex
{
public:
  KDTreeIndex();
  ~KDTreeIndex();

  size_t root_point() const;
  bool has_children(size_t point) const;
  std::pair<size_t, size_t> children_of(size_t point) const;
  size_t parent_of(size_t point) const;
  std::pair<aabb_t, aabb_t> aabbs_split_by(size_t point, const uint8_t* coordinates, uint stride) const;
  glm::vec3 point_coordinate(size_t point, const uint8_t* coordinates, uint stride) const;

  void clear();

  void build(aabb_t total_aabb, const uint8_t* coordinates, size_t num_points, uint stride, std::function<bool(size_t, size_t)> feedback);

  bool is_initialized() const;

private:
  struct range_t
  {
    size_t begin, end;

    size_t median() const;

    bool is_empty() const;
    bool is_leaf() const;
    size_t size() const;

    range_t left_subtree() const;
    range_t right_subtree() const;
  };

  struct subtree_t
  {
    range_t range;
    uint8_t split_dimension;

    size_t root() const{return range.median();}
    subtree_t left_subtree() const {return subtree_t{range.left_subtree(), uint8_t((split_dimension+1)%3)};}
    subtree_t right_subtree() const {return subtree_t{range.right_subtree(), uint8_t((split_dimension+1)%3)};}
  };

  aabb_t total_aabb;
  std::vector<size_t> tree;

  subtree_t traverse_kd_tree_to_point(size_t point, std::function<void(subtree_t inner_subtree)> visitor) const;
  subtree_t whole_tree() const;
};

#endif // POINTCLOUD_KDTREE_INDEX_HPP
