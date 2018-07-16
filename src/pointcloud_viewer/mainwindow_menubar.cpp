#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/import_pointcloud.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud/importer/abstract_importer.hpp>

#include <QMenuBar>
#include <QMimeData>
#include <QFileDialog>

void MainWindow::initMenuBar()
{
  QMenuBar* menuBar = new QMenuBar;
  menuBar->setVisible(true);
  setMenuBar(menuBar);

  // ======== Project ==================================================================================================
  QMenu* menu_project = menuBar->addMenu("&Project");
  QAction* import_pointcloud_layers = menu_project->addAction("&Import Pointcloud");

  import_pointcloud_layers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  connect(import_pointcloud_layers, &QAction::triggered, this, &MainWindow::importPointcloudLayer);

  // ======== Flythrough ===============================================================================================
  QMenu* menu_flythrough = menuBar->addMenu("&Flythrough");
  QAction* action_flythrough_insert_keypoint = menu_flythrough->addAction("&Insert Keypoint");
  menu_flythrough->addSeparator();
  QAction* action_flythrough_export_path = menu_flythrough->addAction("&Export Path");
  QAction* action_flythrough_import_path = menu_flythrough->addAction("&Import Path");

  action_flythrough_insert_keypoint->setShortcut(QKeySequence(Qt::Key_I));
  connect(action_flythrough_insert_keypoint, &QAction::triggered, this, &MainWindow::insertKeypoint);
  connect(action_flythrough_export_path, &QAction::triggered, this, &MainWindow::exportCameraPath);
  connect(action_flythrough_import_path, &QAction::triggered, this, &MainWindow::importCameraPath);

  // ======== View =====================================================================================================
  QMenu* menu_view = menuBar->addMenu("&View");

  // -------- Navigation -----------------------------------------------------------------------------------------------
  QMenu* menu_view_navigation = menu_view->addMenu("&Navigation");
  QAction* action_view_navigation_fps = menu_view_navigation->addAction("&First Person Navigation");
  QAction* action_view_navigation_reset_camera_frame = menu_view_navigation->addAction("Reset Camera &Frame");
  QAction* action_view_navigation_reset_movement_speed = menu_view_navigation->addAction("Reset Movement &Velocity");

  action_view_navigation_fps->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F));
  connect(action_view_navigation_fps, &QAction::triggered, &viewport.navigation, &Navigation::startFpsNavigation);
  connect(action_view_navigation_reset_camera_frame, &QAction::triggered, &viewport.navigation, &Navigation::resetCameraLocation);
  connect(action_view_navigation_reset_movement_speed, &QAction::triggered, &viewport.navigation, &Navigation::resetMovementSpeed);

  // -------- Visualization --------------------------------------------------------------------------------------------
  QMenu* menu_view_visualization = menu_view->addMenu("&Visualization");
  QAction* action_view_visualization_camerapath = menu_view_visualization->addAction("&Camera Path");
  QAction* action_view_visualization_grid = menu_view_visualization->addAction("&Grid");
  QAction* action_view_visualization_axis = menu_view_visualization->addAction("&Axis");
#ifndef NDEBUG
  menu_view_visualization->addSeparator();
  QAction* action_view_visualization_debug_turntable_center = menu_view_visualization->addAction("&Axis");
#endif

  Visualization::settings_t current_settings = Visualization::settings_t::default_settings();

#define TOGGLE(item, var) \
  item->setCheckable(true); \
  item->setChecked(current_settings.var); \
  connect(item, &QAction::toggled, [this, item](){viewport.visualization().settings.var = item->isChecked(); viewport.update();});

  action_view_visualization_camerapath->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
  TOGGLE(action_view_visualization_camerapath, enable_camera_path);

  action_view_visualization_grid->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
  TOGGLE(action_view_visualization_grid, enable_grid);

  action_view_visualization_axis->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
  TOGGLE(action_view_visualization_axis, enable_axis);

#ifndef NDEBUG
  action_view_visualization_debug_turntable_center->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_1));
  TOGGLE(action_view_visualization_debug_turntable_center, enable_turntable_center);
#endif

#undef TOGGLE

  // ======== Application ==============================================================================================
  QMenu* menu_view_application = menuBar->addMenu("&Application");
  QAction* about_action = menu_view_application->addAction("&About");
  connect(about_action, &QAction::triggered, this, &MainWindow::openAboutDialog);

  setAcceptDrops(true);

}

