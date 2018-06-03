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

  void begin_task(QString name, int workload);
  void progress(int done, int workload);

  void aborted_tasks(bool tasks_left);
  void succeeded_task(bool tasks_left);
  void failed_task(bool tasks_left);
};

template<typename data_t>
class WorkQueueWorkerImplementation;

} // namespace internal


/*
This class needs to be defined for each data type seperately.

Mandatory Functions:
- constructor `task_processor_t<data_t>(data_t)`
- `bool process(int begin, int end)`
- `void aborted(int already_processed_begin, int already_processed_end)`
- `bool finished()`

Mandatory Variables:
- int work_amount
- int block_size
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
