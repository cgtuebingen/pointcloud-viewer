#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/offline_renderer_dialogs.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud_viewer/keypoint_list.hpp>

#include <QDockWidget>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QGroupBox>
#include <QTabWidget>

void MainWindow::initDocks()
{
  QDockWidget* dataInspectionDock = initDataInspectionDock();
  QDockWidget* animationDock = initAnimationDock();
  QDockWidget* renderDock = initRenderDock();

  tabifyDockWidget(dataInspectionDock, animationDock);
  tabifyDockWidget(animationDock, renderDock);
}

void remove_focus_after_enter(QAbstractSpinBox* w);

QDockWidget* MainWindow::initAnimationDock()
{
  QDockWidget* dock = new QDockWidget("Flythrough", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  // ==== Animation Tab ====
  QWidget* root = new QWidget;
  dock->setWidget(root);

  // ---- keypoint list ----
  keypointList = new KeypointList;
  keypointList->setModel(&flythrough);

  connect(keypointList, &QListView::doubleClicked, this, &MainWindow::jumpToKeypoint);
  auto update_path_visualization = [this](){
    viewport.visualization().set_path(flythrough.all_keypoints(), keypointList->currentIndex().row());
    viewport.update();
  };
  connect(keypointList, &KeypointList::currentKeypointChanged, update_path_visualization);
  connect(keypointList, &KeypointList::on_delete_keypoint, &flythrough, &Flythrough::delete_keypoint);
  connect(keypointList, &KeypointList::on_move_keypoint_up, &flythrough, &Flythrough::move_keypoint_up);
  connect(keypointList, &KeypointList::on_move_keypoint_down, &flythrough, &Flythrough::move_keypoint_down);
  connect(keypointList, &KeypointList::on_insert_keypoint, [this](int index) {
    flythrough.insert_keypoint(this->viewport.navigation.camera.frame, index);
    keypointList->setCurrentIndex(flythrough.index(index, 0));
  });
  connect(&flythrough, &Flythrough::pathChanged, update_path_visualization);

  // ---- animation duration ----
  QDoubleSpinBox* animationDuration = new QDoubleSpinBox;
  animationDuration->setSuffix("s");
  animationDuration->setDecimals(2);

  animationDuration->setValue(flythrough.animationDuration());
  animationDuration->setMinimum(0.01);
  animationDuration->setMaximum(3600);
  connect(animationDuration, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          &flythrough, &Flythrough::setAnimationDuration);
  connect(&flythrough, &Flythrough::animationDurationChanged,
          animationDuration, &QDoubleSpinBox::setValue);

  // ---- camera velocity ----
  QDoubleSpinBox* cameraVelocity = new QDoubleSpinBox;
  cameraVelocity->setSuffix("unit/s");
  cameraVelocity->setDecimals(2);
  cameraVelocity->setSingleStep(0.1);

  cameraVelocity->setValue(flythrough.cameraVelocity());
  cameraVelocity->setMinimum(0.01);
  cameraVelocity->setMaximum(3600);
  connect(cameraVelocity, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          &flythrough, &Flythrough::setCameraVelocity);
  connect(&flythrough, &Flythrough::cameraVelocityChanged,
          cameraVelocity, &QDoubleSpinBox::setValue);

  // ---- interpolation ----
  QComboBox* interpolation = new QComboBox;
  {
    QMap<int, QString> items;
    items[Flythrough::INTERPOLATION_LINEAR] = "Linear";
    items[Flythrough::INTERPOLATION_LINEAR_SMOOTHSTEP] = "Linear Smoothstep";
    interpolation->addItems(items.values());
  }
  interpolation->setCurrentIndex(flythrough.interpolation());
  connect(interpolation, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), &flythrough, &Flythrough::setInterpolation);
  connect(&flythrough, &Flythrough::interpolationChanged, interpolation, &QComboBox::setCurrentIndex);

  // ---- play ----
  QPushButton* play_animation_realtime = new QPushButton("&Play");
  connect(play_animation_realtime, &QPushButton::clicked, &flythrough.playback, &Playback::play_realtime);
  connect(&flythrough, &Flythrough::canPlayChanged, play_animation_realtime, &QPushButton::setEnabled);
  play_animation_realtime->setEnabled(flythrough.canPlay());

  // ---- mouse sensitivity ----
  QSpinBox* mouseSensitivity = new QSpinBox;
  remove_focus_after_enter(mouseSensitivity);
  mouseSensitivity->setMinimum(viewport.navigation.mouse_sensitivity_value_range()[0]);
  mouseSensitivity->setMaximum(viewport.navigation.mouse_sensitivity_value_range()[1]);

  mouseSensitivity->setValue(viewport.navigation.mouse_sensitivity_value());
  connect(mouseSensitivity, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &viewport.navigation, &Navigation::set_mouse_sensitivity_value);
  connect(&viewport.navigation, &Navigation::mouse_sensitivity_value_changed, mouseSensitivity, &QSpinBox::setValue);

  // ==== layout ====
  QFormLayout* form;

  // -- Keypoints --
  QGroupBox* keypointsGroup = new QGroupBox("Keypoints");
  keypointsGroup->setLayout((form = new QFormLayout));
  form->addWidget(keypointList);

  // -- playback group --
  QGroupBox* playbackGroup = new QGroupBox("Playback");
  playbackGroup->setLayout((form = new QFormLayout));

  form->addRow("Duration:", animationDuration);
  form->addRow("Velocity:", cameraVelocity);
  form->addRow("Interpolation:", interpolation);
  form->addRow(play_animation_realtime, new QWidget());

  // -- navigation --
  QGroupBox* navigationGroup = new QGroupBox("Navigation");
  navigationGroup->setLayout((form = new QFormLayout));

  form->addRow("Mouse Sensitivity:", mouseSensitivity);

  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);
  vbox->addWidget(keypointsGroup);
  vbox->addWidget(playbackGroup);
  vbox->addWidget(navigationGroup);

  return dock;
}

