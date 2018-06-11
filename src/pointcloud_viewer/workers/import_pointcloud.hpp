#ifndef POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
#define POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_

#include <QProgressDialog>
#include <functional>

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

bool import_point_cloud(QWidget* parent, QString file);

#endif // POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
