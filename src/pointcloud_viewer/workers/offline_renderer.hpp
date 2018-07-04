#ifndef POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
#define POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_

#include <QPair>
#include <QSize>
#include <QString>

#define VIDEO_OUTPUT 0

class MainWindow;

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

QPair<RenderSettings, bool> ask_for_render_settings(RenderSettings prevSettings);

/**
The function responsible for rendering the animation to a video-file.
*/
void render(MainWindow* mainWindow, RenderSettings renderSettings);

#endif // POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
