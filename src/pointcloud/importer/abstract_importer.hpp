#ifndef POINTCLOUD_IMPORTER_ABSTRACTIMPORTER_HPP_
#define POINTCLOUD_IMPORTER_ABSTRACTIMPORTER_HPP_

#include <pointcloud/pointcloud.hpp>
#include <QObject>

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

  const std::string input_file;
  const int64_t total_num_bytes = 0;
  state_t state = IDLE;

  PointCloud pointcloud;

  AbstractPointCloudImporter(const std::string& input_file, int64_t total_num_bytes);
  ~AbstractPointCloudImporter();

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

#endif // POINTCLOUD_IMPORTER_ABSTRACTIMPORTER_HPP_
