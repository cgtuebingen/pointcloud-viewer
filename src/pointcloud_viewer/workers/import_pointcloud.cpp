#include <pointcloud_viewer/workers/import_pointcloud.hpp>
#include <pointcloud_viewer/mainwindow.hpp>
#include <core_library/print.hpp>

#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QMessageBox>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>

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

  int64_t total_bytes = file.size();

  QSharedPointer<AbstractPointCloudImporter> importer;
  if(suffix == "ply")
  {
    importer = QSharedPointer<AbstractPointCloudImporter>(new PlyImporter(input_stream, total_bytes));
  }else
  {
    QMessageBox::warning(parent, "Unexpected file format", QString("Unexpected file format '%0'.").arg(suffix));
    return failed;
  }

  QProgressDialog progressDialog(QString("Importing Point Cloud Layer\n<%1>").arg(file.fileName()), "&Abort", 0, AbstractPointCloudImporter::progress_max(), parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);

  progressDialog.show();

  QThread thread;
  importer->moveToThread(&thread);

  QObject::connect(importer.data(), &AbstractPointCloudImporter::update_progress, &progressDialog, &QProgressDialog::setValue);
  QObject::connect(importer.data(), &AbstractPointCloudImporter::finished, &thread, &QThread::quit);
  QObject::connect(&progressDialog, &QProgressDialog::canceled, importer.data(), &AbstractPointCloudImporter::cancel);
  QObject::connect(&thread, &QThread::started, importer.data(), &AbstractPointCloudImporter::import);

  thread.start();

  while(importer->state==AbstractPointCloudImporter::IDLE || importer->state==AbstractPointCloudImporter::RUNNING || thread.isRunning())
    QCoreApplication::processEvents(QEventLoop::EventLoopExec | QEventLoop::DialogExec | QEventLoop::WaitForMoreEvents);

  switch(importer->state)
  {
  case AbstractPointCloudImporter::CANCELED:
    QMessageBox::warning(parent, "Importing Cancelled", QString("Importing the pointcloud file was canceled by the user."));
    return failed;
  case AbstractPointCloudImporter::RUNNING:
  case AbstractPointCloudImporter::IDLE:
    QMessageBox::warning(parent, "Unknown Error", QString("Internal error"));
    return failed;
  case AbstractPointCloudImporter::RUNTIME_ERROR:
    QMessageBox::warning(parent, "Import Error", QString("Couldn't import the file <%0. Probably an io error or invalid file.").arg(file.fileName()));
    return failed;
  case AbstractPointCloudImporter::SUCCEEDED:
    return succeeded;
  }

  return failed;
}

void AbstractPointCloudImporter::import()
{
  this->state = RUNNING;
  this->num_bytes_processed = 0;

  try
  {
    if(import_implementation())
      this->state = SUCCEEDED;
    else if(this->state == RUNNING)
      this->state = RUNTIME_ERROR;
  }catch(...)
  {
    this->state = RUNTIME_ERROR;
  }

  finished();
}

AbstractPointCloudImporter::AbstractPointCloudImporter(std::istream& input_stream, int64_t total_num_bytes)
  : input_stream(input_stream),
    total_num_bytes(total_num_bytes)
{
}

void AbstractPointCloudImporter::cancel()
{
  this->state = CANCELED;
}

bool AbstractPointCloudImporter::handle_loaded_chunk(int64_t num_bytes_processed)
{
  Q_ASSERT(num_bytes_processed <= total_num_bytes);

  this->num_bytes_processed = num_bytes_processed;

  float86_t progress = float86_t(num_bytes_processed) / float86_t(total_num_bytes);
  int discrete_progress_value = int(progress * 65536 + float86_t(0.5));
  discrete_progress_value = glm::clamp(0, 65536, discrete_progress_value);

  update_progress(discrete_progress_value);

  QThread* thread = QThread::currentThread();

  if(thread != nullptr && thread->eventDispatcher() != nullptr)
    thread->eventDispatcher()->processEvents(QEventLoop::EventLoopExec | QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

  return this->state == RUNNING;
}

PlyImporter::PlyImporter(std::istream& input_stream, int64_t total_num_bytes)
  : AbstractPointCloudImporter(input_stream, total_num_bytes)
{
}

bool PlyImporter::import_implementation()
{
  tinyply::PlyFile file;

  // parse the header
  file.parse_header(input_stream);

  glm::bvec3 has_xyz(false);
  glm::bvec3 has_red_green_blue(false);

  std::vector<std::string> data_components;

  for(auto element : file.get_elements())
  {
    if(element.name != "vertex")
      continue;

    for(auto property : element.properties)
    {
      const std::string& name = property.name;

      glm::bvec3 is_xyz(name == "x", name == "y", name == "z");
      glm::bvec3 is_red_green_blue(name == "red", name == "green", name == "blue");

      has_xyz = has_xyz || is_xyz;
      has_red_green_blue = has_red_green_blue || is_red_green_blue;

      const bool is_data = !glm::any(is_xyz || is_red_green_blue);

      if(is_data)
        data_components.push_back(property.name);
    }
  }

  if(glm::all(has_xyz) == false)
    throw QString("Pointcloud contains no coordinates!!");

  std::shared_ptr<tinyply::PlyData> vertices, colors, data;

  vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});

  if(glm::all(has_red_green_blue))
    colors = file.request_properties_from_element("vertex", {"red", "green", "blue"});

  if(data_components.size() == 1)
    data = file.request_properties_from_element("vertex", {data_components[0]});
  else if(data_components.size() == 2)
    data = file.request_properties_from_element("vertex", {data_components[0], data_components[1]});
  else if(data_components.size() == 3)
    data = file.request_properties_from_element("vertex", {data_components[0], data_components[1], data_components[3]});
  else if(data_components.size() == 4)
    data = file.request_properties_from_element("vertex", {data_components[0], data_components[1], data_components[3], data_components[4]});
  else if(data_components.size() != 0)
    throw QString("File contains too many data components. A maximum of 4 data components is supported.");

  if(!handle_loaded_chunk(input_stream.tellg()))
    return failed;

  file.read(input_stream);

  return succeeded;
}
