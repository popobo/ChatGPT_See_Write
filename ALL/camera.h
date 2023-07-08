#ifndef CAMERA_H
#define CAMERA_H

#include <QMutex>
#include <QObject>
#include <QThread>

class Camera : public QObject
{
    Q_OBJECT

public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

    void scanCamera();

private slots:
    void _scanCamera();

signals:
    void scanCameraFin(QStringList list);

private:
    QStringList m_deviceList;
    QMutex m_deviceListMutex;
};

#endif // CAMERA_H
