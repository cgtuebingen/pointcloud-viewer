#include <core_library/color_palette.hpp>


Color::Color(QRgb rgb)
  : color(QColor::fromRgb(rgb))
{
}

Color Color::with_hue(Color other) const
{
  Color c(0);
  c.color.setHsvF(other.color.hueF(), color.saturationF(), color.valueF());
  return c;
}

Color Color::with_saturation(Color other) const
{
  Color c(0);
  c.color.setHsvF(color.hueF(), other.color.saturationF(), color.valueF());
  return c;
}

Color Color::with_saturation(double saturation) const
{
  Color c(0);
  c.color.setHsvF(color.hueF(), saturation, color.valueF());
  return c;
}

Color::operator glm::vec3() const
{
  return glm::vec3(color.redF(), color.greenF(), color.blueF());
}

Color::operator glm::vec4() const
{
  return glm::vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

Color::operator QColor() const
{
  return color;
}
