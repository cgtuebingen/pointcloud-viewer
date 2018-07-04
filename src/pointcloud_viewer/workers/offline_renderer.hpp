#ifndef POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
#define POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_

#include <QSize>
#include <QString>

#define VIDEO_OUTPUT 0

struct RenderSettings
{
  QSize resolution;
  int framerate;

#if VIDEO_OUTPUT
  QString target_video_file;
#endif

  QString target_images_directory;
  QString image_format;
  bool export_images;


  static RenderSettings defaultSettings();
};

#endif // POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
