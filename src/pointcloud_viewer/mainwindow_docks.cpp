#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/offline_renderer_dialogs.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud_viewer/keypoint_list.hpp>
#include <pointcloud_viewer/widgets/rgb_edit.hpp>
#include <core_library/color_palette.hpp>
#include <core_library/print.hpp>

#include <QGridLayout>
#include <QApplication>
#include <QAction>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QGroupBox>
#include <QTabWidget>
#include <QCheckBox>
#include <QToolButton>
#include <QSettings>
#include <QLabel>
#include <QDebug>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>

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

  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);

  // -- unlock picker --
  QPushButton* unlockButton = new QPushButton("Unlock Point &Picker", this);
  unlockButton->setEnabled(kdTreeInspector.canBuildKdTree());
  QObject::connect(&kdTreeInspector, &KdTreeInspector::canBuildKdTreeChanged, unlockButton, &QPushButton::setEnabled);
  QObject::connect(unlockButton, &QPushButton::clicked, &kdTreeInspector, &KdTreeInspector::build_kdtree);
  vbox->addWidget(unlockButton);

  QCheckBox* autoUnlockButton = new QCheckBox("&Automatically Unlock after loading", this);
  autoUnlockButton->setChecked(kdTreeInspector.autoBuildKdTreeAfterLoading());
  QObject::connect(autoUnlockButton, &QCheckBox::toggled, &kdTreeInspector, &KdTreeInspector::setAutoBuildKdTreeAfterLoading);
  QObject::connect(&kdTreeInspector, &KdTreeInspector::canBuildKdTreeChanged, unlockButton, &QCheckBox::setChecked);
  vbox->addWidget(autoUnlockButton);

  vbox->addSpacing(16);

  // -- selected point --
  QGroupBox* selected_point_groupbox = new QGroupBox("Selected Point");
  selected_point_groupbox->setEnabled(pointCloudInspector.hasSelectedPoint());
  QObject::connect(&pointCloudInspector, &PointCloudInspector::hasSelectedPointChanged, selected_point_groupbox, &QWidget::setEnabled);
  vbox->addWidget(selected_point_groupbox);
  {
    QVBoxLayout* vbox = new QVBoxLayout(selected_point_groupbox);
    QLabel* x = new QLabel;
    QLabel* y = new QLabel;
    QLabel* z = new QLabel;

    QHBoxLayout* row;

    row = new QHBoxLayout;
    vbox->addLayout(row);
    row->addWidget(new QLabel(QString("color:")));
    QLabel* color = new QLabel;
    color->setMinimumHeight(32);
    color->setAlignment(Qt::AlignCenter);
    color->setTextFormat(Qt::PlainText);
    color->setTextInteractionFlags(Qt::TextSelectableByMouse);
    row->addWidget(color, 1);

    row = new QHBoxLayout;
    vbox->addLayout(row);

    row->addWidget(new QLabel(QString("<i>x:</i>")));
    row->addWidget(x, 1);
    x->setTextFormat(Qt::PlainText);
    x->setTextInteractionFlags(Qt::TextSelectableByMouse);

    row->addWidget(new QLabel(QString("<i>y:</i>")));
    row->addWidget(y, 1);
    y->setTextFormat(Qt::PlainText);
    y->setTextInteractionFlags(Qt::TextSelectableByMouse);

    row->addWidget(new QLabel(QString("<i>z:</i>")));
    row->addWidget(z, 1);
    z->setTextFormat(Qt::PlainText);
    z->setTextInteractionFlags(Qt::TextSelectableByMouse);

    row = new QHBoxLayout;
    vbox->addLayout(row);
    QLabel* labelUserData = new QLabel;
    row->addWidget(labelUserData);

    row = new QHBoxLayout;
    vbox->addLayout(row);
    row->addStretch(1);
    static QString plyNamesToCopy;
    QPushButton* btnCopyNames = new QPushButton("Copy &Names");
    row->addWidget(btnCopyNames);
    btnCopyNames->setEnabled(false);
    connect(btnCopyNames, &QPushButton::clicked, [](){
      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setText(plyNamesToCopy);
    });
    static QString plyValuesToCopy;
    QPushButton* btnCopyValues = new QPushButton("Copy &Values");
    row->addWidget(btnCopyValues);
    btnCopyValues->setEnabled(false);
    connect(btnCopyValues, &QPushButton::clicked, [](){
      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setText(plyValuesToCopy);
    });

    QObject::connect(&pointCloudInspector, &PointCloudInspector::deselect_picked_point, [x, y, z, color, btnCopyValues, btnCopyNames, labelUserData]() {
      x->setText(QString());
      y->setText(QString());
      z->setText(QString());
      color->setText(QString("#------"));
      color->setStyleSheet(QString());

      labelUserData->setText(QString());

      btnCopyNames->setEnabled(false);
      btnCopyValues->setEnabled(false);
    });

    QObject::connect(&pointCloudInspector, &PointCloudInspector::selected_point, [x, y, z, color, btnCopyNames, btnCopyValues, labelUserData](glm::vec3 coordinate, glm::u8vec3 _color, PointCloud::UserData userData) {
      auto format_float = [](float f) -> QString {
        QString s;
        s.setNum(f);
        return s.toHtmlEscaped();
      };

      x->setText(format_float(coordinate.x));
      y->setText(format_float(coordinate.y));
      z->setText(format_float(coordinate.z));

      const Color pointColor(_color);
      QString colorCode = pointColor.hexcode();
      color->setText(colorCode);
      color->setStyleSheet(QString("QLabel{background: %0; color: %1}").arg(colorCode).arg(glm::vec3(pointColor.with_saturation(0.)).g > 0.4f ? "#000000" : "#ffffff"));

      QString userDataOnly;

      plyNamesToCopy.clear();
      plyValuesToCopy.clear();
      for(int i=0; i<userData.values.length(); ++i)
      {
        static QSet<QString> standardNames({"x", "y", "z", "red", "green", "blue"});
        if(!standardNames.contains(userData.names[i]))
          userDataOnly += (userDataOnly.isEmpty() ? "" : "\n") + userData.names[i] + ": " + userData.values[i].toString();
        plyNamesToCopy += (i!=0 ?  " " : "") + userData.names[i];
        plyValuesToCopy += (i!=0 ?  " " : "") + userData.values[i].toString();
      }

      labelUserData->setText(userDataOnly);
      btnCopyNames->setEnabled(true);
      btnCopyValues->setEnabled(true);
    });
  }

  // -- count points --
  QGroupBox* count_points_groupbox = new QGroupBox("Count Points");
  vbox->addWidget(count_points_groupbox);
  {
    QVBoxLayout* vbox = new QVBoxLayout(count_points_groupbox);
    QHBoxLayout* hbox = new QHBoxLayout;

    RgbEdit* rgbEdit = new RgbEdit;
    QLabel* result = new QLabel;
    QPushButton* searchButton = new QPushButton;

    hbox->addWidget(rgbEdit);
    hbox->addWidget(searchButton);

    vbox->addLayout(hbox);
    vbox->addWidget(result);

    connect(rgbEdit, &RgbEdit::editingFinished, searchButton, &QPushButton::click);

    searchButton->setText("Count &Color");
    searchButton->setToolTip("Count the number of points with the givwn color");
    connect(searchButton, &QPushButton::clicked, [rgbEdit, result, this](){
      int n=0;
      glm::u8vec3 rgb = rgbEdit->rgb();
      if(pointcloud != nullptr)
        for(PointCloud::vertex_t v : *pointcloud)
          n += v.color == rgb;

      result->setText(QString("Found <b>%0</b> points with the color %1").arg(n).arg(Color(rgb).hexcode()));
    });

    auto reset_result = [result](){result->setText("--");};
    reset_result();
    result->setAlignment(Qt::AlignCenter);

    connect(this, &MainWindow::pointcloud_unloaded, reset_result);
    connect(this, &MainWindow::pointcloud_imported, reset_result);
  }

  // -- debug Kd-Tree --
