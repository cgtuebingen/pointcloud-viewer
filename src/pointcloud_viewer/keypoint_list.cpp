#include <pointcloud_viewer/keypoint_list.hpp>

KeypointList::KeypointList()
{

}

KeypointList::~KeypointList()
{

}

void KeypointList::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  QListView::currentChanged(current, previous);

  currentKeypointChanged();
}
