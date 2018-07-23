#ifndef POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
#define POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_

#include <QSize>
#include <QString>
#include <QObject>
#include <QSharedPointer>

#include <geometry/frame.hpp>

#include <glhelper/framebufferobject.hpp>
#include <glhelper/texture2d.hpp>


#define VIDEO_OUTPUT 0


class Viewport;
class Flythrough;

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
  int first_index;


  static RenderSettings defaultSettings();
};

class OfflineRenderer : public QObject
{
Q_OBJECT
public:
  Viewport& viewport;
  const QSharedPointer<Flythrough> flythrough;
  const RenderSettings renderSettings;
  const int total_number_frames;

  OfflineRenderer(Viewport* viewport, const Flythrough& flythrough, const RenderSettings& renderSettings);
  ~OfflineRenderer();

  bool was_aborted() const;

public slots:
  void start();
  void abort();

signals:
  void rendered_frame(int frame_index, const QImage& image);
  void finished();

  void on_aborted();

private:
  bool _aborted = false;
  int frame_index = 0;

  gl::Texture2D result_rgba, result_depth;
  gl::FramebufferObject framebuffer;

private slots:
  void render_next_frame(frame_t camera_frame);

  void save_image(int frame_index, const QImage& image);
};

#endif // POINTCLOUDVIEWER_WORKERS_OFFLINERENDERER_HPP_
