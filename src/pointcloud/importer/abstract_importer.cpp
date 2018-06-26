#include <pointcloud/importer/abstract_importer.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>
#include <pointcloud/importer/assimp_importer.hpp>
#include <pointcloud/importer/ply_importer.hpp>

#include <QThread>
#include <QSharedPointer>
#include <QAbstractEventDispatcher>

#include <iostream>

AbstractPointCloudImporter::~AbstractPointCloudImporter()
{
}

QSharedPointer<AbstractPointCloudImporter> AbstractPointCloudImporter::importerForSuffix(QString suffix, std::string filepath)
{
  if(suffix == "ply")
  {
    return QSharedPointer<AbstractPointCloudImporter>(new PlyImporter(filepath));
#ifdef USE_ASSIMP
  }else if(suffix == "obj")
  {
    return QSharedPointer<AbstractPointCloudImporter>(new AssimpImporter(filepath));
#endif
  }else
    return QSharedPointer<AbstractPointCloudImporter>();
}

void AbstractPointCloudImporter::import()
{
  this->state = RUNNING;

  try
  {
    if(import_implementation())
      this->state = SUCCEEDED;
    else if(this->state == RUNNING)
      this->state = RUNTIME_ERROR;
  }catch(QString message)
  {
    print_error(message.toStdString());
    this->state = RUNTIME_ERROR;
  }catch(...)
  {
    this->state = RUNTIME_ERROR;
  }

  finished();
}

AbstractPointCloudImporter::AbstractPointCloudImporter(const std::string& input_file)
  : input_file(input_file),
    total_progress(progress_max())
{
}

void AbstractPointCloudImporter::cancel()
{
  this->state = CANCELED;
}

bool AbstractPointCloudImporter::handle_loaded_chunk(int64_t current_progress)
{
  Q_ASSERT(current_progress <= total_progress);

  float86_t progress = float86_t(current_progress) / float86_t(total_progress);
  int discrete_progress_value = int(progress * 65536 + float86_t(0.5));
  discrete_progress_value = glm::clamp(0, 65536, discrete_progress_value);

  update_progress(discrete_progress_value);

  QThread* thread = QThread::currentThread();

  if(thread != nullptr && thread->eventDispatcher() != nullptr)
    thread->eventDispatcher()->processEvents(QEventLoop::EventLoopExec | QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

  return this->state == RUNNING;
}
