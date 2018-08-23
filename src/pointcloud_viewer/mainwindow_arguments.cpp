#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/import_pointcloud.hpp>

#include <QApplication>
#include <QSharedPointer>
#include <QDebug>

void MainWindow::handleApplicationArguments()
{
  const QStringList arguments = qApp->arguments();

  auto abort = [this](){
    this->hide();
    this->noninteractive = true;
  };

  for(int argument_index=1; argument_index<arguments.length(); ++argument_index)
  {
    const QString argument = arguments[argument_index];

    if(argument == "--non-interactive")
    {
      noninteractive = true;
    }else if(argument == "--data")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--data\"";
        std::exit(-1);
      }
      argument_index++;

      const QString path = arguments[argument_index];

      QSharedPointer<PointCloud> point_cloud = import_point_cloud(this, path);

      if(Q_UNLIKELY(!point_cloud->is_valid))
      {
        abort();
        return;
      }

      pointcloud_imported(point_cloud);
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
      renderSettings.export_images = true;
    }else  if(argument == "--first_index")
    {
      if(argument_index+1 == arguments.length())
      {
        qDebug() << "Missing argument after \"--first_index\"";
        std::exit(-1);
      }
      argument_index++;

      const QString parameter = arguments[argument_index];

      bool ok;
      renderSettings.first_index = parameter.toInt(&ok);

      if(renderSettings.first_index < 0 || !ok)
      {
        qDebug() << "Invalid value" << parameter << "after \"--first_index\"";
        std::exit(-1);
      }
    }else if(argument == "--help")
    {
      qDebug() << "Usage: pointcloud_viewer [ARGUMENTS]\n"
                  "\n"
                  "--help               Show this helptext and exits immediately                   \n"
                  "--non-interactive    Hides the main window an immediately starts rendering. Used\n"
                  "                     for using this software as command-line tool.              \n"
                  "                     (Import and render dialog will appear nevertheless but     \n"
                  "                     don't require manual input)                                \n"
                  "\n"
                  "--data <FILE>        Pointcloud file to load                                    \n"
                  "--camera-path <FILE> The path of the camera                                     \n"
                  "\n"
                  "--output_dir <DIR>   Where to save the rendered image files                     \n"
                  "--first_index <INTEGER>  The first index used for the first rendered image      \n"
                  "                     filename\n"
                  ;
      std::exit(0);
    }else
    {
      qDebug() << "Unexpected argument " << argument;
      std::exit(-1);
    }
  }

  if(noninteractive == true)
  {
    this->hide();
    offline_render();
  }
}
