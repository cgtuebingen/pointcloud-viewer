#include <pointcloud_viewer/workers/offline_renderer_dialogs.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QSplitter>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileInfo>
#include <QStandardPaths>
#include <QProgressBar>

QPair<RenderSettings, bool> ask_for_render_settings(QWidget* parent, RenderSettings prevSettings)
{
  QDialog dialog(parent);

  dialog.setWindowModality(Qt::ApplicationModal);
  dialog.setWindowTitle("Render Settings");

  QSplitter* splitter = new QSplitter(Qt::Horizontal);

  QGroupBox* group;
  QFormLayout* form;
  QHBoxLayout* hbox;
  QVBoxLayout* vbox;
  QWidget* formWidget;

  // ==== Renderer ====

  group = new QGroupBox("Renderer");
  form = new QFormLayout;

  group->setLayout(form);
  splitter->addWidget(group);

  QSpinBox* resolution_width = new QSpinBox();
  resolution_width->setRange(128, 16384);
  resolution_width->setValue(prevSettings.resolution.width());
  QSpinBox* resolution_height = new QSpinBox();
  resolution_height->setRange(128, 16384);
  resolution_height->setValue(prevSettings.resolution.height());
  QWidget* resolutionWidget = new QWidget;
  hbox = new QHBoxLayout;
  hbox->addWidget(resolution_width, 1);
  hbox->addWidget(new QLabel("x"));
  hbox->addWidget(resolution_height, 1);
  resolutionWidget->setLayout(hbox);

  QSpinBox* framerate = new QSpinBox();
  framerate->setRange(1, 250);
  framerate->setValue(prevSettings.framerate);
  framerate->setSuffix(" fps");

  form->addRow("&Resolution", resolutionWidget);
  form->addRow("&Framerate", framerate);

#if VIDEO_OUTPUT
  // ==== Video Output ====

  group = new QGroupBox("Video Output");
  vbox = new QVBoxLayout;
  form = new QFormLayout;

  formWidget = new QWidget;
  formWidget->setLayout(form);
  vbox->addWidget(formWidget, 1);

  QString videoFile = QFileInfo(prevSettings.target_video_file).dir().exists() ? prevSettings.target_video_file : "";
  QLabel* videoFile_label = new QLabel(".../"+QFileInfo(videoFile).fileName());
  videoFile_label->setWordWrap(true);
  QPushButton* chooseVideoOutputFile = new QPushButton("...");
  QObject::connect(chooseVideoOutputFile, &QPushButton::clicked, [&dialog, &videoFile, videoFile_label](){
    QString filter; // TODO
    QString path = QFileDialog::getSaveFileName(&dialog,
                                                "Video Output file",
                                                QFileInfo(videoFile).dir().exists() ? QFileInfo(videoFile).dir().absolutePath() : QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                filter);
    if(!path.isEmpty())
    {
      videoFile = path;
      videoFile_label->setText(".../"+QFileInfo(path).fileName());
    }
  });
  hbox = new QHBoxLayout;
  hbox->addWidget(videoFile_label, 10);
  hbox->addWidget(chooseVideoOutputFile, 0);
  form->addRow(hbox);

  QCheckBox* enableVideoOutput = new QCheckBox("Enable");
  vbox->addWidget(enableVideoOutput);

  group->setLayout(vbox);
  splitter->addWidget(group);

  enableVideoOutput->setChecked(true);
  QObject::connect(enableVideoOutput, &QCheckBox::toggled, formWidget, &QWidget::setEnabled);
  enableVideoOutput->setChecked(videoFile.isEmpty() == false);
#endif

  // ==== Frame Output ====

  group = new QGroupBox("Individual Frame Output");
  vbox = new QVBoxLayout;
  form = new QFormLayout;

  formWidget = new QWidget;
  formWidget->setLayout(form);
  vbox->addWidget(formWidget, 1);

  QString imageDirectory = prevSettings.target_images_directory.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+"/render_result" : prevSettings.target_images_directory;
  QLabel* imageDirectory_label = new QLabel(".../"+QFileInfo(imageDirectory).fileName());
  imageDirectory_label->setWordWrap(true);
  QPushButton* chooseImageDirectory = new QPushButton("...");
  QObject::connect(chooseImageDirectory, &QPushButton::clicked, [&dialog, &imageDirectory, imageDirectory_label](){
    QString path = QFileDialog::getExistingDirectory(&dialog, "Image Output directory", imageDirectory);
    if(!path.isEmpty())
    {
      imageDirectory = path;
      imageDirectory_label->setText(".../"+QFileInfo(path).fileName());
    }
  });
  hbox = new QHBoxLayout;
  hbox->addWidget(imageDirectory_label, 10);
  hbox->addWidget(chooseImageDirectory, 0);
  form->addRow(hbox);

  QComboBox* imageFormat = new QComboBox;
  {
    QMap<QString, QString> supportedFormats;
    supportedFormats.insert("png", ".png");
    supportedFormats.insert("bmp", ".bmp");
    supportedFormats.insert("ppm", ".ppm");

    for(auto i=supportedFormats.begin(); i!=supportedFormats.end(); ++i)
      imageFormat->addItem(i.key(), i.value());
    imageFormat->setCurrentText(supportedFormats.key(prevSettings.image_format, "png"));
  }
  form->addRow("Format", imageFormat);

  group->setLayout(vbox);
  splitter->addWidget(group);

#if VIDEO_OUTPUT
  QCheckBox* enableImageOutput = new QCheckBox("Enable");
  vbox->addWidget(enableImageOutput);

  enableImageOutput->setChecked(true);
  QObject::connect(enableImageOutput, &QCheckBox::toggled, formWidget, &QWidget::setEnabled);
  enableImageOutput->setChecked(prevSettings.target_images_directory.isEmpty() == false);
#endif

  // ==== Buttons ====

  QDialogButtonBox* buttons = new QDialogButtonBox;
  QVBoxLayout* root = new QVBoxLayout;
  dialog.setLayout(root);
  root->addWidget(splitter);
  root->addWidget(buttons);

  buttons->addButton(QDialogButtonBox::Ok);
  buttons->addButton(QDialogButtonBox::Cancel);

  QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  if(dialog.exec() != QDialog::Accepted)
    return qMakePair(prevSettings, true);

  bool use_result = true;
  RenderSettings renderSettings;

  renderSettings.resolution = QSize(resolution_width->value(), resolution_height->value());
  renderSettings.framerate = framerate->value();
#if VIDEO_OUTPUT
  renderSettings.target_video_file = videoFile;
  if(!enableVideoOutput->isChecked())
    use_result = false;
#endif


  renderSettings.target_images_directory = imageDirectory;
#if VIDEO_OUTPUT
  if(!enableImageOutput->isChecked())
  {
    renderSettings.export_images = false;
    use_result = false;
  }
#else
  renderSettings.export_images = true;
#endif
  renderSettings.image_format = imageFormat->currentData().toString();

  return qMakePair(renderSettings, !use_result);
}


