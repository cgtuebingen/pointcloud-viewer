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

    subtree_t(const subtree_t&) = default;
    subtree_t(subtree_t&&) = default;
    subtree_t& operator=(const subtree_t&) = default;
    subtree_t& operator=(subtree_t&&) = default;

    size_t root() const{return range.median();}
    size_t is_leaf() const{return range.is_leaf();}
    subtree_t left_subtree() const {return subtree(range.left_subtree());}
    subtree_t right_subtree() const {return subtree(range.right_subtree());}

  private:
    subtree_t subtree(range_t range) const;
  };

  aabb_t total_aabb;
  std::vector<size_t> tree;

  subtree_t traverse_kd_tree_to_point(size_t point, std::function<void(subtree_t inner_subtree)> visitor) const;
  subtree_t whole_tree() const;

  void validate_tree();
};

#endif // POINTCLOUD_KDTREE_INDEX_HPP
