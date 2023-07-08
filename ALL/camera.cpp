#include "camera.h"
#include <QFile>
#include "logger.h"

static const qint32 FIR_AV_CA = 0;
static const qint32 MAX_CAMERAS = 3;

Camera::Camera(QObject *parent) : QObject(parent)
{
}

Camera::~Camera()
{
}

void Camera::scanCamera()
{
    QMetaObject::invokeMethod(this, "_scanCamera", Qt::QueuedConnection);
}

void Camera::_scanCamera()
{
    SPD_INFO("current thread is {0}", (int64_t)QThread::currentThread());
    QMutexLocker locker(&m_deviceListMutex);
    for (int32_t i = FIR_AV_CA; i < MAX_CAMERAS; i++)
    {
        QString device = "/dev/video" + QString(i);
        QFile file(device);
        if (file.exists())
        {
           m_deviceList.append(device);
        }
    }
    emit scanCameraFin(m_deviceList);
}
