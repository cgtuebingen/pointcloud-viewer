namespace render_system {

class PointRenderer final
{
public:
  PointRenderer();
  ~PointRenderer();

  PointRenderer(PointRenderer&& point_renderer);
  PointRenderer& operator=(PointRenderer&& point_renderer);

  void render_points();

private:
  class Implementation;

  Implementation* implementation = nullptr;
};

} //namespace render_system
