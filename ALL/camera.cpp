#include "camera.h"
#include <QFile>
#include <QImage>
#include <QGuiApplication>
#include "logger.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

static const qint32 FIR_AV_CA = 0;
static const qint32 MAX_CAMERAS = 3;
static const qint32 FRAME_RATE = 5;

static const qint32 SAVE_IMAGE_FLAG_INIT = 0;
static const qint32 SAVE_IMAGE_FLAG = 1;

Camera::Camera(QObject *parent) : QObject(parent)
{
    m_capture = new cv::VideoCapture();
    m_timer = new QTimer(this);
    m_saveImageFlag.store(SAVE_IMAGE_FLAG_INIT);

    connect(m_timer, &QTimer::timeout, this, &Camera::_timerTimeOut);
}

Camera::~Camera()
{
}

void Camera::scanCamera()
{
    QMetaObject::invokeMethod(this, "_scanCamera", Qt::QueuedConnection);
}

void Camera::openCamera(qint32 index)
{
    QMetaObject::invokeMethod(this, "_openCamera", Qt::QueuedConnection, Q_ARG(qint32, index));
}

void Camera::openCamera(const QString &device)
{
    QMetaObject::invokeMethod(this, "_openCamera", Qt::QueuedConnection, Q_ARG(QString, device));
}

void Camera::closeCamera()
{
    QMetaObject::invokeMethod(this, "_closeCamera", Qt::QueuedConnection);
}

bool Camera::isOpen()
{
    return m_capture && m_capture->isOpened();
}

void Camera::saveImage()
{
    m_saveImageFlag.store(SAVE_IMAGE_FLAG);
}

void Camera::_scanCamera()
{
    SPD_INFO("current thread is {0}", (qint64)QThread::currentThread());
    QMutexLocker locker(&m_deviceListMutex);
    for (qint32 i = FIR_AV_CA; i < MAX_CAMERAS; i++)
    {
        QString device;
        device.sprintf("/dev/video%d", i);
        QFile file(device);
        if (file.exists())
        {
           m_deviceList.append(device);
        }
    }
    emit scanCameraFin(m_deviceList);
}

void Camera::_openCamera(qint32 index)
{
    SPD_INFO("_openCamera {0}", index);
    if (m_capture->isOpened())
    {
        m_capture->release();
    }

    bool ret = m_capture->open(index);
    if (ret)
    {
        SPD_INFO("open capture successfully");
    }
    else
    {
        SPD_ERROR("fail to open capture");
    }

    m_timer->start(1000 / FRAME_RATE);
    emit openCameraFin(ret);
}

void Camera::_openCamera(const QString &device)
{
    SPD_INFO("_openCamera {0}", device.toStdString());
    if (m_capture->isOpened())
    {
        m_capture->release();
    }

    bool ret = m_capture->open(device.toStdString());
    if (ret)
    {
        SPD_INFO("open capture successfully");
    }
    else
    {
        SPD_ERROR("fail to open capture");
    }

    m_timer->start(1000 / FRAME_RATE);
    emit openCameraFin(ret);
}

void Camera::_timerTimeOut()
{
    if (!m_capture->isOpened()) {
        m_timer->stop();
        return;
    }

    static cv::Mat frame;

    *m_capture >> frame;
    if (frame.cols)
        emit readyImage(matToQImage(frame));
}

void Camera::_closeCamera()
{
    if (m_capture->isOpened())
    {
        m_capture->release();
    }
}

QImage Camera::matToQImage(const cv::Mat &img)
{
    if(img.type() == CV_8UC3) {
        const uchar *pimg = (const uchar*)img.data;

        QImage qImage(pimg, img.cols, img.rows, img.step,
                      QImage::Format_RGB888);

        if (m_saveImageFlag.load() == SAVE_IMAGE_FLAG)
        {
            saveImageToLocal(qImage);
            m_saveImageFlag.store(SAVE_IMAGE_FLAG_INIT);
        }

        return qImage.rgbSwapped();
    }

    return QImage();
}

void Camera::saveImageToLocal(const QImage& saveImage)
{
    /* 判断图像是否为空 */
    if (!saveImage.isNull()) {
        QString fileName =
                QCoreApplication::applicationDirPath() + "/test.png";
        SPD_INFO("saving image {0}", fileName.toStdString());

        saveImage.save(fileName, "PNG", -1);
        emit saveImageFin(saveImage);
    }
}

