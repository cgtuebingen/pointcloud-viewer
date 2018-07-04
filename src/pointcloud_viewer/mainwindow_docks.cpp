#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/offline_renderer.hpp>

#include <QDockWidget>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>

void MainWindow::initDocks()
{
  initKeypointListDocks();
}

void MainWindow::initKeypointListDocks()
{
  QDockWidget* dock = new QDockWidget("Flythrough Keypoints", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  QWidget* root = new QWidget;
  dock->setWidget(root);

  // ---- keypoint list ----
  keypointList = new QListView;
  keypointList->setModel(&flythrough);

  connect(keypointList, &QListView::doubleClicked, this, &MainWindow::jumpToKeypoint);

  // ---- animation duration ----
  QDoubleSpinBox* animationDuration = new QDoubleSpinBox;
  animationDuration->setSuffix("s");
  animationDuration->setDecimals(2);

  animationDuration->setValue(flythrough.animationDuration());
  animationDuration->setMinimum(0.01);
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

  // ---- render button ----
  QPushButton* renderButton = new QPushButton("&Render");
  connect(renderButton, &QPushButton::clicked, this, &MainWindow::offline_render);

  // ==== layout ====
  QFormLayout* form;

  // -- animation group --
  QGroupBox* animationGroup = new QGroupBox("Animation");
  animationGroup->setLayout((form = new QFormLayout));

  form->addRow("Duration:", animationDuration);
  form->addRow("Velocity:", cameraVelocity);
  form->addRow("Interpolation:", interpolation);
  form->addRow(play_animation_realtime, new QWidget());

  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);
  vbox->addWidget(keypointList);
  vbox->addWidget(animationGroup);
  vbox->addWidget(renderButton);
}

void MainWindow::jumpToKeypoint(const QModelIndex& modelIndex)
{
  if(!modelIndex.isValid())
    return;

  viewport.set_camera_frame(flythrough.keypoint_at(modelIndex.row()).frame);
}

void MainWindow::offline_render()
{
  QPair<RenderSettings, bool> result = ask_for_render_settings(renderSettings);

  renderSettings = result.first;
  bool was_canceled = result.second;

  if(was_canceled)
    return;

  ::render(this, renderSettings);
}
