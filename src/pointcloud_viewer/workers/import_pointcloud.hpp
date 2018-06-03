#ifndef POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
#define POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_

#include <QProgressDialog>

#include <core_library/work_queue.hpp>

class MainWindow;

/*
Class responsible for actually loading point clouds.

It also shows up the progress dialog.

*/

class ImportPointCloud final : public QObject
{
  Q_OBJECT
public:
  ImportPointCloud(MainWindow* mainWindow);
  ~ImportPointCloud();

public slots:
  void importPointCloudLayer(QString filename);

private:
  struct PointCloudFile;

  friend struct task_processor_t<ImportPointCloud::PointCloudFile>;

  MainWindow* const mainWindow;
  QProgressDialog* progressDialog = nullptr;

  ThreadedWorkQueue<PointCloudFile>* const point_cloud_loader;

  void closeProgressDialog();

private slots:
  void init_task(QString name);
  void begin_task(QString name, int64_t workload);
  void progress(int64_t done, int64_t workload);

  void aborted_tasks(bool tasks_left);
  void succeeded_task(bool tasks_left);
  void failed_task(bool tasks_left);
};

#endif // POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
