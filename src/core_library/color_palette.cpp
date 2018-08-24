#include <core_library/color_palette.hpp>


Color::Color(QRgb rgb)
  : Color(QColor::fromRgb(rgb))
{
}

Color::Color(QColor color)
  : color(color)
{
}

Color::Color(glm::vec3 rgb)
  : Color(glm::dvec3(rgb))
{
}

Color::Color(glm::dvec3 rgb)
  : Color(QColor::fromRgbF(rgb.r, rgb.g, rgb.b))
{

}

Color::Color(glm::u8vec3 rgb)
  : Color(glm::dvec3(rgb) / 255.)
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

QString Color::hexcode() const
{
  return color.name();
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
