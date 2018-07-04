#include <pointcloud_viewer/workers/offline_renderer.hpp>
#include <pointcloud_viewer/flythrough/flythrough.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <core_library/image.hpp>

#include <QTimer>


OfflineRenderer::OfflineRenderer(Viewport* viewport, const Flythrough& flythrough, const RenderSettings& renderSettings)
  : viewport(*viewport),
    flythrough(flythrough.copy()),
    renderSettings(renderSettings),
    total_number_frames(int(glm::round(flythrough.animationDuration() * renderSettings.framerate))),
    result_rgba(renderSettings.resolution.width(),
                renderSettings.resolution.height(),
                gl::TextureFormat::RGB8),
    result_depth(renderSettings.resolution.width(),
                 renderSettings.resolution.height(),
                 gl::TextureFormat::DEPTH_COMPONENT32F),
    framebuffer(gl::FramebufferObject::Attachment(&result_rgba),
                gl::FramebufferObject::Attachment(&result_depth))
{
  this->flythrough->playback.setFixed_framerate(renderSettings.framerate);

  connect(this->flythrough.data(), &Flythrough::set_new_camera_frame, this, &OfflineRenderer::render_next_frame, Qt::DirectConnection);
  connect(this, &OfflineRenderer::rendered_frame, &this->flythrough->playback, &Playback::previous_frame_finished, Qt::QueuedConnection);
}

void OfflineRenderer::start()
{
  _aborted = false;
  frame_index = 0;
  flythrough->playback.play_with_fixed_framerate();
}

void OfflineRenderer::abort()
{
  _aborted = true;
}

void OfflineRenderer::render_next_frame(frame_t camera_frame)
{
  if(frame_index >= total_number_frames || _aborted)
    return;

  const int width = renderSettings.resolution.width();
  const int height = renderSettings.resolution.height();

  QImage frame_content(renderSettings.resolution.width(),
                       renderSettings.resolution.height(),
                       QImage::Format_RGB888);

  viewport.makeCurrent();

  const GLuint fbo = framebuffer.GetInternHandle();
  GL_CALL(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);
  GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo);
  GL_CALL(glViewport, 0, 0, width, height);

  viewport.render_points(camera_frame, float(width)/float(height), [](){});

  GL_CALL(glNamedFramebufferReadBuffer, fbo, GL_COLOR_ATTACHMENT0);
  GL_CALL(glReadPixels, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, frame_content.bits());
  GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

  flip_image(frame_content);

  ++frame_index;
  if(frame_index < total_number_frames)
    rendered_frame(frame_index-1, frame_content);
  else
    finished();
}
