#include <pointcloud_viewer/flythrough/interpolation.hpp>

Interpolation::Interpolation(const QVector<keypoint_t>* keypoints)
  : keypoints(*keypoints)
{
}

double LinearInterpolation::path_length() const
{
  double pathLength = 0;
  for(int i=1; i<keypoints.length(); ++i)
    pathLength += double(glm::distance<float>(keypoints[i].frame.position, keypoints[i-1].frame.position));

  return pathLength;
}

frame_t LinearInterpolation::frame_for_time(double time, double cameraVelocity) const
{
  double prevTime = 0;
  for(int i=1; i<keypoints.length(); ++i)
  {
    double segmentLength = double(glm::distance<float>(keypoints[i].frame.position, keypoints[i-1].frame.position));

    double timeInTheNextSegment = segmentLength / cameraVelocity;

    double nextTime = prevTime + timeInTheNextSegment;


    if(prevTime<=time && nextTime>=time)
    {
      double alpha = glm::clamp((time - prevTime) / timeInTheNextSegment, 0., 1.);

      frame_t a = keypoints[i-1].frame;
      frame_t b = keypoints[i].frame;

      TODO interpolate
    }

    prevTime = nextTime;
  }

  return keypoints.last().frame;
}