#ifndef NDEBUG
  Visualization::settings_t current_settings = Visualization::settings_t::default_settings();

  QGroupBox* debug_kd_groupbox = new QGroupBox("Debug Kd-Tree");
  debug_kd_groupbox->setEnabled(kdTreeInspector.hasKdTreeAvailable());
  QObject::connect(&kdTreeInspector, &KdTreeInspector::hasKdTreeAvailableChanged, debug_kd_groupbox, &QWidget::setEnabled);
  vbox->addWidget(debug_kd_groupbox);
  {
    QGridLayout* grid = new QGridLayout(debug_kd_groupbox);
    QCheckBox* checkBox = new QCheckBox("Enable");
    checkBox->setChecked(current_settings.enable_kdtree_as_aabb);
    connect(checkBox, &QCheckBox::toggled, [this](bool checked){
      viewport.visualization().settings.enable_kdtree_as_aabb = checked;
      viewport.update();
    });
    grid->addWidget(checkBox, 0,0, 1,5);

    const QStyle* style = QApplication::style();

    QAction* move_up = new QAction(style->standardIcon(QStyle::SP_ArrowUp), "Move Up", this);
    QAction* move_down = new QAction(style->standardIcon(QStyle::SP_ArrowDown), "Move Down", this);
    QAction* move_left = new QAction(style->standardIcon(QStyle::SP_ArrowLeft), "Move Left", this);
    QAction* move_right = new QAction(style->standardIcon(QStyle::SP_ArrowRight), "Move Right", this);

    for(QAction* a : {move_up, move_down, move_left, move_right})
    {
      a->setEnabled(kdTreeInspector.hasKdTreeAvailable());
      QObject::connect(&kdTreeInspector, &KdTreeInspector::hasKdTreeAvailableChanged, a, &QAction::setEnabled);
    }

    QToolButton* btn_up = new QToolButton;
    QToolButton* btn_down = new QToolButton;
    QToolButton* btn_left = new QToolButton;
    QToolButton* btn_right = new QToolButton;

    btn_up->setDefaultAction(move_up);
    btn_down->setDefaultAction(move_down);
    btn_left->setDefaultAction(move_left);
    btn_right->setDefaultAction(move_right);

    grid->addWidget(btn_up, 1, 2, 1, 1);
    grid->addWidget(btn_left, 2, 1, 1, 1);
    grid->addWidget(btn_down, 2, 2, 1, 1);
    grid->addWidget(btn_right, 2, 3, 1, 1);

    connect(move_up, &QAction::triggered, &kdTreeInspector, &KdTreeInspector::kd_tree_inspection_move_to_parent);
    connect(move_down, &QAction::triggered, &kdTreeInspector, &KdTreeInspector::kd_tree_inspection_move_to_subtree);
    connect(move_left, &QAction::triggered, &kdTreeInspector, &KdTreeInspector::kd_tree_inspection_select_left);
    connect(move_right, &QAction::triggered, &kdTreeInspector, &KdTreeInspector::kd_tree_inspection_select_right);
  }

  connect(&kdTreeInspector,
          &KdTreeInspector::kd_tree_inspection_changed,
          [this](aabb_t active_aabb, glm::vec3 separating_point, aabb_t other_aabb) {
    viewport.visualization().set_kdtree_as_aabb(active_aabb, separating_point, other_aabb);
    viewport.update();
  });
