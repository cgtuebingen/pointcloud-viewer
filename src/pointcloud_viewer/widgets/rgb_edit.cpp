#include <pointcloud_viewer/widgets/rgb_edit.hpp>

RgbEdit::RgbEdit()
{
  setInputMask("\\#HHHHHH");
  setText("#000000");
  setMaxLength(7);

  connect(this, &RgbEdit::textEdited, this, &RgbEdit::handle_changed);
}

int RgbEdit::red() const
{
  return rgb().r;
}

int RgbEdit::green() const
{
  return rgb().g;
}

int RgbEdit::blue() const
{
  return rgb().b;
}

glm::u8vec3 RgbEdit::rgb() const
{
  QString text = this->text();

  if(text.length() != 7)
    text += "0000000";

  glm::bvec3 ok;
  glm::u8vec3 rgb;

  rgb.r = uint8_t(text.mid(1, 2).toInt(&ok.r, 16));
  rgb.g = uint8_t(text.mid(3, 2).toInt(&ok.g, 16));
  rgb.b = uint8_t(text.mid(5, 2).toInt(&ok.b, 16));

  return rgb;
}

void RgbEdit::handle_changed()
{
  colorChanged(rgb());
}
