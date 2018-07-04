#include <pointcloud_viewer/workers/offline_renderer.hpp>
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

QPair<RenderSettings, bool> ask_for_render_settings(RenderSettings prevSettings)
{
  QDialog dialog;

  dialog.setWindowTitle("Render Settings");

  QDialogButtonBox* buttons = new QDialogButtonBox;
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
#if 0
  renderSettings.target_video_file = videoFile;
  if(!enableVideoOutput->isChecked())
    use_result = false;
#endif


  renderSettings.target_images_directory = imageDirectory;
#if 0
  if(!enableImageOutput->isChecked())
    use_result = false;
#endif

  return qMakePair(renderSettings, !use_result);
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

  renderSettings.resolution = QSize(1920, 1080);
  renderSettings.framerate = 25;

  return renderSettings;
}
