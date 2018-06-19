#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/import_pointcloud.hpp>

#include <QMenuBar>
#include <QFileDialog>

void MainWindow::initMenuBar()
{
  QMenuBar* menuBar = new QMenuBar;
  menuBar->setVisible(true);
  setMenuBar(menuBar);

  QMenu* menu_project = menuBar->addMenu("&Project");
  QAction* import_pointcloud_layers = menu_project->addAction("&Import Pointcloud Layers");

  QMenu* menu_view = menuBar->addMenu("&View");
  QMenu* menu_view_navigation = menu_view->addMenu("&Navigation");
  QAction* action_view_navigation_fps = menu_view_navigation->addAction("&First Person Navigation");

  action_view_navigation_fps->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F));
  connect(action_view_navigation_fps, &QAction::triggered, &viewport.navigation, &Navigation::startFpsNavigation);

  import_pointcloud_layers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  connect(import_pointcloud_layers, &QAction::triggered, this, &MainWindow::importPointcloudLayer);
}

void MainWindow::importPointcloudLayer()
{
  QStringList files_to_import = QFileDialog::getOpenFileNames(this,
                                                              "Select one or more pointcloud layers to import",
                                                              ".",
                                                              "Any Supported (*.ply *.obj);;PLY (*.ply);;OBJ (*.obj)");

  for(QString file_to_import : files_to_import)
  {
    viewport.load_point_cloud(import_point_cloud(this, file_to_import));
  }
}
