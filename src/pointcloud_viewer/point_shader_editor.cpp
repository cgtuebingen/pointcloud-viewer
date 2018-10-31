#include <core_library/print.hpp>
#include <core_library/color_palette.hpp>

#include <pointcloud_viewer/point_shader_editor.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/NodeDataModel>
#include <nodes/Node>

#include <pointcloud_viewer/shader_nodes/make_vector_node.hpp>
#include <pointcloud_viewer/shader_nodes/math_operator_node.hpp>
#include <pointcloud_viewer/shader_nodes/property_node.hpp>
#include <pointcloud_viewer/shader_nodes/rotate_quickly_node.hpp>
#include <pointcloud_viewer/shader_nodes/split_vector_node.hpp>
#include <pointcloud_viewer/shader_nodes/spy_node.hpp>
#include <pointcloud_viewer/shader_nodes/output_node.hpp>
#include <pointcloud_viewer/shader_nodes/value_node.hpp>
#include <pointcloud_viewer/shader_nodes/vector_property_node.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QQueue>
#include <QDialogButtonBox>
#include <QSettings>
#include <QFileDialog>
#include <QPushButton>

PointShaderEditor::PointShaderEditor(MainWindow* mainWindow)
  : mainWindow(*mainWindow)
{
  setWindowTitle("Point Shader");

  QVBoxLayout* rootLayout = new QVBoxLayout;

  this->setLayout(rootLayout);
  rootLayout->setMargin(0);
  rootLayout->setSpacing(0);

  QMenuBar* menuBar = new QMenuBar;
  rootLayout->addWidget(menuBar);
  QMenu* shader_menu = menuBar->addMenu("&Shader");
  importShader_action = shader_menu->addAction("&Import Shader");
  exportShader_action = shader_menu->addAction("&Export Shader");
  shader_menu->addSeparator();
  QAction* applyShaderEditor_action = shader_menu->addAction("Apply Shader");
  shader_menu->addSeparator();
  QAction* closeShaderEditor_action = shader_menu->addAction("Close Shader Editor");

  QObject::connect(exportShader_action, &QAction::triggered, this, &PointShaderEditor::exportShader);
  QObject::connect(importShader_action, &QAction::triggered, this, &PointShaderEditor::importShader);

  QObject::connect(applyShaderEditor_action, &QAction::triggered, this, &PointShaderEditor::applyShader);
  QObject::connect(closeShaderEditor_action, &QAction::triggered, this, &PointShaderEditor::closeEditor);

  fallbackFlowScene = new QtNodes::FlowScene(qt_nodes_model_registry(nullptr));

  {
    readonlyNotificationBar = new QLabel;
    readonlyNotificationBar->setVisible(false);
    readonlyNotificationBar->setStyleSheet(QString("QLabel{background: " + color_palette::orange[0].hexcode() + "; color: " + color_palette::aluminium[5].hexcode() + "}"));
    readonlyNotificationBar->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    QFont font = readonlyNotificationBar->font();
    font.setPixelSize(32);
    font.setBold(true);
    readonlyNotificationBar->setFont(font);
    rootLayout->addWidget(readonlyNotificationBar);
  }

  QVBoxLayout* vbox = new QVBoxLayout;
  rootLayout->addLayout(vbox);

  flowView = new QtNodes::FlowView(fallbackFlowScene);
  flowView->setMinimumSize(1024, 768);
  vbox->addWidget(flowView);

  QToolButton* apply_button = new QToolButton();
  QToolButton* close_button = new QToolButton();

  apply_button->setDefaultAction(applyShaderEditor_action);
  close_button->setDefaultAction(closeShaderEditor_action);

  shaderName_Editor = new QLineEdit;
  connect(shaderName_Editor, &QLineEdit::textChanged, this, &PointShaderEditor::setShaderName);
  connect(this, &PointShaderEditor::shaderNameChanged, shaderName_Editor, &QLineEdit::setText);

  QHBoxLayout* hbox = new QHBoxLayout;
  hbox->setMargin(8);
  hbox->addWidget(shaderName_Editor);
  hbox->addStretch(1);
  hbox->addWidget(apply_button);
  hbox->addWidget(close_button);
  vbox->addLayout(hbox);

  auto update_widget_sensitivity = [this, applyShaderEditor_action]() {
    const bool is_readonly = this->isReadOnly();
    const bool is_pointcloud_loaded = this->isPointCloudLoaded();

    const bool something_to_edit = !is_readonly && is_pointcloud_loaded;

    flowView->setEnabled(something_to_edit);
    importShader_action->setEnabled(something_to_edit);
    applyShaderEditor_action->setEnabled(something_to_edit);
    shaderName_Editor->setEnabled(something_to_edit);

    exportShader_action->setEnabled(is_pointcloud_loaded);

    if(is_readonly)
      readonlyNotificationBar->setText("Can't edit the current point-shader");
    else if(!is_pointcloud_loaded)
      readonlyNotificationBar->setText("No point cloud loaded");
    else
      readonlyNotificationBar->setText("Can't edit the shader");
    readonlyNotificationBar->setVisible(!something_to_edit);
  };

  connect(this, &PointShaderEditor::isPointCloudLoadedChanged, update_widget_sensitivity);
  connect(this, &PointShaderEditor::isReadOnlyChanged, update_widget_sensitivity);

  update_widget_sensitivity();
}

