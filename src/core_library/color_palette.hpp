#ifndef CORELIBRARY_COLOR_PALETTE_HPP_
#define CORELIBRARY_COLOR_PALETTE_HPP_

#include <glm/glm.hpp>
#include <QColor>

class Color
{
public:
  Color(QRgb rgb);
  Color(QColor color);
  Color(glm::vec3 rgb);
  Color(glm::dvec3 rgb);
  Color(glm::u8vec3 rgb);

  Color with_hue(Color other) const;
  Color with_saturation(Color other) const;
  Color with_saturation(double saturation) const;

  QString hexcode() const;

  operator QColor() const;
  operator glm::vec3() const;
  operator glm::vec4() const;

private:
  QColor color;
};

// see https://en.wikipedia.org/wiki/Tango_Desktop_Project#Palette
namespace color_palette
{

const Color red[3] = {Color(0xa40000), Color(0xcc0000), Color(0xef2929)};
const Color green[3] = {Color(0x4e9a06), Color(0x73d216), Color(0x8ae234)};
const Color blue[3] = {Color(0x204a87), Color(0x3465a4), Color(0x729fcf)};
const Color yellow[3] = {Color(0xc4a000), Color(0xedd400), Color(0xce94f)};
const Color orange[3] = {Color(0xce5c00), Color(0xf57900), Color(0xfcaf3e)};
const Color brown[3] = {Color(0x8f5902), Color(0xc17d11), Color(0xe9b96e)};
const Color violet[3] = {Color(0x5c3566), Color(0x75507b), Color(0xad7fa8)};
const Color aluminium[6] = {Color(0x2e3436), Color(0x555753), Color(0x888a85), Color(0xbabdb6), Color(0xd3d7cf), Color(0xeeeeec)};

const Color grey[6] = {aluminium[0].with_saturation(0.), aluminium[1].with_saturation(0.), aluminium[2].with_saturation(0.), aluminium[3].with_saturation(0.), aluminium[4].with_saturation(0.), aluminium[5].with_saturation(0.)};
const Color cyan[3] = {blue[0].with_hue(0x00ffff), blue[1].with_hue(0x00ffff), blue[2].with_hue(0x00ffff)};
const Color magenta[3] = {violet[0].with_hue(0xff00ff), violet[1].with_hue(0xff00ff), violet[2].with_hue(0xff00ff)};

} // namespace color_palette

#endif // CORELIBRARY_COLOR_PALETTE_HPP_
