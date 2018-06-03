#include <core_library/work_queue.hpp>

namespace internal {


WorkQueueWorker::WorkQueueWorker(WorkQueueInterface* queue)
{
  connect(queue, &WorkQueueInterface::_receivedWork, this, &WorkQueueWorker::_receivedWork);
}

} // namespace internal
