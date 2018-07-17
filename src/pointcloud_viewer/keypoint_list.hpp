#ifndef POINTCLOUDVIEWER_KEYPOINTLIST_HPP_
#define POINTCLOUDVIEWER_KEYPOINTLIST_HPP_

#include <QListView>

class KeypointList final : public QListView
{
  Q_OBJECT
public:
  KeypointList();
  ~KeypointList() override;

signals:
  void currentKeypointChanged();

  void on_delete_keypoint(int index);
  void on_move_keypoint_up(int index);
  void on_move_keypoint_down(int index);

protected:
  void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private:
  struct item_digest_t
  {
    bool has_selected_keypoint;
    bool is_first;
    bool is_last;
  };

  QMenu* context_menu;
  QAction* action_delete_keypoint;
  QAction* action_move_keypoint_up;
  QAction* action_move_keypoint_down;

  void customContextMenu(const QPoint& pos);

  item_digest_t digest(QModelIndex index) const;

private slots:
  void delete_keypoint();
  void move_keypoint_up();
  void move_keypoint_down();
};

#endif // POINTCLOUDVIEWER_KEYPOINTLIST_HPP_
