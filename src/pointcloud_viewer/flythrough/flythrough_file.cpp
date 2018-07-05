#include <QFile>

#include <pointcloud_viewer/flythrough/flythrough_file.hpp>

FlythroughFile FlythroughFile::import_from_file(QString filepath)
{
  FlythroughFile result;

  QFile file(filepath);

  if(file.open(QIODevice::ReadOnly) == false)
    throw QString("Couldn't open %0 for reading").arg(filepath);

  if(file.read(reinterpret_cast<char*>(&result.header), sizeof(header_t)) != sizeof(header_t))
    throw QString("Invalid file!\n(Couldn't read header)");

  if(result.header.magic_number != header_t::EXPECTED_MAGIC_NUMBER)
    throw QString("Invalid file!\n(wrong magic number)");

  if(result.header.header_length != sizeof(header_t))
    throw QString("Invalid file!\n(unexpected header length)");

  if(result.header.file_version != 0)
    throw QString("Invalid file!\n(unexpected file version)");

  if(result.header.num_keypoints < 0)
    throw QString("Invalid file!\n(negative number of keypoints)");

  if(result.header.interpolation < 0)
    throw QString("Invalid file!\n(negative interpolation id)");

  const int64_t keypoint_frames_size_bytes = sizeof(frame_t)*uint32_t(result.header.num_keypoints);

  if(file.size()-int64_t(sizeof(header_t)) != keypoint_frames_size_bytes)
    throw QString("Invalid file!\n(Number keypoints doesn't match filesize)");

  if(result.header.num_keypoints > 65535) // AN upper limit just to have an upper limit. Not tested. YOu can pic a different one if needed
    throw QString("Invalid file!\n(Too many keypoints)");

  result.keypoint_frames.resize(result.header.num_keypoints);

  if(keypoint_frames_size_bytes != file.read(reinterpret_cast<char*>(result.keypoint_frames.data()), keypoint_frames_size_bytes))
    throw QString("Invalid file!\n(failed reading keypoints)");

  return result;
}

void FlythroughFile::export_to_file(QString filepath)
{
  if(!filepath.endsWith(".camera_path"))
    filepath += ".camera_path";

  header.magic_number = header_t::EXPECTED_MAGIC_NUMBER;
  header.header_length = sizeof(header_t);
  header.file_version = 0;
  header.num_keypoints = keypoint_frames.length();

  QFile file(filepath);

  if(file.open(QIODevice::WriteOnly) == false)
    throw QString("Couldn't open %0 for writing").arg(filepath);

  file.write(reinterpret_cast<const char*>(&this->header), sizeof(header_t));
  file.write(reinterpret_cast<const char*>(this->keypoint_frames.data()), sizeof(frame_t)*uint32_t(keypoint_frames.length()));
}
