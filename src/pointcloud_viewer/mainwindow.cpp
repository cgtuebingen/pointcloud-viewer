#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
  : kdTreeInspector(this),
    pointCloudInspector(&viewport)
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
  });
  connect(this, &MainWindow::pointcloud_imported, [this](QSharedPointer<PointCloud> p){
    pointcloud = p;
    if(glm::any(glm::isnan(p->vertex(0).coordinate)))
      this->apply_point_shader(p->shader);
    loadedShader = p->shader;
  });

  connect(this, &MainWindow::pointcloud_unloaded, &pointShaderEditor, &PointShaderEditor::unload_all_point_clouds);
  connect(this, &MainWindow::pointcloud_unloaded, &viewport, &Viewport::unload_all_point_clouds);
  connect(this, &MainWindow::pointcloud_unloaded, &kdTreeInspector, &KdTreeInspector::unload_all_point_clouds);
  connect(this, &MainWindow::pointcloud_unloaded, &pointCloudInspector, &PointCloudInspector::unload_all_point_clouds);

  connect(this, &MainWindow::pointcloud_imported, &pointShaderEditor, &PointShaderEditor::load_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &viewport, &Viewport::load_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &kdTreeInspector, &KdTreeInspector::handle_new_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &pointCloudInspector, &PointCloudInspector::handle_new_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &viewport.navigation, &Navigation::handle_new_point_cloud);
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
  const bool had_some_changes = !coordinates_changed || !colors_changed || new_shader.is_empty();

  if(!needs_being_rebuilt_for_the_first_time && had_some_changes)
    return false;

  if(!viewport.reapply_point_shader())
    return false;

  // TODO: the called should update the aabb, if the coordinates where changed
  // TODO: the called should rebuild the KD tree, if the coordinates where changed

  // update the selected point
  pointCloudInspector.update();

  return true;
}
