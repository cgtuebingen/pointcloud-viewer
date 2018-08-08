#ifndef POINTCLOUDVIEWER_FLYTHROUGH_PLAYBACK_HPP_
#define POINTCLOUDVIEWER_FLYTHROUGH_PLAYBACK_HPP_

#include <QObject>

/** Class sending signals to playback an animation either in realtime or with a fixed frequency
*/
class Playback : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int fixed_framerate READ fixed_framerate WRITE setFixed_framerate NOTIFY fixed_framerateChanged)
public:
  bool only_one_frame = false;

  double current_time() const;
  int fixed_framerate() const;

  int totalNumberFramesForFixedFramerate() const;

public slots:
  void play_realtime();
  void play_with_fixed_framerate();

  void stop();

  void previous_frame_finished(double duration);

  void setFixed_framerate(int fixed_framerate);

signals:
  void request_next_frame(double time);
  void started();
  void end_reached();
  void aborted();

  void fixed_framerateChanged(int fixed_framerate);

private:
  friend class Flythrough;

  enum mode_t
  {
    IDLE,
    REALTIME,
    FIXED_FRAMERATE,
  };

  mode_t _mode = IDLE;

  double _animationDuration = 0.;
  double _current_time = 0;
  double _fixed_frametime = 1./25.;
  int _fixed_framerate = 25;

  Playback();

  void _reqest_next_frame(double time);
};

#endif // POINTCLOUDVIEWER_FLYTHROUGH_PLAYBACK_HPP_