void MainWindow::dropEvent(QDropEvent *ev) {
  QList<QUrl> urls = ev->mimeData()->urls();
  foreach (QUrl url, urls) {
    const QString file_to_import = url.path();
    if(file_to_import.isEmpty())
      return;
    viewport.load_point_cloud(import_point_cloud(this, file_to_import));
    return;
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *ev) {
  ev->accept();
}

void MainWindow::insertKeypoint()
{
  const int position_after_last = std::numeric_limits<int>::max();
  int position = position_after_last;

  flythrough.insert_keypoint(viewport.navigation.camera.frame, position);
}

void MainWindow::exportCameraPath()
{
  QString file_to_export = QFileDialog::getSaveFileName(this,
                                                        "Save camera path as",
                                                        ".",
                                                        "Camera Path (*.camera_path)");

  if(file_to_export.isEmpty())
    return;

  flythrough.export_path(file_to_export);
}

void MainWindow::importCameraPath()
{
  QString file_to_import = QFileDialog::getOpenFileName(this,
                                                        "Import camera path as",
                                                        ".",
                                                        "Camera Path (*.camera_path)");

  if(file_to_import.isEmpty())
    return;

  flythrough.import_path(file_to_import);
}

#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPlainTextEdit>
#include <QApplication>
#include <QScrollBar>

void MainWindow::importPointcloudLayer()
{
  QString file_to_import = QFileDialog::getOpenFileName(this,
                                                         "Select pointcloud to import",
                                                         ".",
                                                         AbstractPointCloudImporter::allSupportedFiletypes());

  if(file_to_import.isEmpty())
    return;

  viewport.load_point_cloud(import_point_cloud(this, file_to_import));
}

extern const QString pcl_notes;
extern const QString pcl_license;

void MainWindow::openAboutDialog()
{
  QDialog aboutDialog;
  aboutDialog.setModal(true);
  aboutDialog.setSizeGripEnabled(true);

  QVBoxLayout* layout = new QVBoxLayout;
  aboutDialog.setLayout(layout);

  auto add_note = [&](QString info, QString details){
    QLabel* label = new QLabel(info);
    QPlainTextEdit* plain_text_edit = new QPlainTextEdit(details);

    plain_text_edit->setWordWrapMode(QTextOption::NoWrap);
    plain_text_edit->setReadOnly(true);

    layout->addWidget(label);
    layout->addWidget(plain_text_edit);
    layout->addSpacing(22);
  };

  layout->addWidget(new QLabel("Dependencies:"));
  layout->addSpacing(22);

  add_note(pcl_notes, pcl_license);

  aboutDialog.exec();
}

const QString pcl_notes = "The Point Cloud Library (PCL) is used for:\n"
                          "- Loading PLY files";
const QString pcl_license = "Software License Agreement (BSD License)\n"
                            "\n"
                            "Point Cloud Library (PCL) - www.pointclouds.org\n"
                            "Copyright (c) 2009-2012, Willow Garage, Inc.\n"
                            "Copyright (c) 2012-, Open Perception, Inc.\n"
                            "Copyright (c) XXX, respective authors.\n"
                            "\n"
                            "All rights reserved.\n"
                            "\n"
                            "Redistribution and use in source and binary forms, with or without\n"
                            "modification, are permitted provided that the following conditions\n"
                            "are met: \n"
                            "\n"
                            " * Redistributions of source code must retain the above copyright\n"
                            "   notice, this list of conditions and the following disclaimer.\n"
                            " * Redistributions in binary form must reproduce the above\n"
                            "   copyright notice, this list of conditions and the following\n"
                            "   disclaimer in the documentation and/or other materials provided\n"
                            "   with the distribution.\n"
                            " * Neither the name of the copyright holder(s) nor the names of its\n"
                            "   contributors may be used to endorse or promote products derived\n"
                            "   from this software without specific prior written permission.\n"
                            "\n"
                            "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
                            "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
                            "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n"
                            "FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE\n"
                            "COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
                            "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n"
                            "BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
                            "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
                            "CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n"
                            "LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n"
                            "ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n"
                            "POSSIBILITY OF SUCH DAMAGE.\n";
