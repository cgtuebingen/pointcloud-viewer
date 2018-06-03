#include <core_library/work_queue.hpp>

#include <glm/glm.hpp>

namespace internal {

/*
The WorkQueueWorker is the object running in the seperate thread.

Again, the limitation of which with template classes (signals can't be created for template classes) required me to
split this class into two classes: WorkQueueWorker and WorkQueueWorkerImplementation.

The WorkQueueWorker class has slots thought to receive signals from the main thread:
- receivedWork: will be called whenever a new piece of work is available to be processed

There are also signals meant to be sent to the main thread (those will be resent from WorkQueueInterface)
- begin_task(QString name, int workload) whenever a task started
  void progress(int done, int workload);

  void aborted_tasks();
  int succeeded_task();
*/
class WorkQueueWorker : public QObject
{
Q_OBJECT

protected:
  WorkQueueWorker(WorkQueueInterface* queue);

signals:
  void begin_task(QString name, int workload);
  void progress(int done, int workload);

  void aborted_tasks(bool tasks_left);
  void failed_task(bool tasks_left);
  void succeeded_task(bool tasks_left);

protected slots:
  virtual void _receivedWork() = 0;
};

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
  : queue(*queue)
{
}

template<typename data_t>
void WorkQueueWorkerImplementation<data_t>::_receivedWork()
{
  typedef ThreadedWorkQueue<error_t>::error_t error_t;

  try
  {
    data_t data = queue.dequeue(&data);

    task_processor_t<data_t> processor(data);

    const int work_amount = processor.work_amount;
    const int block_size = processor.block_size;
    const QString name = processor.name;

    job_id = ++queue.running_jobs;

    begin_task(processor.name, work_amount);

    for(int next_block=0; next_block<work_amount;)
    {
      const int begin = next_block;
      const int end = glm::min(processor.work_amount, begin + block_size);

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

    failed_task(!queue.is_empty());

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

  connect(worker, &internal::WorkQueueWorker::begin_task, this, &internal::WorkQueueInterface::begin_task);
  connect(worker, &internal::WorkQueueWorker::progress, this, &internal::WorkQueueInterface::progress);
  connect(worker, &internal::WorkQueueWorker::aborted_tasks, this, &internal::WorkQueueInterface::aborted_tasks);
  connect(worker, &internal::WorkQueueWorker::failed_task, this, &internal::WorkQueueInterface::failed_task);
  connect(worker, &internal::WorkQueueWorker::succeeded_task, this, &internal::WorkQueueInterface::succeeded_task);

  running_jobs = 0;
  aborted_jobs = 0;
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

  aborted_jobs = running_jobs;
  queue.clear();
}
