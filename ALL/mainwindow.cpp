#include "mainwindow.h"
#include "logger.h"
#include "common_constant.h"
#include <QGuiApplication>
#include <QScreen>

static const char* STRAR_CAMERA = "start camera";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    layoutInit();

    moduleInit();
}

MainWindow::~MainWindow()
{
    m_cameraThread.quit();
    m_ocrControllerThread.quit();
    m_cameraThread.wait();
    m_ocrControllerThread.wait();
}

static void initButton(QPushButton* button, const QString& name, bool enable, int32_t maxWidth, int32_t maxHeight)
{
    button->setText(name);
    button->setEnabled(enable);
    button->setMaximumHeight(maxHeight);
    button->setMaximumWidth(maxWidth);
}

void MainWindow::layoutInit()
{
    /* 获取屏幕的分辨率，Qt官方建议使用这
     * 种方法获取屏幕分辨率，防上多屏设备导致对应不上
     * 注意，这是获取整个桌面系统的分辨率
     */
    QList <QScreen *> list_screen =  QGuiApplication::screens();

    /* 如果是ARM平台，直接设置大小为屏幕的大小 */
#if __arm__
    /* 重设大小 */
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
#else
    /* 否则则设置主窗体大小为800x480 */
    this->resize(800, 480);
#endif

    /* 实例化与布局，常规操作 */
    m_mainWidget = new QWidget();
    m_photoLabel = new QLabel();
    m_rightWidget = new QWidget();
    m_middleWidget = new QWidget();
    m_comboBoxCameras = new QComboBox();
    m_saveImageButton = new QPushButton();
    m_openCameraButton = new QPushButton();
    m_sendButton = new QPushButton();
    m_serialButton = new QPushButton();
    m_scrollArea = new QScrollArea();
    m_displayLabel = new QLabel(m_scrollArea);
    m_sendEdit = new QTextEdit();
    m_receivedBrowser = new QTextBrowser();
    m_vboxLayoutControl = new QVBoxLayout();
    m_vboxLayoutText = new QVBoxLayout();
    m_hboxLayoutMain = new QHBoxLayout();

    m_vboxLayoutControl->addWidget(m_photoLabel);
    m_vboxLayoutControl->addWidget(m_comboBoxCameras);
    m_vboxLayoutControl->addWidget(m_saveImageButton);
    m_vboxLayoutControl->addWidget(m_openCameraButton);
    m_vboxLayoutControl->addWidget(m_sendButton);
    m_vboxLayoutControl->addWidget(m_serialButton);

    m_rightWidget->setLayout(m_vboxLayoutControl);

    m_vboxLayoutText->addWidget(m_receivedBrowser, 3);
    m_vboxLayoutText->addWidget(m_sendEdit, 2);
    m_middleWidget->setLayout(m_vboxLayoutText);

    m_hboxLayoutMain->addWidget(m_scrollArea, 3);
    m_hboxLayoutMain->addWidget(m_middleWidget, 2);
    m_hboxLayoutMain->addWidget(m_rightWidget, 1);

    m_mainWidget->setLayout(m_hboxLayoutMain);

    this->setCentralWidget(m_mainWidget);

    initButton(m_saveImageButton, "save image", false, 200, 40);
    initButton(m_openCameraButton, STRAR_CAMERA, true, 200, 40);
    initButton(m_sendButton, "send data", true, 200, 40);
    initButton(m_serialButton, "open serial", true, 200, 40);

    m_comboBoxCameras->setMaximumHeight(40);
    m_comboBoxCameras->setMaximumWidth(200);
    m_photoLabel->setMaximumSize(100, 75);
    // m_scrollArea->setMinimumWidth(this->width()- m_rightWidget->width() - m_middleWidget->width());

    /* 显示图像最大画面为xx */
    m_displayLabel->setMinimumWidth(m_scrollArea->width() * 0.75);
    m_displayLabel->setMinimumHeight(m_scrollArea->height() * 0.75);
    m_scrollArea->setWidget(m_displayLabel);

    /* 居中显示 */
    m_scrollArea->setAlignment(Qt::AlignCenter);

    /* 自动拉伸 */
    m_photoLabel->setScaledContents(true);
    m_displayLabel->setScaledContents(true);
}

void MainWindow::moduleInit()
{
    Logger::init();

    m_camera = new Camera();
    m_camera->moveToThread(&m_cameraThread);

    connect(&m_cameraThread, &QThread::finished, m_camera, &Camera::deleteLater);
    connect(m_openCameraButton, &QPushButton::clicked, this, &MainWindow::_openCamera);
    connect(m_camera, &Camera::scanCameraFin, this, &MainWindow::_scanCameraFin);
    connect(m_camera, &Camera::readyImage, this, &MainWindow::_showImage);
    connect(m_camera, &Camera::openCameraFin, this, &MainWindow::_openCameraFin);
    connect(m_saveImageButton, &QPushButton::clicked, this, &MainWindow::_saveImage);
    connect(m_camera, &Camera::saveImageFin, this, &MainWindow::_saveImageFin);

    m_cameraThread.start();
    m_camera->scanCamera();


    m_ocrController = new OCRController();
    m_ocrController->moveToThread(&m_ocrControllerThread);

    connect(&m_ocrControllerThread, &QThread::finished, m_ocrController, &OCRController::deleteLater);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::_sendData);
    connect(m_ocrController, &OCRController::response, this, &MainWindow::_orcResponseHandle);

    m_ocrControllerThread.start();

    m_gptController = new GPTController();
    m_gptController->moveToThread(&m_gptControllerThread);

    connect(m_ocrController, &OCRController::response, m_gptController, &GPTController::request);
    connect(m_gptController, &GPTController::response, this, &MainWindow::_gptResponseHandle);

    m_gptControllerThread.start();
}

void MainWindow::_scanCameraFin(const QStringList &list)
{
    for(const auto& str: list)
    {
        m_comboBoxCameras->addItem(str);
    }
}

void MainWindow::_showImage(const QImage &image)
{
    m_displayLabel->setPixmap(QPixmap::fromImage(image));

    if (!image.isNull())
        m_saveImageButton->setEnabled(true);
    else
        m_saveImageButton->setEnabled(false);
}

void MainWindow::_openCamera()
{
    if (!m_camera->isOpen())
    {
        m_camera->openCamera(m_comboBoxCameras->currentIndex());
    }
    else
    {
        m_camera->closeCamera();
        m_openCameraButton->setText(STRAR_CAMERA);
    }
}

void MainWindow::_openCameraFin(bool ret)
{
    if (ret)
        m_openCameraButton->setText("Camera Opened");
    else
        m_openCameraButton->setText("Failed to Open");
}

void MainWindow::_saveImage()
{
    m_camera->saveImage();
}

void MainWindow::_saveImageFin(const QImage &saveImage)
{
    m_photoLabel->setPixmap(QPixmap::fromImage(saveImage));
}

void MainWindow::_sendData()
{
    m_ocrController->request(QCoreApplication::applicationDirPath() + "/" + IMAGE_NAME);
}

void MainWindow::_orcResponseHandle(const QString& result)
{
    m_sendEdit->setText(result);
}

void MainWindow::_gptResponseHandle(const QString& result)
{
    m_receivedBrowser->setText(result);
}