QDockWidget* MainWindow::initDataInspectionDock()
{
  QDockWidget* dock = new QDockWidget("Data Inspection", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  // ==== Data Inspection Tab ====
  QWidget* root = new QWidget;
  dock->setWidget(root);

  // -- unlock picker --
  QPushButton* unlockButton = new QPushButton("Unlock Point &Picker", this);
  unlockButton->setEnabled(pointDataInspector.canBuildKdTree());
  QObject::connect(&pointDataInspector, &PointCloudInspector::canBuildKdTreeChanged, unlockButton, &QPushButton::setEnabled);
  QObject::connect(unlockButton, &QPushButton::clicked, &pointDataInspector, &PointCloudInspector::build_kdtree);


  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);
  vbox->addWidget(unlockButton);

  return dock;
}

QDockWidget* MainWindow::initRenderDock()
{
  QDockWidget* dock = new QDockWidget("Render", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  // ==== Render ====
  QWidget* root = new QWidget;
  dock->setWidget(root);

  // ---- render button ----
  QPushButton* renderButton = new QPushButton("&Render");
  connect(renderButton, &QPushButton::clicked, this, &MainWindow::offline_render_with_ui);
  connect(&flythrough, &Flythrough::canPlayChanged, renderButton, &QPushButton::setEnabled);
  renderButton->setEnabled(flythrough.canPlay());

  // ---- background ----
  QSpinBox* backgroundBrightness = new QSpinBox;
  remove_focus_after_enter(backgroundBrightness);
  backgroundBrightness->setMinimum(0);
  backgroundBrightness->setMaximum(255);
  backgroundBrightness->setValue(viewport.backgroundColor());
  backgroundBrightness->setToolTip("The brightness of the gray in the background (default: 54)");
  connect(&viewport, &Viewport::backgroundColorChanged, backgroundBrightness, &QSpinBox::setValue);
  connect(backgroundBrightness, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &viewport, &Viewport::setBackgroundColor);

  // ---- pointSize ----
  QSpinBox* pointSize = new QSpinBox;
  remove_focus_after_enter(pointSize);
  pointSize->setMinimum(1);
  pointSize->setMaximum(16);
  pointSize->setValue(int(viewport.pointSize()));
  pointSize->setToolTip("The size of a sprite to draw a point in pixels (default: 1)");
  connect(&viewport, &Viewport::pointSizeChanged, pointSize, &QSpinBox::setValue);
  connect(pointSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &viewport, &Viewport::setPointSize);

  // -- render style --
  QGroupBox* styleGroup = new QGroupBox("Style");
  QFormLayout* form = new QFormLayout;
  styleGroup->setLayout((form));

  form->addRow("Background:", backgroundBrightness);
  form->addRow("Point Size:", pointSize);

  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);

  vbox->addWidget(renderButton);
  vbox->addWidget(styleGroup);

  return dock;
}

void MainWindow::jumpToKeypoint(const QModelIndex& modelIndex)
{
  if(!modelIndex.isValid())
    return;

  viewport.set_camera_frame(flythrough.keypoint_at(modelIndex.row()).frame);
}

void remove_focus_after_enter(QAbstractSpinBox* w)
{
  QObject::connect(w, &QSpinBox::editingFinished, [w](){w->clearFocus();});
}
