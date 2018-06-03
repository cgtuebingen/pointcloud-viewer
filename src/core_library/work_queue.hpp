#ifndef CORELIBRARY_WORK_QUEUE_HPP_
#define CORELIBRARY_WORK_QUEUE_HPP_

#include <QQueue>
#include <QMutex>
#include <QThread>

#include <atomic>
#include <thread>


/*
The WorkQueue is a class for multithreaded master slave implementation. The number of threads is not limited, and every
thread can queue and enqueue as many tasks as wanted.

The idea is, that the gui can push tasks to the queue, which will be handeled in a seperate thread keeping the gui
responsible.

Long jobs can be aborted.

For updates (like progress bars and aborting work), signal slots are supported.
*/

template<typename data_t>
class ThreadedWorkQueue;


// ======== implementation ========

namespace internal {

/*
The WorkQueueInterface is used to support Qt signal/slot system, which has serious limitations with template classes (signals can't be creator for template classes).

So I had to split thew WorkQueue class into two classes.

It has one slot
- abort()
  Which will abort all tasks (the running one and all future ones)

And three signals:
- init_task whenever a task is being initialized
- begin_task whenever a task started
- progress is called after some chunks of work for informing the gui about the progress
- aborted_tasks is called, after all tasks were aborted
- succeeded_task
- failed_task

Never connect _receivedWork directly, this is only used for inter thread communication.
*/
class WorkQueueInterface : public QObject
{
  Q_OBJECT

protected:
  WorkQueueInterface(){}

public slots:
  virtual void abort() = 0;

signals:
  void _receivedWork();

  void init_task(QString name);
  void begin_task(QString name, int64_t workload);
  void progress(int64_t done, int64_t workload);

  void aborted_tasks(bool tasks_left);
  void succeeded_task(bool tasks_left);
  void failed_task(bool tasks_left);
};

template<typename data_t>
class WorkQueueWorkerImplementation;


/*
The WorkQueueWorker is the object running in the seperate thread.

Again, the limitation of which with template classes (signals can't be created for template classes) required me to
split this class into two classes: WorkQueueWorker and WorkQueueWorkerImplementation.

The WorkQueueWorker class has slots thought to receive signals from the main thread:
- receivedWork: will be called whenever a new piece of work is available to be processed

There are also signals meant to be sent to the main thread.
*/
class WorkQueueWorker : public QObject
{
Q_OBJECT

protected:
  WorkQueueWorker(WorkQueueInterface* queue);

signals:
  void init_task(QString name);
  void begin_task(QString name, int64_t workload);
  void progress(int64_t done, int64_t workload);

  void aborted_tasks(bool tasks_left);
  void failed_task(bool tasks_left);
  void succeeded_task(bool tasks_left);

protected slots:
  virtual void _receivedWork() = 0;
};


template<typename data_t>
class WorkQueueWorkerImplementation;

} // namespace internal


/*
This class needs to be defined for each data type seperately.

Mandatory Functions:
- constructor `task_processor_t<data_t>(data_t)`
- static `QString name_during_init(data_t)`
- `bool process(int64_t begin, int64_t end)`
- `void aborted(int64_t already_processed_begin, int64_t already_processed_end)`
- `bool finished()`

Mandatory Variables:
- int64_t work_amount
- int64_t block_size
- QString name
*/
template<typename data_t>
struct task_processor_t;

template<typename data_t>
class ThreadedWorkQueue final : public internal::WorkQueueInterface
{
public:
  typedef data_t data_type;

  ThreadedWorkQueue();

  void enqueue(data_t data);
  bool is_empty();

private:
  friend class internal::WorkQueueWorkerImplementation<data_t>;

  enum class error_t
  {
    empty_stack,
  };

  QThread thread;

  QMutex queue_mutex;
  QQueue<data_t> queue;

  std::atomic<uint64_t> running_jobs;
  std::atomic<uint64_t> aborted_jobs;

  data_t dequeue();

  void abort() override;
};

#include <core_library/work_queue.inl>

#endif // CORELIBRARY_WORK_QUEUE_HPP_
