#include <core_library/image.hpp>

void flip_image(QImage& image)
{
  const int height = image.height();
  const int bytesPerLine = image.bytesPerLine();

  for(int y=0; y<image.height()/2; ++y)
  {
    uchar* a = image.scanLine(y);
    uchar* b = image.scanLine(height-1-y);
    for(int x=0; x<bytesPerLine; ++x)
      std::swap(a[x], b[x]);
  }
}
