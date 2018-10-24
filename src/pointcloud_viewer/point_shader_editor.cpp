#include <pointcloud_viewer/point_shader_editor.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <renderer/gl450/point_remapper.hpp>
#include <core_library/print.hpp>

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

PointShaderEditor::PointShaderEditor()
{
}

PointShaderEditor::~PointShaderEditor()
{
}

void PointShaderEditor::unload_all_point_clouds()
{
  _pointCloud.clear();
}

void PointShaderEditor::load_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  _pointCloud = point_cloud;
}

PointCloud::Shader PointShaderEditor::autogenerate() const
{
  static const QSet<QString> xyz_names = {"x", "y", "z"};
  static const QSet<QString> red_green_blue_names = {"red", "green", "blue"};

  PointCloud::Shader shader;

  if(_pointCloud != nullptr)
  {
    const QSet<QString> properties_contained_within_the_point_cloud = _pointCloud->user_data_names.toList().toSet();
    if(properties_contained_within_the_point_cloud.contains(xyz_names))
      shader.used_properties.unite(xyz_names);
    if(properties_contained_within_the_point_cloud.contains(red_green_blue_names))
      shader.used_properties.unite(red_green_blue_names);
  }

  std::shared_ptr<QtNodes::DataModelRegistry> registry = qt_nodes_model_registry(_pointCloud);

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

  return shader;
}

// TODO
#if 0
void PointShader::apply_shader(Viewport& viewport, const QSharedPointer<PointCloud>& currentPointcloud) const
{
  if(currentPointcloud == nullptr)
    return;

  viewport.makeCurrent();

  QString shader_code_glsl450;
  QVector<uint> bindings;
  std::tie(shader_code_glsl450, bindings) = this->shader_code_glsl450(currentPointcloud);

  if(!renderer::gl450::remap_points(shader_code_glsl450.toStdString(), bindings, currentPointcloud.data()))
  {
    QMessageBox::warning(&viewport, "Shader error", "Could not apply the point shader.\nPlease take a look at the Standard Output");
    return;
  }


  viewport.load_point_cloud(currentPointcloud);
  // TODO: rebuild the KD tree?
  //TODO:  update selected point?

  viewport.doneCurrent();
}

PointShader::PointShader(const QSharedPointer<PointShader::Implementation>& implementation)
  : _implementation(implementation)
{
}

QSharedPointer<PointShader::Implementation> PointShader::Implementation::clone() const
{
  QSharedPointer<Implementation> implementation(new Implementation);

  implementation->name = this->name + " (Copy)";
  implementation->properties = this->properties;
  implementation->nodes = this->nodes;

  return implementation;
}
#endif

// TODO
#if 0
bool PointShader::edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud)
{
  QDialog dialog(parent);
  dialog.setModal(true);

  QVBoxLayout* vbox = new QVBoxLayout;

  std::shared_ptr<QtNodes::DataModelRegistry> registry = qt_nodes_model_registry(currentPointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);
  flowScene->loadFromMemory(_implementation->nodes);

  QtNodes::FlowView* flowView = new QtNodes::FlowView(flowScene);
  flowView->setMinimumSize(1024, 768);
  vbox->addWidget(flowView);

  QDialogButtonBox* buttonGroup = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  vbox->addWidget(buttonGroup);
  QObject::connect(buttonGroup, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(buttonGroup, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  dialog.setLayout(vbox);

  bool accepted = false;

  QObject::connect(&dialog, &QDialog::accepted, [flowScene, this, &accepted](){
    _implementation->nodes = flowScene->saveToMemory();
    accepted = true;
  });

  dialog.exec();

  return accepted;
}
#endif

// TODO
#if 0
std::tuple<QString, QVector<uint>> PointShader::shader_code_glsl450(const QSharedPointer<PointCloud>& currentPointcloud) const
{
  QString code;
  code += "#version 450 core\n";
  code += "\n";

  // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
  // https://www.khronos.org/files/opengl45-quick-reference-card.pdf

  std::shared_ptr<QtNodes::DataModelRegistry> registry = qt_nodes_model_registry(currentPointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);
  flowScene->loadFromMemory(_implementation->nodes);

  QSet<QString> used_properties;

  QString coordinate_code;
  QString color_code;

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
  flowScene->iterateOverNodes([&coordinate_code, &color_code, &code, collectProperties](QtNodes::Node* node){
    OutputNode* outputNode = dynamic_cast<OutputNode*>(node->nodeDataModel());

    if(outputNode!=nullptr)
    {
      collectProperties(node);

      if(outputNode->coordinate != nullptr && outputNode->coordinate->expression.length() != 0)
      {
        if(!coordinate_code.isEmpty())
          code += "#error Multiple Output Nodes\n";
        coordinate_code = outputNode->coordinate->expression;
      }
      if(outputNode->color != nullptr && outputNode->color->expression.length() != 0)
      {
        if(!color_code.isEmpty())
          code += "#error Multiple Output Nodes\n";
        color_code = outputNode->color->expression;
      }
    }

  });

  if(coordinate_code.isEmpty())
    coordinate_code = "vec3(0) /* not set */";
  if(color_code.isEmpty())
    color_code = "uvec3(255) /* not set */";

  code += "\n";
  code += "// ==== Input Buffer ====\n";
  uint binding_index = 0;
  QVector<decltype(binding_index)> property_bindings;
  for(int i=0; i<currentPointcloud->user_data_names.length(); ++i)
  {
    QString type = format(property_to_value_type(currentPointcloud->user_data_types[i]));
    QString name = currentPointcloud->user_data_names[i];

    if(used_properties.contains(name) == false)
    {
      property_bindings << std::numeric_limits<decltype(binding_index)>::max();
      continue;
    }

    property_bindings << binding_index;

    code += "layout(location = " + QString::number(binding_index)+ ")\n";
    code += "in " + type + " " + name + ";\n";

    binding_index++;
  }
  code += "\n";

  code += "// ==== Output Buffer ====\n";
  code += "struct vertex_t\n";
  code += "{\n";
  code += "  vec3 coordinate;\n";
  code += "  uint color;\n";
  code += "};\n";
  code += "\n";
  code += "layout(std430, binding=0)\n";
  code += "buffer impl_output_buffer\n";
  code += "{\n";
  code += "  vertex_t impl_output_vertex[];\n";
  code += "};\n";
  code += "\n";

  code += "// ==== Helper Functions ====\n";
  code += "int    to_scalar(in ivec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "uint   to_scalar(in uvec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "float  to_scalar(in  vec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "double to_scalar(in dvec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "\n";
  code += "// ==== Actual execution ====\n";
  code += "void main()\n";
  code += "{\n";
  code += "  impl_output_vertex[gl_VertexID].coordinate = \n\n"
          "      // ==== COORDINATE ====\n"
          "      " + coordinate_code + ";\n"
          "      // ====================\n\n";
  code += "  impl_output_vertex[gl_VertexID].color = packUnorm4x8(vec4(vec3(\n\n"
          "      // ==== COLOR =========\n"
          "      " + color_code + "\n      // ====================\n\n  ), 0) / 255.);\n";
  code += "}\n";

  return std::make_tuple(code, property_bindings);
}
#endif


std::shared_ptr<QtNodes::DataModelRegistry> PointShaderEditor::qt_nodes_model_registry(const QSharedPointer<PointCloud>& currentPointcloud) const
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
