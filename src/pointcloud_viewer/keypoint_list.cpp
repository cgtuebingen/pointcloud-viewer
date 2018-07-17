#include <core_library/print.hpp>
#include <pointcloud_viewer/keypoint_list.hpp>

#include <QAction>
#include <QMenu>

KeypointList::KeypointList()
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &KeypointList::customContextMenuRequested, this, &KeypointList::customContextMenu);

  action_flythrough_insert_keypoint = new QAction("&Insert Keypoint", this);
  action_flythrough_insert_keypoint->setShortcut(QKeySequence(Qt::Key_I));
  connect(action_flythrough_insert_keypoint, &QAction::triggered, this, &KeypointList::insert_keypoint);

  action_delete_keypoint = new QAction("&Delete Keypoint", this);
  action_delete_keypoint->setShortcut(Qt::Key_Delete);
  action_delete_keypoint->setEnabled(false);
  connect(action_delete_keypoint, &QAction::triggered, this, &KeypointList::delete_keypoint);

  action_move_keypoint_up = new QAction("&Delete Keypoint", this);
  action_move_keypoint_up->setShortcut(Qt::SHIFT + Qt::CTRL + Qt::Key_Up);
  action_move_keypoint_up->setEnabled(false);
  connect(action_move_keypoint_up, &QAction::triggered, this, &KeypointList::move_keypoint_up);

  action_move_keypoint_down = new QAction("&Delete Keypoint", this);
  action_move_keypoint_down->setShortcut(Qt::SHIFT + Qt::CTRL + Qt::Key_Down);
  action_move_keypoint_down->setEnabled(false);
  connect(action_move_keypoint_down, &QAction::triggered, this, &KeypointList::move_keypoint_down);

  context_menu = new QMenu(this);
  context_menu->addAction(action_flythrough_insert_keypoint);
  context_menu->addSeparator();
  context_menu->addAction(action_delete_keypoint);
  context_menu->addSeparator();
  context_menu->addAction(action_move_keypoint_up);
  context_menu->addAction(action_move_keypoint_down);

  // This allows executing the action by shortcut
  this->addAction(action_flythrough_insert_keypoint);
  this->addAction(action_delete_keypoint);
  this->addAction(action_move_keypoint_up);
  this->addAction(action_move_keypoint_down);
}

KeypointList::~KeypointList()
{
}

void KeypointList::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  QListView::currentChanged(current, previous);

  item_digest_t digest = this->digest(current);

  action_delete_keypoint->setEnabled(digest.has_selected_keypoint);
  action_move_keypoint_up->setEnabled(digest.has_selected_keypoint && !digest.is_first);
  action_move_keypoint_down->setEnabled(digest.has_selected_keypoint && !digest.is_last);

  currentKeypointChanged();
}

void KeypointList::customContextMenu(const QPoint& pos)
{
  context_menu->exec(this->mapToGlobal(pos));
}

KeypointList::item_digest_t KeypointList::digest(QModelIndex index) const
{
  item_digest_t digest;

  digest.has_selected_keypoint = index.isValid();
  digest.is_first = index.row()==0;
  digest.is_last = index.sibling(index.row()+1, index.column()).isValid() == false;

  return digest;
}

void KeypointList::insert_keypoint()
{
  QModelIndex index = currentIndex();
  KeypointList::item_digest_t digst = this->digest(index);

  on_insert_keypoint(index.row() + 1);
}

void KeypointList::delete_keypoint()
{
  QModelIndex index = currentIndex();
  KeypointList::item_digest_t digest = this->digest(index);

  if(!digest.has_selected_keypoint)
    return;

  on_delete_keypoint(index.row());
}

void KeypointList::move_keypoint_up()
{
  QModelIndex index = currentIndex();
  KeypointList::item_digest_t digest = this->digest(index);

  if(!digest.has_selected_keypoint || digest.is_first)
    return;

  on_move_keypoint_up(index.row());
}

void KeypointList::move_keypoint_down()
{
  QModelIndex index = currentIndex();
  KeypointList::item_digest_t digest = this->digest(index);

  if(!digest.has_selected_keypoint || digest.is_last)
    return;

  on_move_keypoint_down(index.row());
}
