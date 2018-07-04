#include <pointcloud_viewer/workers/offline_renderer.hpp>
#include <pointcloud_viewer/flythrough/flythrough.hpp>
#include <pointcloud_viewer/viewport.hpp>

#include <QTimer>


OfflineRenderer::OfflineRenderer(Viewport* viewport, const Flythrough& flythrough, const RenderSettings& renderSettings)
  : viewport(*viewport),
    flythrough(flythrough.copy()),
    renderSettings(renderSettings),
    total_number_frames(int(glm::round(flythrough.animationDuration() * renderSettings.framerate))),
    result_rgba(renderSettings.resolution.width(),
                renderSettings.resolution.height(),
                gl::TextureFormat::RGB8UI),
    result_depth(renderSettings.resolution.width(),
                 renderSettings.resolution.height(),
                 gl::TextureFormat::DEPTH32F_STENCIL8),
    framebuffer(gl::FramebufferObject::Attachment(&result_rgba),
                gl::FramebufferObject::Attachment(&result_depth))
{
  this->flythrough->playback.setFixed_framerate(renderSettings.framerate);

  connect(this->flythrough.data(), &Flythrough::set_new_camera_frame, this, &OfflineRenderer::render_next_frame, Qt::DirectConnection);
  connect(this, &OfflineRenderer::rendered_frame, &this->flythrough->playback, &Playback::previous_frame_finished, Qt::QueuedConnection);
}

void OfflineRenderer::start()
{
  frame_index = 0;
  flythrough->playback.play_with_fixed_framerate();
}

void OfflineRenderer::render_next_frame(frame_t camera_frame)
{
  if(frame_index >= total_number_frames)
    return;

  QImage frame_content(renderSettings.resolution.width(),
                       renderSettings.resolution.height(),
                       QImage::Format_RGB888);

  framebuffer.Bind(true);

  viewport.render_points(camera_frame, [](){});

  GL_CALL(glReadPixels, 0, 0, renderSettings.resolution.width(), renderSettings.resolution.height(), GL_RGB, GL_UNSIGNED_BYTE, frame_content.bits());

  gl::FramebufferObject::BindBackBuffer();

  ++frame_index;
  if(frame_index < total_number_frames)
    rendered_frame(frame_index-1, frame_content);
  else
    finished();
}