PointShaderEditor::~PointShaderEditor()
{
  delete fallbackFlowScene;
  delete flowScene;
}

void PointShaderEditor::unload_all_point_clouds()
{
  if(!isPointCloudLoaded())
    return;

  unload_shader();

  isPointCloudLoadedChanged(isPointCloudLoaded());
}

void PointShaderEditor::load_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  if(point_cloud == nullptr)
  {
    unload_all_point_clouds();
    return;
  }

  _pointCloud = point_cloud;

  load_shader(point_cloud->shader);

  isPointCloudLoadedChanged(isPointCloudLoaded());
}

void PointShaderEditor::unload_shader()
{
  if(flowScene == nullptr)
    return;

  delete flowScene;
  flowScene = nullptr;
  flowView->setScene(fallbackFlowScene);

  _pointCloud.clear();
}

void PointShaderEditor::load_shader(PointCloud::Shader shader)
{
  flowView->setScene(fallbackFlowScene);
  delete flowScene;
  flowScene = nullptr;

  std::shared_ptr<QtNodes::DataModelRegistry> registry = qt_nodes_model_registry(_pointCloud.data());

  flowScene = new QtNodes::FlowScene(registry);
  flowScene->loadFromMemory(shader.node_data.toUtf8());
  flowView->setScene(flowScene);
}

PointCloud::Shader PointShaderEditor::autogenerate(const PointCloud* pointcloud)
{
  static const QSet<QString> xyz_names = {"x", "y", "z"};
  static const QSet<QString> red_green_blue_names = {"red", "green", "blue"};

  PointCloud::Shader shader;

  if(pointcloud != nullptr)
  {
    const QSet<QString> properties_contained_within_the_point_cloud = pointcloud->user_data_names.toList().toSet();
    if(properties_contained_within_the_point_cloud.contains(xyz_names))
      shader.used_properties.unite(xyz_names);
    if(properties_contained_within_the_point_cloud.contains(red_green_blue_names))
      shader.used_properties.unite(red_green_blue_names);
  }

  std::shared_ptr<QtNodes::DataModelRegistry> registry = qt_nodes_model_registry(pointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);

  QSizeF areaSize(1024, 768);
  qreal margin = 16;

  auto set_node_position = [flowScene, areaSize, margin](QtNodes::Node& node, QPointF relative) {
    QSizeF node_size = flowScene->getNodeSize(node);
    QPointF pos(relative.x() * (areaSize.width() - margin * 2. - node_size.width()) + margin,
                relative.y() * (areaSize.height() - margin * 2. - node_size.height()) + margin);
    flowScene->setNodePosition(node, pos);
  };

  QtNodes::Node& outputNode = flowScene->createNode(std::make_unique<OutputNode>());
  set_node_position(outputNode, QPointF(1., 0.5));

  if(shader.used_properties.contains(xyz_names))
  {
    std::unique_ptr<QtNodes::NodeDataModel> model = registry->create("VectorProperty");

    VectorPropertyNode* node = dynamic_cast<VectorPropertyNode*>(model.get());
    Q_ASSERT(node != nullptr);

    if(node != nullptr)
    {
      node->set_properties("x", "y", "z");

      QtNodes::Node& coordinates_node = flowScene->createNode(std::move(model));
      set_node_position(coordinates_node, QPointF(0., 0.25));
      flowScene->createConnection(outputNode, 0, coordinates_node, 0);
    }
  }

  if(shader.used_properties.contains(red_green_blue_names))
  {
    std::unique_ptr<QtNodes::NodeDataModel> model = registry->create("VectorProperty");

    VectorPropertyNode* node = dynamic_cast<VectorPropertyNode*>(model.get());
    Q_ASSERT(node != nullptr);

    if(node != nullptr)
    {
      node->set_properties("red", "green", "blue");

      QtNodes::Node& coordinates_node = flowScene->createNode(std::move(model));
      set_node_position(coordinates_node, QPointF(0., 0.75));
      flowScene->createConnection(outputNode, 1, coordinates_node, 0);
    }
  }

  shader.node_data = flowScene->saveToMemory();

  shader = ::generate_code_from_shader(flowScene, shader);

  return shader;
}

