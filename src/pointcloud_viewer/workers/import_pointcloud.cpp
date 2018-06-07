#include <pointcloud_viewer/workers/import_pointcloud.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>

#include <fstream>

#include <tinyply.h>

typedef long double float86_t;

typedef bool result_t;
const result_t failed = false;
const result_t succeeded = true;


result_t import_point_cloud(QWidget* parent, QString filepath)
{
  QFileInfo file(filepath);

  if(!file.exists())
  {
    QMessageBox::warning(parent, "Not existing file", QString("The given file <%0> does not exist!").arg(filepath));
    return failed;
  }

  std::ifstream input_stream;
  try
  {
    input_stream.open(file.absoluteFilePath().toStdString());
  }catch(...)
  {
    QMessageBox::warning(parent, "Can't existing file", QString("Could not open the file <%0> for reading.").arg(filepath));
    return failed;
  }

  const QString suffix = file.completeSuffix();

  std::function<result_t(std::istream& input_stream, std::function<bool (int64_t)> callback)> loader;
  if(suffix == "ply")
  {
    loader = import_ply_file;
  }else
  {
    QMessageBox::warning(parent, "Unexpected file format", QString("Unexpected file format '%0'.").arg(suffix));
    return failed;
  }

  QProgressDialog progressDialog(QString("Importing Point Cloud Layer\n<%1>").arg(file.fileName()), "&Abort", 0, 65536, parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);

  int64_t total_bytes = file.size();

  auto update_progress_bar = [&progressDialog, total_bytes](int64_t bytes_processed) {
    int value = int((float86_t(bytes_processed) / float86_t(total_bytes)) * 65536 + float86_t(0.5));
    value = glm::clamp(0, 65536, value);
    progressDialog.setValue(value);

    return !progressDialog.wasCanceled();
  };

  try
  {
    return loader(input_stream, update_progress_bar);
  }catch(...)
  {
    QMessageBox::warning(parent, "Rrror while loading point cloud file", QString("Error occured while loading the point cloud file '%0'.").arg(suffix));
    return failed;
  }
}

result_t import_ply_file(std::istream& input_stream, std::function<bool(int64_t)> callback)
{
  tinyply::PlyFile file;
  file.parse_header(input_stream);


  /*
  file.request_properties_from_element("vertex", {"x", "y", "z"});
  // TODO I have the full contoll over the stream! I can copy blocks of the ply file to the memory and read them with file.read? Can I? THis way, I can have a progress bar!! :D
  // file.read(filestream);
  */

  return succeeded;
}
