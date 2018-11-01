#ifndef POINTCLOUDVIEWER_WIDGETS_RGBEDIT_HPP_
#define POINTCLOUDVIEWER_WIDGETS_RGBEDIT_HPP_

#include <QLineEdit>

#include <glm/glm.hpp>

class RgbEdit : public QLineEdit
{
  Q_OBJECT
public:
  RgbEdit();

  int red() const;
  int green() const;
  int blue() const;

  glm::u8vec3 rgb() const;

signals:
  void colorChanged(glm::u8vec3 rgb);

private slots:
  void handle_changed();
};

#endif // POINTCLOUDVIEWER_WIDGETS_RGBEDIT_HPP_
