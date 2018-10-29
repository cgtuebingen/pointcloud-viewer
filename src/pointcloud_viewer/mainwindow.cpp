#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
  : kdTreeInspector(this),
    pointCloudInspector(&viewport),
    pointShaderEditor(this)
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  initMenuBar();
  initDocks();

  connect(&flythrough, &Flythrough::set_new_camera_frame, &viewport, &Viewport::set_camera_frame);
  connect(&viewport, &Viewport::frame_rendered, &flythrough.playback, &Playback::previous_frame_finished);
  connect(&viewport, &Viewport::openGlContextCreated, this, &MainWindow::handleApplicationArguments);

  connect(&viewport.navigation, &Navigation::picked_point, &pointCloudInspector, &PointCloudInspector::pick_point);
  connect(&viewport, &Viewport::pointSizeChanged, &pointCloudInspector, &PointCloudInspector::setPickRadius);

  connect(this, &MainWindow::pointcloud_unloaded, [this](){
    pointcloud.clear();
    loadedShader = PointCloud::Shader();
    viewport.unload_all_point_clouds();
    pointShaderEditor.unload_all_point_clouds();
    kdTreeInspector.unload_all_point_clouds();
    pointCloudInspector.unload_all_point_clouds();
  });
  connect(this, &MainWindow::pointcloud_imported, [this](QSharedPointer<PointCloud> p){
    pointcloud = p;
    viewport.load_point_cloud(p);
    pointShaderEditor.load_point_cloud(p);
    kdTreeInspector.handle_new_point_cloud(p);
    pointCloudInspector.handle_new_point_cloud(p);
    viewport.navigation.handle_new_point_cloud();
    if(glm::any(glm::isnan(p->vertex(0).coordinate)))
      this->apply_point_shader(p->shader);
    loadedShader = p->shader;
  });
}

MainWindow::~MainWindow()
{
}

bool MainWindow::apply_point_shader(PointCloud::Shader new_shader)
{
  if(this->pointcloud==nullptr)
    return false;

  PointCloud::Shader old_shader = this->pointcloud->shader;
  this->pointcloud->shader = new_shader;

  const bool coordinates_changed = new_shader.coordinate_expression == new_shader.coordinate_expression;
  const bool colors_changed = new_shader.color_expression == new_shader.color_expression;

  const bool needs_being_rebuilt_for_the_first_time = glm::any(glm::isnan(this->pointcloud->vertex(0).coordinate));
  const bool had_some_changes = coordinates_changed || colors_changed;

  if(!needs_being_rebuilt_for_the_first_time && !had_some_changes)
    return false;

  if(this->pointcloud->shader.is_empty())
    this->pointcloud->shader = pointShaderEditor.autogenerate();

  if(!viewport.reapply_point_shader(coordinates_changed))
    return false;

  // update the selected point
  pointCloudInspector.update();

  return true;
}
