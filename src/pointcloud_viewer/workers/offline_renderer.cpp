#include <pointcloud_viewer/workers/offline_renderer.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>

RenderSettings ask_for_render_settings(RenderSettings prevSettings)
{
  QDialog dialog;

  dialog.setWindowTitle("Render Settings");

  QVBoxLayout* vbox = new QVBoxLayout;
  QHBoxLayout* hbox = new QHBoxLayout;
  QFormLayout* form = new QFormLayout;
  QDialogButtonBox* buttons = new QDialogButtonBox;

  dialog.setLayout(vbox);

  vbox->addLayout(form);
  vbox->addWidget(buttons);

  QSpinBox* resolution_width = new QSpinBox();
  resolution_width->setRange(128, 16384);
  resolution_width->setValue(prevSettings.encoderSettings.resolution().width());
  QSpinBox* resolution_height = new QSpinBox();
  resolution_height->setRange(128, 16384);
  resolution_height->setValue(prevSettings.encoderSettings.resolution().height());
  QWidget* resolutionWidget = new QWidget;
  hbox->addWidget(resolution_width, 1);
  hbox->addWidget(new QLabel("x"));
  hbox->addWidget(resolution_height, 1);
  resolutionWidget->setLayout(hbox);

  form->addRow("&Resolution", resolutionWidget);

  buttons->addButton(QDialogButtonBox::Ok);
  buttons->addButton(QDialogButtonBox::Cancel);

  QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  if(dialog.exec() != QDialog::Accepted)
    return RenderSettings();

  RenderSettings renderSettings;

  renderSettings.encoderSettings.setResolution(QSize(resolution_width->value(), resolution_height->value()));

  return renderSettings;
}

void render(MainWindow* mainWindow, RenderSettings renderSettings)
{
  QDialog dialog;

  dialog.setWindowTitle("Rendering Now...");

  if(dialog.exec() != QDialog::Accepted)
  {
    QMessageBox::warning(mainWindow, "Rendering aborted", "Rendering process was aborted");
    return;
  }

}

RenderSettings RenderSettings::defaultSettings()
{
  RenderSettings renderSettings;

  renderSettings.encoderSettings.setResolution(1920, 1080);

  return renderSettings;
}
