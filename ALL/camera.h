#ifndef CAMERA_H
#define CAMERA_H

#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTimer>

namespace cv {
class VideoCapture;
class Mat;
}

class Camera : public QObject
{
    Q_OBJECT

public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

    void scanCamera();

    void openCamera(qint32 index);

    void openCamera(const QString& device);

    void closeCamera();

    bool isOpen();

private:
    QImage matToQImage(const cv::Mat&);

private slots:
    void _scanCamera();
    void _openCamera(qint32 index);
    void _openCamera(const QString& device);
    void _timerTimeOut();
    void _closeCamera();

signals:
    void scanCameraFin(QStringList list);
    void readyImage(const QImage&);
    void openCameraFin(bool ret);

private:
    QStringList m_deviceList;
    QMutex m_deviceListMutex;

    cv::VideoCapture* m_capture;

    QTimer* m_timer;
};

#endif // CAMERA_H
