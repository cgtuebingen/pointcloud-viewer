#include <pointcloud_viewer/workers/import_pointcloud.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>

struct ImportPointCloud::PointCloudFile
{
  QString file;
};

template<>
struct task_processor_t<ImportPointCloud::PointCloudFile>
{
  typedef ImportPointCloud::PointCloudFile data_t;

  QFileInfo file;

  int64_t work_amount;
  const int block_size = 1;
  QString name;

  task_processor_t(data_t);

  static QString name_during_init(data_t);

  bool process(int begin, int end);
  void aborted(int already_processed_begin, int already_processed_end);
  bool finished();
};

ImportPointCloud::ImportPointCloud(MainWindow* mainWindow)
  : mainWindow(mainWindow),
    point_cloud_loader(new ThreadedWorkQueue<PointCloudFile>)
{
  connect(point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::init_task, this, &ImportPointCloud::init_task);
  connect(point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::begin_task, this, &ImportPointCloud::begin_task);
  connect(point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::progress, this, &ImportPointCloud::progress);
  connect(point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::aborted_tasks, this, &ImportPointCloud::aborted_tasks);
  connect(point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::succeeded_task, this, &ImportPointCloud::succeeded_task);
  connect(point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::failed_task, this, &ImportPointCloud::failed_task);
}

ImportPointCloud::~ImportPointCloud()
{
  delete point_cloud_loader;
  closeProgressDialog();
}

void ImportPointCloud::importPointCloudLayer(QString filename)
{
  filename = QFileInfo(filename).absoluteFilePath();

  point_cloud_loader->enqueue(PointCloudFile{filename});
}

void ImportPointCloud::closeProgressDialog()
{
  delete progressDialog;
  progressDialog = nullptr;
}

void ImportPointCloud::init_task(QString name)
{
  closeProgressDialog();
  progressDialog = new QProgressDialog(QString("Importing Point Cloud Layer\n<%1>").arg(name), "&Abort", 0, 65536, mainWindow);
  progressDialog->setWindowModality(Qt::WindowModal);
  progressDialog->setMinimumDuration(200);
  connect(progressDialog, &QProgressDialog::canceled, point_cloud_loader, &ThreadedWorkQueue<PointCloudFile>::abort);

  progressDialog->setValue(0);
  progressDialog->show();
}

void ImportPointCloud::begin_task(QString name, int64_t workload)
{
  Q_UNUSED(name);
  Q_UNUSED(workload);
}

typedef long double float86_t;

void ImportPointCloud::progress(int64_t done, int64_t workload)
{
  if(progressDialog)
  {
    int value = int((float86_t(done) / float86_t(workload)) * 65536 + float86_t(0.5));
    value = glm::clamp(0, 65536, value);
    progressDialog->setValue(value);
    // FIXME: why does this crash?
  }
}

void ImportPointCloud::aborted_tasks(QString name, bool tasks_left)
{
  Q_UNUSED(tasks_left);
  closeProgressDialog();
}

void ImportPointCloud::succeeded_task(QString name, bool tasks_left)
{
  Q_UNUSED(tasks_left);
  closeProgressDialog();
}

void ImportPointCloud::failed_task(QString name, bool tasks_left)
{
  // TODO collect errors and show them after the last one ended

  Q_UNUSED(tasks_left);
  closeProgressDialog();
}

// ======== task_processor_t<ImportPointCloud::PointCloudFile> ========

task_processor_t<ImportPointCloud::PointCloudFile>::task_processor_t(data_t data)
  : file(data.file),
    name(file.fileName())
{
  qint64 filesize = file.size();
  work_amount = filesize;

  work_amount = 10;
}

QString task_processor_t<ImportPointCloud::PointCloudFile>::name_during_init(data_t data)
{
  return QFileInfo(data.file).fileName();
}

bool task_processor_t<ImportPointCloud::PointCloudFile>::process(int begin, int end)
{
  qDebug() << begin << end;
  QThread::sleep(1);
  return true;
}

void task_processor_t<ImportPointCloud::PointCloudFile>::aborted(int already_processed_begin, int already_processed_end)
{
}

bool task_processor_t<ImportPointCloud::PointCloudFile>::finished()
{
  return true;
}

