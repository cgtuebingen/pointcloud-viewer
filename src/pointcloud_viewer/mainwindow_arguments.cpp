#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/import_pointcloud.hpp>

#include <QApplication>
#include <QDebug>

void MainWindow::handleApplicationArguments()
{
  const QStringList arguments = qApp->arguments();

  bool background = false;

  for(int argument_index=1; argument_index<arguments.length(); ++argument_index)
  {
    const QString argument = arguments[argument_index];

    if(argument == "--background")
    {
      background = true;
    }else if(argument == "--data")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--data\"";
        std::exit(-1);
      }
      argument_index++;

      const QString path = arguments[argument_index];

      viewport.load_point_cloud(import_point_cloud(this, path));
    }else if(argument == "--camera-path")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--path\"";
        std::exit(-1);
      }
      argument_index++;

      const QString path = arguments[argument_index];

      flythrough.import_path(path);
    }else if(argument == "--output_dir")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--output_dir\"";
        std::exit(-1);
      }
      argument_index++;

      const QString path = arguments[argument_index];

      renderSettings.target_images_directory = QDir(path).absolutePath();
    }else if(argument == "--help")
    {
      qDebug() << "Usage: pointcloud_viewer [ARGUMENTS]\n"
                  "\n"
                  "--help               Show this helptext and exits immediately                   \n"
                  "--background         Hides the gui. Used for using this software as command-line\n"
                  "                     tool.                                                      \n"
                  "--data <FILE>        Pointcloud file to load                                    \n"
                  "--camera-path <FILE> The path of the camera                                     \n"
                  "--output_dir <DIR>   Where to save the rendered files                           \n"
                  ;
      std::exit(0);
    }else
    {
      qDebug() << "Unexpected argument " << argument;
      std::exit(-1);
    }
  }

  if(background == true)
  {
    offline_render();
    std::exit(0);
  }
}
