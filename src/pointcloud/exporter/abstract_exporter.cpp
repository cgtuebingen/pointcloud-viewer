#include <pointcloud/exporter/abstract_exporter.hpp>
#include <pointcloud/exporter/ply_exporter.hpp>
#include <pointcloud/exporter/pcvd_exporter.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <QThread>
#include <QSharedPointer>
#include <QAbstractEventDispatcher>
#include <QSettings>
#include <QFileInfo>

#include <iostream>

#define PLY_FILTER "PLY (*.ply)"
#define PCVD_FILTER "Pointcoud Viewer Dump (*.pcvd)"

AbstractPointCloudExporter::~AbstractPointCloudExporter()
{
}

QString AbstractPointCloudExporter::addMissingSuffix(QString filepath, QString selectedFilter)
{
  const QString suffix = QFileInfo(filepath).suffix().toLower();

  if(selectedFilter == PLY_FILTER)
  {
    if(suffix == "ply")
      return filepath;
    return filepath + ".ply";
  }else if(selectedFilter == PCVD_FILTER)
  {
    if(suffix == "pcvd")
      return filepath;
    return filepath + ".pcvd";
  }

  return addMissingSuffix(filepath, PCVD_FILTER);
}

QSharedPointer<AbstractPointCloudExporter> AbstractPointCloudExporter::exporterForSuffix(QString selectedFilter, std::string filepath, const PointCloud& pointcloud)
{
  if(selectedFilter == PLY_FILTER)
    return QSharedPointer<AbstractPointCloudExporter>(new PlyExporter(filepath, pointcloud));
  else if(selectedFilter == PCVD_FILTER)
    return QSharedPointer<AbstractPointCloudExporter>(new PcvdExporter(filepath, pointcloud));

  Q_UNREACHABLE();
  return exporterForSuffix(PCVD_FILTER, filepath, pointcloud);
}

QString AbstractPointCloudExporter::allSupportedFiletypes()
{
  return PCVD_FILTER ";;" PLY_FILTER;
}

void AbstractPointCloudExporter::export_now()
{
  this->state = RUNNING;

  try
  {
    if(export_implementation())
      this->state = SUCCEEDED;
    else if(this->state == RUNNING)
      this->state = RUNTIME_ERROR;
  }catch(QString message)
  {
    print_error(message.toStdString());
    this->state = RUNTIME_ERROR;
  }catch(canceled_t)
  {
    this->state = CANCELED;
  }catch(...)
  {
    this->state = RUNTIME_ERROR;
  }

  finished();
}

AbstractPointCloudExporter::AbstractPointCloudExporter(const std::string& output_file, const PointCloud& pointcloud)
  : output_file(output_file),
    pointcloud(pointcloud),
    total_progress(progress_max())
{
}

void AbstractPointCloudExporter::cancel()
{
  this->state = CANCELED;
}

bool AbstractPointCloudExporter::handle_written_chunk(int64_t current_progress)
{
  Q_ASSERT(current_progress <= total_progress);

  auto process_events = [](){
    QThread* thread = QThread::currentThread();
    if(thread != nullptr && thread->eventDispatcher() != nullptr)
      thread->eventDispatcher()->processEvents(QEventLoop::EventLoopExec | QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
  };

  process_events();

  if(Q_UNLIKELY(this->state == CANCELED))
    throw canceled_t();

  float86_t progress = float86_t(current_progress) / float86_t(total_progress);
  int discrete_progress_value = int(progress * 65536 + float86_t(0.5));
  discrete_progress_value = glm::clamp(0, 65536, discrete_progress_value);

  update_progress(discrete_progress_value);

  process_events();

  return this->state == RUNNING;
}