void MainWindow::offline_render()
{
  QPair<RenderSettings, bool> result = ask_for_render_settings(this, renderSettings);

  renderSettings = result.first;
  bool was_canceled = result.second;

  if(was_canceled)
    return;

  OfflineRenderer offlineRenderer(&viewport, flythrough, renderSettings);

  QDialog dialog(this);

  dialog.setWindowModality(Qt::ApplicationModal);
  dialog.setWindowTitle("Rendering Now...");

  QVBoxLayout* root = new QVBoxLayout;

  // ==== Progressbar ====
  QProgressBar* progressBar = new QProgressBar;

  progressBar->setMaximum(offlineRenderer.total_number_frames-1);
  progressBar->setTextVisible(true);
  root->addWidget(progressBar, 0);

  // ==== image_preview ====
  QLabel* rendered_frame = new QLabel("Please wait...");
  root->addWidget(rendered_frame, 1);

  connect(&offlineRenderer, &OfflineRenderer::rendered_frame, [rendered_frame, progressBar](int frame_index, QImage frame){
    progressBar->setValue(frame_index);
    progressBar->setFormat("%v/%m (%p%)");
    rendered_frame->setPixmap(QPixmap::fromImage(frame.scaledToHeight(384)));
  });

  // ==== Buttons ====
  QDialogButtonBox* buttons = new QDialogButtonBox;

  dialog.setLayout(root);

  root->addWidget(buttons, 0);

  buttons->addButton(QDialogButtonBox::Abort);

  QObject::connect(buttons, &QDialogButtonBox::rejected, &offlineRenderer, &OfflineRenderer::abort);
  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
  QObject::connect(&offlineRenderer, &OfflineRenderer::finished, &dialog, &QDialog::accept);

  offlineRenderer.start();

  if(dialog.exec() != QDialog::Accepted)
  {
    QMessageBox::warning(this, "Rendering aborted", "Rendering process was aborted");
    return;
  }
}

RenderSettings RenderSettings::defaultSettings()
{
  RenderSettings renderSettings;

  renderSettings.resolution = QSize(1920, 1080);
  renderSettings.framerate = 25;
  renderSettings.image_format = ".png";

  return renderSettings;
}
