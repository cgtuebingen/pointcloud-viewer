#ifndef POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_DIALOG_HPP_
#define POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_DIALOG_HPP_

#include <pointcloud_viewer/workers/offline_renderer.hpp>

#include <QPair>

class MainWindow;

QPair<RenderSettings, bool> ask_for_render_settings(RenderSettings prevSettings);

/**
The function responsible for rendering the animation to a video-file.
*/
void render(MainWindow* mainWindow, RenderSettings renderSettings);

#endif // POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_DIALOG_HPP_
