#include <pointcloud_viewer/keypoint_list.hpp>

KeypointList::KeypointList()
{
  connect(this, &KeypointList::customContextMenuRequested, this, &KeypointList::customContextMenu);
}

KeypointList::~KeypointList()
{

}

void KeypointList::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  QListView::currentChanged(current, previous);

  currentKeypointChanged();
}

void KeypointList::customContextMenu(QPoint& pos)
{
  TODO
}