#endif

  vbox->addStretch(1);

  return dock;
}

QDockWidget* MainWindow::initRenderDock()
{
  QDockWidget* dock = new QDockWidget("Render", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  QFormLayout* form;
  QHBoxLayout* hbox;

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

  // ---- remove shader ----
  QPushButton* removeShaderButton = new QPushButton("&Remove");

  // ---- clone shader ----
  QPushButton* cloneShaderButton = new QPushButton("&Clone");

  // ---- edit shader ----
  QPushButton* editShaderButton = new QPushButton("&Edit");

  // ---- shader choice ----
  auto is_builtin_visualization=[](int index){return index<=1;};

  QComboBox* shaderComboBox = new QComboBox;

  auto current_selected_point_shader = [shaderComboBox, this]() -> PointCloud::Shader {
    switch(shaderComboBox->currentIndex())
    {
    case 0:
      if(loadedShader.coordinate_expression.isEmpty())
        return pointShaderEditor.autogenerate(pointcloud.data());
      else
        return loadedShader;
    case 1:
      return pointShaderEditor.autogenerate(pointcloud.data());
    default:
      return shaderComboBox->currentData().value<PointCloud::Shader>();
    }
  };

  auto apply_current_shader = [this, current_selected_point_shader](){
    const PointCloud::Shader current_shader = current_selected_point_shader();

    pointShaderEditor.load_shader(current_shader);
    pointShaderEditor.applyShader();
  };

  auto switch_to_loaded_shader = [shaderComboBox](){
    shaderComboBox->setCurrentIndex(0);
  };

  connect(shaderComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this, shaderComboBox, is_builtin_visualization,removeShaderButton,editShaderButton,apply_current_shader](int index){
    const bool enable_modifying_buttons = !is_builtin_visualization(index);

    removeShaderButton->setEnabled(enable_modifying_buttons);
    editShaderButton->setEnabled(enable_modifying_buttons);
    pointShaderEditor.setIsReadOnly(!enable_modifying_buttons);
    pointShaderEditor.setShaderName(shaderComboBox->currentText());

    apply_current_shader();
  });

  connect(&pointShaderEditor, &PointShaderEditor::shaderNameChanged, [shaderComboBox, is_builtin_visualization](QString text){
    const int index = shaderComboBox->currentIndex();
    if(!is_builtin_visualization(index))
      shaderComboBox->setItemText(index, text);
  });

  connect(this, &MainWindow::pointcloud_imported, [switch_to_loaded_shader,apply_current_shader](){
    switch_to_loaded_shader();
    apply_current_shader();
  });
  connect(this, &MainWindow::pointcloud_unloaded, switch_to_loaded_shader);

  connect(&pointShaderEditor, &PointShaderEditor::shader_applied, [this, is_builtin_visualization, shaderComboBox](bool coordinates_changed, bool colors_changed){
    apply_point_shader(pointcloud->shader, coordinates_changed, colors_changed);
    if(!is_builtin_visualization(shaderComboBox->currentIndex()))
      shaderComboBox->setItemData(shaderComboBox->currentIndex(), QVariant::fromValue<PointCloud::Shader>(pointcloud->shader));
  });

  shaderComboBox->addItem("<loaded>");
  shaderComboBox->addItem("<autogenerated>");

  connect(cloneShaderButton, &QPushButton::clicked, [shaderComboBox, current_selected_point_shader](){
    PointCloud::Shader pointShader = current_selected_point_shader();
    QString name = shaderComboBox->currentText();
    name = name.trimmed();
    if(name.startsWith("<"))
      name = name.mid(1);
    if(name.endsWith(">"))
      name.chop(1);
    name += "_cloned";

    shaderComboBox->addItem(name, QVariant::fromValue(pointShader));
    shaderComboBox->setCurrentIndex(shaderComboBox->count()-1);
  });

  connect(removeShaderButton, &QPushButton::clicked, [shaderComboBox, is_builtin_visualization](){
    if(!is_builtin_visualization(shaderComboBox->currentIndex()))
      shaderComboBox->removeItem(shaderComboBox->currentIndex());
  });

  connect(editShaderButton, &QPushButton::clicked, [shaderComboBox, is_builtin_visualization, this](){
    if(is_builtin_visualization(shaderComboBox->currentIndex()))
      return;

    if(!pointShaderEditor.isVisible())
      pointShaderEditor.show();
  });

  // -- render style --
  QGroupBox* styleGroup = new QGroupBox("Style");
  form = new QFormLayout;
  styleGroup->setLayout((form));

  form->addRow("Background:", backgroundBrightness);
  form->addRow("Point Size:", pointSize);

  // -- property visualization --
  QGroupBox* propertyVisualizationGroup = new QGroupBox("Property Visualization");
  form = new QFormLayout;
  propertyVisualizationGroup->setLayout((form));

  form->addRow("Visualization:", shaderComboBox);

  hbox = new QHBoxLayout;
  form->addRow(hbox);
  hbox->addWidget(editShaderButton);

  hbox = new QHBoxLayout;
  form->addRow(hbox);
  hbox->addWidget(cloneShaderButton);
  hbox->addWidget(removeShaderButton);

  // -- vbox --
  QVBoxLayout* vbox = new QVBoxLayout(root);

  vbox->addWidget(renderButton);
  vbox->addWidget(styleGroup);
  vbox->addWidget(propertyVisualizationGroup);
  vbox->addStretch(1);

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
