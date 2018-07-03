#ifndef POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
#define POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_

#include <QVideoEncoderSettings>

class MainWindow;

struct RenderSettings
{
  QVideoEncoderSettings encoderSettings;
  QString target_video_file;

  QString target_images_files;

  static RenderSettings defaultSettings();
};

RenderSettings ask_for_render_settings(RenderSettings prevSettings=RenderSettings::defaultSettings());

/**
The function responsible for rendering the animation to a video-file.
*/
void render(MainWindow* mainWindow, RenderSettings renderSettings);

#endif // POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
