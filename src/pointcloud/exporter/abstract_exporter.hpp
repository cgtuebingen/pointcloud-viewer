#ifndef POINTCLOUD_IMPORTER_ABSTRACTEXPORTER_HPP_
#define POINTCLOUD_IMPORTER_ABSTRACTEXPORTER_HPP_

#include <pointcloud/pointcloud.hpp>
#include <QObject>

/**
Parent class for different kinds of PointCloud formats to import.
*/
class AbstractPointCloudExporter : public QObject
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

  enum class canceled_t{};

  const std::string output_file;
  state_t state = IDLE;

  const PointCloud& pointcloud;

  AbstractPointCloudExporter(const std::string& output_file, const PointCloud& pointcloud);
  ~AbstractPointCloudExporter();

  constexpr static int progress_max(){return 65536;}

  static QSharedPointer<AbstractPointCloudExporter> exporterForSuffix(QString suffix, std::string filepath, const PointCloud& pointcloud);
  static QString addMissingSuffix(QString filepath, QString selectedFilter);
  static QString allSupportedFiletypes();

public slots:
  void export_now();
  void cancel();

signals:
  void update_progress(int progress);
  void finished();

protected:
  int64_t total_progress = 0;
  bool handle_written_chunk(int64_t progress);

protected slots:
  virtual bool export_implementation() = 0;
};

#endif // POINTCLOUD_IMPORTER_ABSTRACTEXPORTER_HPP_
