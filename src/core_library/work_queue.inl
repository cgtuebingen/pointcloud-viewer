#include <core_library/work_queue.hpp>

#include <glm/glm.hpp>

namespace internal {

/*
Type dependent implementation
*/
template<typename data_t>
class WorkQueueWorkerImplementation final : public WorkQueueWorker
{
public:
  WorkQueueWorkerImplementation(ThreadedWorkQueue<data_t>* queue);

protected:
  void _receivedWork() override;

private:
  ThreadedWorkQueue<data_t>& queue;
  uint64_t job_id = 0;
};


template<typename data_t>
WorkQueueWorkerImplementation<data_t>::WorkQueueWorkerImplementation(ThreadedWorkQueue<data_t>* queue)
  : WorkQueueWorker(queue),
    queue(*queue)
{
}

template<typename data_t>
void WorkQueueWorkerImplementation<data_t>::_receivedWork()
{
  typedef ThreadedWorkQueue<error_t>::error_t error_t;

  try
  {
    data_t data = queue.dequeue();

    job_id = ++queue.running_jobs;

    init_task(task_processor_t<data_t>::name_during_init(data));

    task_processor_t<data_t> processor(data);

    const int64_t work_amount = processor.work_amount;
    const int64_t block_size = processor.block_size;
    const QString name = processor.name;

    begin_task(name, work_amount);

    for(int64_t next_block=0; next_block<work_amount;)
    {
      const int64_t begin = next_block;
      const int64_t end = glm::min(processor.work_amount, begin + block_size);

      if(!processor.process(begin, end))
      {
        failed_task(!queue.is_empty());
        return;
      }

      progress(end, work_amount);

      if(queue.aborted_jobs >= job_id)
      {
        processor.aborted(0, end);
        aborted_tasks(!queue.is_empty());
        return;
      }

      next_block = end;
    }

    if(!processor.finished())
    {
      failed_task(!queue.is_empty());
      return;
    }

    succeeded_task(!queue.is_empty());

  }catch(error_t error)
  {
    switch(error)
    {
    case error_t::empty_stack:
      break;
    }
  }
}


} // namespace internal


template<typename data_t>
ThreadedWorkQueue<data_t>::ThreadedWorkQueue()
{
  internal::WorkQueueWorker* worker = new internal::WorkQueueWorkerImplementation<data_t>(this);
  worker->moveToThread(&thread);

  connect(this, &QObject::destroyed, worker, &QObject::deleteLater);

  connect(worker, &internal::WorkQueueWorker::init_task, this, &internal::WorkQueueInterface::init_task);
  connect(worker, &internal::WorkQueueWorker::begin_task, this, &internal::WorkQueueInterface::begin_task);
  connect(worker, &internal::WorkQueueWorker::progress, this, &internal::WorkQueueInterface::progress);
  connect(worker, &internal::WorkQueueWorker::aborted_tasks, this, &internal::WorkQueueInterface::aborted_tasks);
  connect(worker, &internal::WorkQueueWorker::failed_task, this, &internal::WorkQueueInterface::failed_task);
  connect(worker, &internal::WorkQueueWorker::succeeded_task, this, &internal::WorkQueueInterface::succeeded_task);

  running_jobs = 0;
  aborted_jobs = 0;

  thread.start();
}

template<typename data_t>
void ThreadedWorkQueue<data_t>::enqueue(data_t data)
{
  {
    QMutexLocker locker(&queue_mutex);

    queue.enqueue(data);
  }

  _receivedWork();
}

template<typename data_t>
bool ThreadedWorkQueue<data_t>::is_empty()
{
  QMutexLocker locker(&queue_mutex);
  return queue.isEmpty();
}

template<typename data_t>
data_t ThreadedWorkQueue<data_t>::dequeue()
{
  QMutexLocker locker(&queue_mutex);

  if(queue.isEmpty())
    throw error_t::empty_stack;

  return queue.dequeue();
}

template<typename data_t>
void ThreadedWorkQueue<data_t>::abort()
{
  QMutexLocker locker(&queue_mutex);

  aborted_jobs = running_jobs.load();
  queue.clear();
}
