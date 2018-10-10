#include <pointcloud_viewer/workers/export_pointcloud.hpp>
#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud/exporter/abstract_exporter.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProgressDialog>
#include <QAbstractEventDispatcher>

#include <fstream>

bool export_point_cloud(QWidget* parent, QString filepath, const PointCloud& pointcloud, QString selectedFilter)
{
  filepath = AbstractPointCloudExporter::addMissingSuffix(filepath, selectedFilter);

  QFileInfo file(filepath);

  const std::string filepath_std = file.absoluteFilePath().toStdString();

  QString suffix = file.suffix();

  QSharedPointer<AbstractPointCloudExporter> exporter = AbstractPointCloudExporter::exporterForSuffix(selectedFilter, filepath_std, pointcloud);

  Q_ASSERT(exporter != nullptr);

  QProgressDialog progressDialog(QString("Exporting Pointcloud \n<%1>").arg(file.fileName()), "&Abort", 0, AbstractPointCloudExporter::progress_max(), parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);

  progressDialog.show();

  QThread thread;
  exporter->moveToThread(&thread);

  bool waiting = true;

  QObject::connect(exporter.data(), &AbstractPointCloudExporter::update_progress, &progressDialog, &QProgressDialog::setValue);
  QObject::connect(exporter.data(), &AbstractPointCloudExporter::finished, &thread, &QThread::quit);
  QObject::connect(&progressDialog, &QProgressDialog::canceled, exporter.data(), &AbstractPointCloudExporter::cancel);
  QObject::connect(&thread, &QThread::started, exporter.data(), &AbstractPointCloudExporter::export_now);
  QObject::connect(&thread, &QThread::finished, [&waiting](){waiting=false;});

  thread.start();

  while(waiting)
    QCoreApplication::processEvents(QEventLoop::EventLoopExec | QEventLoop::DialogExec | QEventLoop::WaitForMoreEvents);

  switch(exporter->state)
  {
  case AbstractPointCloudExporter::CANCELED:
    QMessageBox::warning(parent, "Exporting Cancelled", QString("Exporting the pointcloud file was canceled by the user."));
    return false;
  case AbstractPointCloudExporter::RUNNING:
  case AbstractPointCloudExporter::IDLE:
    QMessageBox::warning(parent, "Unknown Error", QString("Internal error"));
    return false;
  case AbstractPointCloudExporter::RUNTIME_ERROR:
    QMessageBox::warning(parent, "Export Error", QString("Couldn't export to the file <%0>. Probably an io error.").arg(file.fileName()));
    return false;
  case AbstractPointCloudExporter::SUCCEEDED:
    return true;
  }

  progressDialog.hide();

  return false;
}
