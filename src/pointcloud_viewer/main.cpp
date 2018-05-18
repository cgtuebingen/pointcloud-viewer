#include <pointcloud_viewer/mainwindow.hpp>

#include <QApplication>

int main(int argc, char** argv)
{
  QApplication application(argc, argv);

  MainWindow mainwindow;

  mainwindow.show();

  application.exec();
}