bool PointShaderEditor::isPointCloudLoaded() const
{
  return _pointCloud!=nullptr;
}

bool PointShaderEditor::isReadOnly() const
{
  return m_isReadOnly;
}

QString PointShaderEditor::shaderName() const
{
  return m_shaderName;
}

void PointShaderEditor::setIsReadOnly(bool isReadOnly)
{
  if (m_isReadOnly == isReadOnly)
    return;

  m_isReadOnly = isReadOnly;
  emit isReadOnlyChanged(m_isReadOnly);
}

void PointShaderEditor::setShaderName(QString shaderName)
{
  if (m_shaderName == shaderName)
    return;

  m_shaderName = shaderName;
  emit shaderNameChanged(m_shaderName);
}

std::shared_ptr<QtNodes::DataModelRegistry> PointShaderEditor::qt_nodes_model_registry(const PointCloud* currentPointcloud)
{
  QStyle* style = QApplication::style();

  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  QPixmap warning_icon = style->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(QSize(22,22));
  QMap<QString, property_type_t> base_type_for_name;
  if(currentPointcloud == nullptr)
  {
    missingPropertyNames << "x" << "y" << "z" << "red" << "green" << "blue";
    base_type_for_name["x"]   = base_type_for_name["y"]    = base_type_for_name["z"]     = PROPERTY_TYPE::FLOAT32;
    base_type_for_name["red"] = base_type_for_name["green"] = base_type_for_name["blue"] = PROPERTY_TYPE::UINT8;
  }else
  {
    supportedPropertyNames << currentPointcloud->user_data_names.toList();

    for(QString expected_property : currentPointcloud->shader.used_properties)
      if(!supportedPropertyNames.contains(expected_property))
      {
        missingPropertyNames << expected_property;
        base_type_for_name[expected_property] = PROPERTY_TYPE::FLOAT64;
      }

    for(int i=0; i<currentPointcloud->user_data_names.length(); ++i)
      base_type_for_name[currentPointcloud->user_data_names[i]] = currentPointcloud->user_data_types[i];
  }

  if(supportedPropertyNames.isEmpty() && missingPropertyNames.isEmpty())
  {
    missingPropertyNames << "x" << "y" << "z";
    base_type_for_name["x"]   = base_type_for_name["y"]    = base_type_for_name["z"]     = PROPERTY_TYPE::FLOAT32;
  }

  supportedPropertyNames.sort();
  missingPropertyNames.sort();

  std::shared_ptr<QtNodes::DataModelRegistry> registry(new QtNodes::DataModelRegistry);

  registry->registerModel<MathOperatorNode>("Math");
  registry->registerModel<ValueNode>("Math");
  registry->registerModel<MakeVectorNode>("Vector");
  registry->registerModel<SplitVectorNode>("Vector");
  registry->registerModel<RotateQuicklyNode>("Transform");
  registry->registerModel<OutputNode>("Output");
  registry->registerModel<SpyNode>("Output");
  registry->registerModel<PropertyNode>("Input",
                                        [supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon]() {
    return std::make_unique<PropertyNode>(supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon);
  });
  registry->registerModel<VectorPropertyNode>("Input",
                                        [supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon]() {
    return std::make_unique<VectorPropertyNode>(supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon);
  });

  return registry;
}

void PointShaderEditor::applyShader()
{
  if(!isPointCloudLoaded())
    return;

  _pointCloud->shader.node_data = flowScene->saveToMemory();

  PointCloud::Shader old_shader = _pointCloud->shader;
  PointCloud::Shader new_shader = generate_code_from_shader(flowScene, _pointCloud->shader);

  _pointCloud->shader = new_shader;

  const bool coordinates_changed = old_shader.coordinate_expression != new_shader.coordinate_expression;
  const bool colors_changed = old_shader.color_expression != new_shader.color_expression;

  shader_applied(coordinates_changed, colors_changed);
}

void PointShaderEditor::closeEditor()
{
  hide();
}

void PointShaderEditor::importShader()
{
  if(!isPointCloudLoaded() || isReadOnly())
  {
    Q_UNREACHABLE();
    return;
  }


  QString dir;

  {
    QSettings settings;
    dir = settings.value("VizualizationShaders/exportDir").toString();
  }

  QString filename = QFileDialog::getOpenFileName(this, "Import Visualization", dir, "Point Visualization (*.point-visualization)");
  if(!filename.isEmpty())
  {
    {
      QSettings settings;
      settings.setValue("VizualizationShaders/exportDir", QFileInfo(filename).dir().absolutePath());
    }

    try
    {
      load_shader(PointCloud::Shader::import_from_file(filename));
    }catch(...)
    {
      QMessageBox::warning(this, "Import Error", "Couldn't import the Visualization");
    }
  }
}

