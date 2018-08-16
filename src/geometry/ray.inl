#include <geometry/ray.hpp>
#include <geometry/swizzle.hpp>

inline ray_t ray_t::from_two_points(glm::vec3 from, glm::vec3 to)
{
  ray_t r;
  r.direction = normalize(to-from);
  r.origin = from;
  return r;
}

inline glm::vec3 ray_t::get_point(float t) const
{
  return origin + direction * t;
}

inline glm::vec3 ray_t::operator[](float t) const
{
  return get_point(t);
}

inline glm::vec3 ray_t::nearest_point(glm::vec3 point) const
{
  return origin + direction * glm::max(0.f, dot(direction, point-origin));
}

inline float ray_t::distance_to(glm::vec3 point) const
{
  return distance(nearest_point(point), point);
}

inline bool ray_t::intersects_aabb(aabb_t aabb, float* intersection_distance_front, float* intersection_distance_back) const
{
  glm::bvec3 intersects1;
  glm::bvec3 intersects2;
  glm::vec3 distances1 = __intersects_aabb_intersection_with_common_point(aabb, aabb.min_point, &intersects1);
  glm::vec3 distances2 = __intersects_aabb_intersection_with_common_point(aabb, aabb.max_point, &intersects2);

  *intersection_distance_front = 0.f;
  *intersection_distance_back = 0.f;

  for(int i=0; i<3; ++i)
  {
    if(intersects1[i])
      *intersection_distance_front = glm::min(distances1[i], *intersection_distance_front);
    if(intersects2[i])
      *intersection_distance_front = glm::min(distances2[i], *intersection_distance_front);

    if(intersects1[i])
      *intersection_distance_back = glm::max(distances1[i], *intersection_distance_back);
    if(intersects2[i])
      *intersection_distance_back = glm::max(distances2[i], *intersection_distance_back);
  }

  return any(intersects1) || any(intersects2);
}

inline glm::vec3 ray_t::__intersects_aabb_intersection_with_common_point(aabb_t aabb, glm::vec3 common_point, glm::bvec3* intersects) const
{
  glm::vec3 distances = __intersection_distances_to_axis_planes(common_point);
  glm::vec3 p0 = get_point(distances[0]);
  glm::vec3 p1 = get_point(distances[1]);
  glm::vec3 p2 = get_point(distances[2]);

  *intersects = glm::bvec3((all(lessThanEqual(yz(aabb.min_point), yz(p0))) && all(lessThanEqual(yz(p0), yz(aabb.max_point)))),
                           (all(lessThanEqual(xz(aabb.min_point), xz(p1))) && all(lessThanEqual(xz(p1), xz(aabb.max_point)))),
                           (all(lessThanEqual(xy(aabb.min_point), xy(p2))) && all(lessThanEqual(xy(p2), xy(aabb.max_point)))));

  return distances;
}

inline glm::vec3 ray_t::__intersection_distances_to_axis_planes(glm::vec3 common_point) const
{
  return (common_point - origin) / direction;
}
