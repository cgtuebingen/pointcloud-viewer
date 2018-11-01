#ifndef POINTCLOUDVIEWER_SHADER_NODES_VECTOR_PROPERTY_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_VECTOR_PROPERTY_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QLabel>
#include <QMap>
#include <QComboBox>

class VectorPropertyNode final : public QtNodes::NodeDataModel
{
public:
  VectorPropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_types, QPixmap warning_icon);

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  QString caption() const override;
  QString name() const override;
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  QVector<QString> vector_properties_names() const {return QVector<QString>({x_combobox->currentText(), y_combobox->currentText(), z_combobox->currentText()});}

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  void set_properties(const QString& nameX, const QString& nameY, const QString& nameZ);

  QWidget* embeddedWidget() override;

private slots:
  void changedXProperty(QString name);
  void changedYProperty(QString name);
  void changedZProperty(QString name);
  void changedProperty();

private:
  QMap<QString, property_type_t> property_base_types;
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<Value> _x_property = std::make_shared<Value>("0", VALUE_TYPE::INT);
  std::shared_ptr<Value> _y_property = std::make_shared<Value>("0", VALUE_TYPE::INT);
  std::shared_ptr<Value> _z_property = std::make_shared<Value>("0", VALUE_TYPE::INT);
  std::shared_ptr<Value> _vector_property = std::make_shared<Value>("ivec3(0)", VALUE_TYPE::IVEC3);

  QWidget* _root_widget;
  QLabel* x_warning_widget;
  QLabel* y_warning_widget;
  QLabel* z_warning_widget;

  QComboBox* x_combobox;
  QComboBox* y_combobox;
  QComboBox* z_combobox;
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_VECTOR_PROPERTY_NODE_HPP_
