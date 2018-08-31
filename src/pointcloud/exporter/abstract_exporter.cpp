#include <pointcloud/exporter/abstract_exporter.hpp>
#include <pointcloud/exporter/ply_exporter.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <QThread>
#include <QSharedPointer>
#include <QAbstractEventDispatcher>
#include <QSettings>

#include <iostream>

AbstractPointCloudExporter::~AbstractPointCloudExporter()
{
}

QSharedPointer<AbstractPointCloudExporter> AbstractPointCloudExporter::exporterForSuffix(QString suffix, std::string filepath, const PointCloud& pointcloud)
{
  if(suffix == "ply")
  {
    return QSharedPointer<AbstractPointCloudExporter>(new PlyExporter(filepath, pointcloud));
  }else
    return QSharedPointer<AbstractPointCloudExporter>();
}

QString AbstractPointCloudExporter::allSupportedFiletypes()
{
  return "PLY (*.ply)";
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
