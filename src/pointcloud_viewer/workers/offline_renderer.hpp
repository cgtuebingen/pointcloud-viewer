#ifndef POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
#define POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_

#include <QVideoEncoderSettings>
#include <QPair>

class MainWindow;

struct RenderSettings
{
  QVideoEncoderSettings encoderSettings;
  QString target_video_file;

  QString target_images_directory;

  static RenderSettings defaultSettings();
};

QPair<RenderSettings, bool> ask_for_render_settings(RenderSettings prevSettings);

/**
The function responsible for rendering the animation to a video-file.
*/
void render(MainWindow* mainWindow, RenderSettings renderSettings);

#endif // POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
