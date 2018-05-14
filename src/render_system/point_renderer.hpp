#include <core_library/macros.hpp>

namespace render_system {

class PointRenderer final
{
public:
  nocopy(PointRenderer)

  PointRenderer();
  ~PointRenderer();

  void render_points();

private:
};

} //namespace render_system
