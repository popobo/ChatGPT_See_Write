#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTextBrowser>
#include "camera.h"
#include "ocrcontroller.h"
#include "gptcontroller.h"
#include "serialport.h"
#include "gcodegenerator.h"
#include "directionwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void layoutInit();

    void moduleInit();

    void initGcodeGenerator();

private slots:
    void _scanCameraFin(const QStringList& list);
    void _showImage(const QImage &image);
    void _openCamera();
    void _openCameraFin(bool ret);
    void _saveImage();
    void _saveImageFin(const QImage& saveImage);
    void _sendData();
    void _orcResponseHandle(const QString& result);
    void _gptResponseHandle(const QString& result);
    void _serialOpened(bool ret);

private:
    /* 主容器，Widget也可以当作一种容器 */
    QWidget* m_mainWidget;

    /* 滚动区域，方便开发高分辨率 */
    QScrollArea *m_scrollArea;

    /* 将采集到的图像使用Widget显示 */
    QLabel* m_displayLabel;

    QHBoxLayout* m_hboxLayoutMain;

    QVBoxLayout* m_vboxLayoutControl;

    QVBoxLayout* m_vboxLayoutText;

    /* 界面右侧区域容器 */
    QWidget* m_rightWidget;

    QWidget* m_middleWidget;

    /* 界面右侧区域显示拍照的图片 */
    QLabel* m_photoLabel;

    /* 界面右侧区域摄像头设备下拉选择框 */
    QComboBox* m_comboBoxCameras;

    QPushButton* m_saveImageButton;

    QPushButton* m_openCameraButton;

    QTextBrowser* m_receivedBrowser;

    QTextEdit* m_sendEdit;

    QPushButton* m_serialButton;

    QPushButton* m_sendButton;

    DirectionWidget* m_directionWdiget;

    QPushButton* m_penUpButton;
    QPushButton* m_penDownButton;
    QWidget* m_penWidget;
    QHBoxLayout* m_hLayoutPen;

    QPushButton* m_setOPButton;

private:
    Camera* m_camera = nullptr;
    QThread m_cameraThread;

    OCRController* m_ocrController = nullptr;
    QThread m_ocrControllerThread;

    GPTController* m_gptController = nullptr;
    QThread m_gptControllerThread;

    QScopedPointer<SerialPort> m_serialPort;
    QScopedPointer<GcodeGenerator> m_gcodeGenerator;
};
#endif // MAINWINDOW_H
