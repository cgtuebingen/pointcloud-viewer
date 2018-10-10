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
    INVALID_FILE,
    CANCELED,
  };

  enum class canceled_t{};

  const std::string input_file;
  state_t state = IDLE;

  PointCloud pointcloud;

  AbstractPointCloudImporter(const std::string& input_file);
  ~AbstractPointCloudImporter();

  constexpr static int progress_max(){return 65536;}

  static QSharedPointer<AbstractPointCloudImporter> importerForSuffix(QString suffix, std::string filepath);
  static QString allSupportedFiletypes();

public slots:
  void import();
  void cancel();

signals:
  void update_progress(int progress);
  void finished();

protected:
  int64_t total_progress = 0;
  void handle_loaded_chunk(int64_t progress);

protected slots:
  virtual bool import_implementation() = 0;
};

#endif // POINTCLOUD_IMPORTER_ABSTRACTIMPORTER_HPP_
