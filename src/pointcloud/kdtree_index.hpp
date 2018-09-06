#ifndef POINTCLOUD_KDTREE_INDEX_HPP
#define POINTCLOUD_KDTREE_INDEX_HPP

#include <core_library/types.hpp>
#include <geometry/aabb.hpp>
#include <geometry/cone.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <functional>

/**
Representation of an Kd-Tree of all points.

This allowes picking single points.
*/
class KDTreeIndex
{
public:
  enum class point_index_t : size_t {INVALID = std::numeric_limits<size_t>::max()};
  typedef point_index_t POINT_INDEX;

  KDTreeIndex();
  ~KDTreeIndex();

  point_index_t pick_point(cone_t cone, const uint8_t* coordinates, uint stride, point_index_t fallback=POINT_INDEX::INVALID) const;

  size_t root_point() const;
  bool has_children(size_t point) const;
  std::pair<size_t, size_t> children_of(size_t point) const;
  size_t parent_of(size_t point) const;
  std::pair<aabb_t, aabb_t> aabbs_split_by(size_t point, const uint8_t* coordinates, uint stride) const;
  glm::vec3 point_coordinate(size_t point, const uint8_t* coordinates, uint stride) const;

  void clear();

  void build(aabb_t total_aabb, const uint8_t* coordinates, size_t num_points, uint stride, std::function<bool(size_t, size_t)> feedback);

  bool is_initialized() const;

  const point_index_t* data() const;

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
    size_t is_empty() const{return range.is_empty();}
    subtree_t left_subtree() const {return subtree(range.left_subtree());}
    subtree_t right_subtree() const {return subtree(range.right_subtree());}

  private:
    subtree_t subtree(range_t range) const;
  };

  aabb_t total_aabb;
  std::vector<point_index_t> tree;

  subtree_t traverse_kd_tree_to_point(size_t point, std::function<void(subtree_t inner_subtree)> visitor) const;
  subtree_t whole_tree() const;

  void validate_tree(const uint8_t* coordinates, size_t num_points, uint stride);

  static float component_for_index(point_index_t point_index, uint8_t dimension, const uint8_t* coordinates, uint stride);
  float component_for_index(size_t entry_index, uint8_t dimension, const uint8_t* coordinates, uint stride) const;
  static glm::vec3 coordinate_for_index(point_index_t point_index, const uint8_t* coordinates, uint stride);
  glm::vec3 coordinate_for_index(size_t entry_index, const uint8_t* coordinates, uint stride) const;
};

#endif // POINTCLOUD_KDTREE_INDEX_HPP
