#include <pointcloud/importer/abstract_importer.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <QThread>
#include <QAbstractEventDispatcher>

#include <iostream>

AbstractPointCloudImporter::~AbstractPointCloudImporter()
{
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