void PointShaderEditor::exportShader()
{
  if(!isPointCloudLoaded())
  {
    Q_UNREACHABLE();
    return;
  }

  QString dir;

  {
    QSettings settings;
    dir = settings.value("VizualizationShaders/exportDir").toString();
  }

  QString filename = QFileDialog::getSaveFileName(this, "Export Visualization", dir, "Point Visualization (*.point-visualization)");
  if(!filename.isEmpty())
  {
    {
      QSettings settings;
      settings.setValue("VizualizationShaders/exportDir", QFileInfo(filename).dir().absolutePath());
    }

    try
    {
      if(!filename.toLower().endsWith(".point-visualization"))
        filename += ".point-visualization";
      _pointCloud->shader.export_to_file(filename);
    }catch(...)
    {
      QMessageBox::warning(this, "Export Error", "Couldn't export the Visualization");
    }
  }
}

QSet<QString> find_used_properties(const PointCloud* pointcloud)
{
  if(pointcloud == nullptr)
    return QSet<QString>();

  QSet<QString> used_properties;

  std::shared_ptr<QtNodes::DataModelRegistry> registry = PointShaderEditor::qt_nodes_model_registry(pointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);
  flowScene->loadFromMemory(pointcloud->shader.node_data.toUtf8());

  // Lambda filling used_properties with all property names, which are actually used
  auto collectProperties = [&used_properties, flowScene](QtNodes::Node* node){
    QHash<QtNodes::Node*, QSet<QtNodes::Node*>> incoming_nodes;
    for(auto _connection : flowScene->connections())
    {
      std::shared_ptr<QtNodes::Connection> connection = _connection.second;
      incoming_nodes[connection->getNode(QtNodes::PortType::In)].insert(connection->getNode(QtNodes::PortType::Out));
    }

    QSet<QtNodes::Node*> done_nodes;
    QQueue<QtNodes::Node*> queued_nodes;

    queued_nodes.enqueue(node);

    while(queued_nodes.isEmpty() == false)
    {
      node = queued_nodes.dequeue();

      if(done_nodes.contains(node))
        continue;

      PropertyNode* property = dynamic_cast<PropertyNode*>(node->nodeDataModel());
      VectorPropertyNode* vectorProperty = dynamic_cast<VectorPropertyNode*>(node->nodeDataModel());

      if(property != nullptr)
        used_properties << property->property_name();
      if(vectorProperty != nullptr)
        for(QString n : vectorProperty->vector_properties_names())
          used_properties << n;

      for(QtNodes::Node* n : incoming_nodes[node])
        if(!done_nodes.contains(n) && !queued_nodes.contains(n))
          queued_nodes.enqueue(n);
    }
  };

  // Find the output nodes to
  // a) collect all actually used attributes with collectProperties
  // b) generate the actual expression
  flowScene->iterateOverNodes([collectProperties](QtNodes::Node* node){
    OutputNode* outputNode = dynamic_cast<OutputNode*>(node->nodeDataModel());

    if(outputNode!=nullptr)
      collectProperties(node);
  });

  return used_properties;
}

PointCloud::Shader generate_code_from_shader(const PointCloud* pointcloud)
{
  if(pointcloud == nullptr)
    return PointCloud::Shader();

  if(pointcloud->shader.node_data.isEmpty())
    return pointcloud->shader;

  std::shared_ptr<QtNodes::DataModelRegistry> registry = PointShaderEditor::qt_nodes_model_registry(pointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);
  flowScene->loadFromMemory(pointcloud->shader.node_data.toUtf8());

  return generate_code_from_shader(flowScene, pointcloud->shader);
}

PointCloud::Shader generate_code_from_shader(QtNodes::FlowScene* flowScene, PointCloud::Shader shader)
{
  QString fallback_coordinate_expression = shader.coordinate_expression;
  QString fallback_color_expression = shader.color_expression;

  shader.coordinate_expression.clear();
  shader.color_expression.clear();

  flowScene->iterateOverNodes([&shader](QtNodes::Node* node){
    OutputNode* outputNode = dynamic_cast<OutputNode*>(node->nodeDataModel());

    if(outputNode!=nullptr)
    {
      if(outputNode->coordinate != nullptr && outputNode->coordinate->expression.length() != 0)
      {
        if(!shader.coordinate_expression.isEmpty())
          println_error("#error Multiple Output Nodes");
        shader.coordinate_expression = outputNode->coordinate->expression;
      }
      if(outputNode->color != nullptr && outputNode->color->expression.length() != 0)
      {
        if(!shader.color_expression.isEmpty())
          println_error("#error Multiple Output Nodes");
        shader.color_expression = outputNode->color->expression;
      }
    }

  });

  return shader;
}
