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

protected:
  void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private:
  void customContextMenu(const QPoint& pos);

  QMenu* context_menu;
  QAction* action_delete_keypoint;

private slots:
  void delete_keypoint();
};

#endif // POINTCLOUDVIEWER_KEYPOINTLIST_HPP_
