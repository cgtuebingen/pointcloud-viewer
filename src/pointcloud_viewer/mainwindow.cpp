#include <pointcloud_viewer/mainwindow.hpp>

#include <QApplication>
#include <QDebug>

MainWindow::MainWindow()
  : pointCloudLayer(this)
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  handleApplicationArguments();
  initMenuBar();
  initDocks();

  connect(&flythrough, &Flythrough::set_new_camera_frame, &viewport, &Viewport::set_camera_frame);
  connect(&viewport, &Viewport::frame_rendered, &flythrough.playback, &Playback::previous_frame_finished);
}

MainWindow::~MainWindow()
{
}

void MainWindow::handleApplicationArguments()
{
  const QStringList arguments = qApp->arguments();

  for(int argument_index=1; argument_index<arguments.length(); ++argument_index)
  {
    const QString argument = arguments[argument_index];

    if(argument == "--background")
    {
      // TODO
    }else if(argument == "--data")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--data\"";
        std::exit(-1);
      }
      // TODO
    }else if(argument == "--path")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--path\"";
        std::exit(-1);
      }

      // TODO
    }else if(argument == "--output_dir")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--output_dir\"";
        std::exit(-1);
      }

      // TODO
    }else if(argument == "--help")
    {
      qDebug() << "Usage: pointcloud_viewer [ARGUMENTS]\n"
                  "                                                                                \n"
                  "--help             Show this helptext and exits immediately                     \n"
                  "--background       Hides the gui. Used for using this software as command-line  \n"
                  "                   tool.                                                        \n"
                  "--data <FILE>      Pointcloud file to load                                      \n"
                  "--path <FILE>      The path of the camera                                       \n"
                  "--output_dir <DIR> Where to save the rendered files                             \n"
                  ;
      std::exit(0);
    }else
    {
      qDebug() << "Unexpected argument " << argument;
      std::exit(-1);
    }
  }
}
