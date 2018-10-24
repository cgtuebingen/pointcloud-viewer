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

  connect(&pointCloudInspector, &PointCloudInspector::hasSelectedPointChanged, [this](bool has) {
    if(!has)
      viewport.navigation.unsetSelectedPoint();
  });
  connect(&pointCloudInspector, &PointCloudInspector::selected_point, [this](glm::vec3 coordinate, glm::u8vec3, PointCloud::UserData) {
    viewport.navigation.setSelectedPoint(coordinate);
  });
}

MainWindow::~MainWindow()
{
}
