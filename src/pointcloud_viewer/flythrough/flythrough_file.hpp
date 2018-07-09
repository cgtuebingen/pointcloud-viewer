#ifndef POINTCLOUDVIEWER_FLYTHROUGH_FILE_HPP_
#define POINTCLOUDVIEWER_FLYTHROUGH_FILE_HPP_

#include <core_library/types.hpp>
#include <geometry/frame.hpp>
#include <QVector>

class FlythroughFile
{
public:
  struct header_t
  {
    constexpr static const uint64_t EXPECTED_MAGIC_NUMBER = 0x46506d6143796c46; // "FlyCamPF" FLYthrough CAMera Path File

    uint64_t magic_number = EXPECTED_MAGIC_NUMBER;
    uint32_t header_length = sizeof(header_t);
    uint32_t file_version = 0;
    double animation_duration = 0;
    double camera_velocity = 0;
    int32_t interpolation = 0;
    int32_t num_keypoints = 0;
  };

  header_t header;
  QVector<frame_t> keypoint_frames;

  static FlythroughFile import_from_file(QString filepath);
  void export_to_file(QString filepath);
};

#endif // POINTCLOUDVIEWER_FLYTHROUGH_FILE_HPP_
