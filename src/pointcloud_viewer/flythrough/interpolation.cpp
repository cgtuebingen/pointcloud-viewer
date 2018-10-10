#include <pointcloud_viewer/flythrough/interpolation.hpp>

Interpolation::Interpolation(const QVector<keypoint_t>* keypoints)
  : keypoints(*keypoints)
{
}

Interpolation::~Interpolation()
{
}

LinearInterpolation::LinearInterpolation(const QVector<keypoint_t>* keypoints, bool smoothstep)
  : Interpolation(keypoints),
    smoothstep(smoothstep)
{

}

double LinearInterpolation::path_length() const
{
  double pathLength = 0;
  for(int i=1; i<keypoints.length(); ++i)
    pathLength += double(glm::distance(keypoints[i].frame.position, keypoints[i-1].frame.position));

  return pathLength;
}

frame_t LinearInterpolation::frame_for_overcome_distance(double distance) const
{
  double prevDistance = 0;
  for(int i=1; i<keypoints.length(); ++i)
  {
    double segmentLength = double(glm::distance(keypoints[i].frame.position, keypoints[i-1].frame.position));

    double nextDistance = prevDistance + segmentLength;


    if(prevDistance<=distance && nextDistance>=distance)
    {
      double alpha = glm::clamp((distance - prevDistance) / segmentLength
                                , 0., 1.);

      if(smoothstep)
        alpha = glm::smoothstep(0., 1., alpha);

      frame_t a = keypoints[i-1].frame;
      frame_t b = keypoints[i].frame;

      return mix(a, b, float(alpha));
    }

    prevDistance = nextDistance;
  }

  return keypoints.last().frame;
}
