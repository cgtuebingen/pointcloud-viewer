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
  resolution_width->setValue(prevSettings.encoderSettings.resolution().width());
  QSpinBox* resolution_height = new QSpinBox();
  resolution_height->setRange(128, 16384);
  resolution_height->setValue(prevSettings.encoderSettings.resolution().height());
  QWidget* resolutionWidget = new QWidget;
  hbox = new QHBoxLayout;
  hbox->addWidget(resolution_width, 1);
  hbox->addWidget(new QLabel("x"));
  hbox->addWidget(resolution_height, 1);
  resolutionWidget->setLayout(hbox);

  QDoubleSpinBox* framerate = new QDoubleSpinBox();
  framerate->setRange(1, 250);
  framerate->setDecimals(1);
  framerate->setValue(prevSettings.encoderSettings.frameRate());
  framerate->setSuffix(" fps");

  form->addRow("&Resolution", resolutionWidget);
  form->addRow("&Framerate", framerate);

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

  QComboBox* encodingMode = new QComboBox;
  {
    QMap<int, QString> values;
    values[QMultimedia::ConstantQualityEncoding] = "Constant Quality";
    values[QMultimedia::ConstantBitRateEncoding] = "Constant BitRate";
    values[QMultimedia::AverageBitRateEncoding] = "Average BitRate";
    values[QMultimedia::TwoPassEncoding] = "Two Pass";
    encodingMode->addItems(values.values());
  }
  form->addRow("&Encoding Mode", encodingMode);
  formWidget->setEnabled(false);

  group->setLayout(vbox);
  splitter->addWidget(group);

  enableVideoOutput->setChecked(true);
  QObject::connect(enableVideoOutput, &QCheckBox::toggled, formWidget, &QWidget::setEnabled);
  enableVideoOutput->setChecked(videoFile.isEmpty() == false);


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

  QCheckBox* enableImageOutput = new QCheckBox("Enable");
  vbox->addWidget(enableImageOutput);

  group->setLayout(vbox);
  splitter->addWidget(group);

  enableImageOutput->setChecked(true);
  QObject::connect(enableImageOutput, &QCheckBox::toggled, formWidget, &QWidget::setEnabled);
  enableImageOutput->setChecked(prevSettings.target_images_directory.isEmpty() == false);

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

  RenderSettings renderSettings;

  renderSettings.encoderSettings.setResolution(QSize(resolution_width->value(), resolution_height->value()));
  renderSettings.encoderSettings.setFrameRate(framerate->value());
  renderSettings.encoderSettings.setEncodingMode(QMultimedia::EncodingMode(encodingMode->currentIndex()));

  renderSettings.target_video_file = videoFile;
  renderSettings.target_images_directory = imageDirectory;

  if(!enableVideoOutput->isChecked())
    renderSettings.target_video_file.clear();
  if(!enableImageOutput->isChecked())
    renderSettings.target_images_directory.clear();

  return qMakePair(renderSettings, false);
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
  renderSettings.encoderSettings.setFrameRate(25);

  return renderSettings;
}
