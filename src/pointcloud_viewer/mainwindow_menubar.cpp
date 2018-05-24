#include <pointcloud_viewer/mainwindow.hpp>

#include <QMenuBar>

void MainWindow::initMenuBar()
{
  QMenuBar* menuBar = new QMenuBar;
  menuBar->setVisible(true);
  setMenuBar(menuBar);

  QMenu* menu_view = menuBar->addMenu("&View");
  QMenu* menu_view_navigation = menu_view->addMenu("&Navigation");
  QAction* action_view_navigation_fps = menu_view_navigation->addAction("&First Person Navigation");

  action_view_navigation_fps->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F));
  connect(action_view_navigation_fps, &QAction::triggered, &viewport.navigation, &Navigation::startFpsNavigation);
}
