#include <core_library/print.hpp>
#include <pointcloud_viewer/keypoint_list.hpp>

#include <QAction>
#include <QMenu>

KeypointList::KeypointList()
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &KeypointList::customContextMenuRequested, this, &KeypointList::customContextMenu);

  action_delete_keypoint = new QAction("&Delete Keypoint", this);
  action_delete_keypoint->setShortcut(Qt::Key_Delete);
  action_delete_keypoint->setEnabled(false);
  connect(action_delete_keypoint, &QAction::triggered, this, &KeypointList::delete_keypoint);

  context_menu = new QMenu(this);
  context_menu->addAction(action_delete_keypoint);

  // This allows executing the action by shortcut
  this->addAction(action_delete_keypoint);
}

KeypointList::~KeypointList()
{
}

void KeypointList::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  QListView::currentChanged(current, previous);

  bool has_selected_keypoint = current.isValid();

  action_delete_keypoint->setEnabled(has_selected_keypoint);

  currentKeypointChanged();
}

void KeypointList::customContextMenu(const QPoint& pos)
{
  context_menu->exec(this->mapToGlobal(pos));
}

void KeypointList::delete_keypoint()
{
  QModelIndex index = currentIndex();

  if(!index.isValid())
    return;

  on_delete_keypoint(index.row());
}
