#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/offline_renderer_dialogs.hpp>

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

  // ---- background ----
  QSpinBox* backgroundBrightness = new QSpinBox;
  backgroundBrightness->setMinimum(0);
  backgroundBrightness->setMaximum(255);
  backgroundBrightness->setValue(viewport.backgroundColor());
  connect(&viewport, &Viewport::backgroundColorChanged, backgroundBrightness, &QSpinBox::setValue);
  connect(backgroundBrightness, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &viewport, &Viewport::setBackgroundColor);

  // ---- background ----
  QSpinBox* pointSize = new QSpinBox;
  pointSize->setMinimum(1);
  pointSize->setMaximum(16);
  pointSize->setValue(viewport.pointSize());
  connect(&viewport, &Viewport::pointSizeChanged, pointSize, &QSpinBox::setValue);
  connect(pointSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &viewport, &Viewport::setPointSize);

  // ---- render button ----
  QPushButton* renderButton = new QPushButton("&Render");
  connect(renderButton, &QPushButton::clicked, this, &MainWindow::offline_render);
  connect(&flythrough, &Flythrough::canPlayChanged, renderButton, &QPushButton::setEnabled);
  renderButton->setEnabled(flythrough.canPlay());

  // ==== layout ====
  QFormLayout* form;

  // -- animation group --
  QGroupBox* animationGroup = new QGroupBox("Animation");
  animationGroup->setLayout((form = new QFormLayout));

  form->addRow("Duration:", animationDuration);
  form->addRow("Velocity:", cameraVelocity);
  form->addRow("Interpolation:", interpolation);
  form->addRow(play_animation_realtime, new QWidget());

  // -- render style --
  QGroupBox* renderGroup = new QGroupBox("Render");
  renderGroup->setLayout((form = new QFormLayout));

  form->addRow("Background:", backgroundBrightness);
  form->addRow("Point Size:", pointSize);
  form->addRow(renderButton);

  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);
  vbox->addWidget(keypointList);
  vbox->addWidget(animationGroup);
  vbox->addWidget(renderGroup);
}

void MainWindow::jumpToKeypoint(const QModelIndex& modelIndex)
{
  if(!modelIndex.isValid())
    return;

  viewport.set_camera_frame(flythrough.keypoint_at(modelIndex.row()).frame);
}
