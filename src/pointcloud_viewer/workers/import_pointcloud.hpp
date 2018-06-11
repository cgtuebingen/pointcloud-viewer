#ifndef POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
#define POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_

#include <pointcloud_viewer/point_cloud.hpp>
#include <QSharedPointer>
#include <QObject>

/**
The function responsible for import point clouds.
*/
QSharedPointer<PointCloud> import_point_cloud(QWidget* parent, QString file);


// ======== Implementation =============================================================================================
// I would have prefered to put the class definitions to put into the cpp, but the moc system enforced me to put them
// into a header file.

/**
Parent class for different kinds of PointCloud formats to import.
*/
class AbstractPointCloudImporter : public QObject
{
  Q_OBJECT

public:
  enum state_t
  {
    IDLE,
    RUNNING,
    SUCCEEDED,
    RUNTIME_ERROR,
    CANCELED,
  };

  std::istream& input_stream;
  int64_t total_num_bytes = 0;
  state_t state = IDLE;

  QSharedPointer<PointCloud> point_cloud;

  AbstractPointCloudImporter(std::istream& input_stream, int64_t total_num_bytes);

  constexpr static int progress_max(){return 65536;}

public slots:
  void import();
  void cancel();

signals:
  void update_progress(int progress);
  void finished();

protected:
  bool handle_loaded_chunk(int64_t num_bytes_processed);

protected slots:
  virtual bool import_implementation() = 0;

private:
  int64_t num_bytes_processed = 0;
};

class PlyImporter final : public AbstractPointCloudImporter
{
  Q_OBJECT

public:
  PlyImporter(std::istream& input_stream, int64_t total_num_bytes);

protected:
  bool import_implementation() override;
};

#endif // POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
