#ifndef CORELIBRARY_IMAGE_HPP_
#define CORELIBRARY_IMAGE_HPP_

#include <QImage>

// Flip the image vertically.
//
// Used for converting between the opengl texture coordinate system and the QImage coordinate system
void flip_image(QImage& image);

#endif // CORELIBRARY_IMAGE_HPP_
