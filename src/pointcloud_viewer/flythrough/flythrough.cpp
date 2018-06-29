#include <pointcloud_viewer/flythrough/flythrough.hpp>

#include <QModelIndex>

enum SECTION
{
  INDEX,
};

void Flythrough::insert_keypoint(frame_t frame, int index)
{
  index = glm::clamp(index, 0, _keypoints.length());

  keypoint_t keypoint;

  keypoint.frame = frame;

  beginInsertRows(QModelIndex(), index, index+1);
  _keypoints.insert(index, keypoint);
  endInsertRows();
}

keypoint_t Flythrough::keypoint_at(int index) const
{
  return _keypoints[index];
}

int Flythrough::rowCount(const QModelIndex& parent) const
{
  if(parent.isValid())
    return 0;

  return _keypoints.length();
}

QVariant Flythrough::data(const QModelIndex& index, int role) const
{
  switch(role)
  {
  case Qt::DisplayRole:
    switch(SECTION(index.column()))
    {
    case SECTION::INDEX:
      return index.row();
    }
    return QVariant();
  default:
    return QVariant();
  }
}

QVariant Flythrough::headerData(int section, Qt::Orientation orientation, int role) const
{
  switch(role)
  {
  case Qt::DisplayRole:
    switch(SECTION(section))
    {
    case SECTION::INDEX:
      return "#";
    }
    return QVariant();
  default:
    return QVariant();
  }
}
