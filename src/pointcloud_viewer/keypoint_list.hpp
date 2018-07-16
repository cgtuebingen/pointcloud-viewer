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
  void customContextMenu(QPoint& pos);
};

#endif // POINTCLOUDVIEWER_KEYPOINTLIST_HPP_
