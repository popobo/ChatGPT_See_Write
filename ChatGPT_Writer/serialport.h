#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QQueue>
#include <QThread>

class SerialPort: public QObject
{
    Q_OBJECT

public:
    SerialPort();

    void write(const QString& data);

    bool open();

    void close();

    bool isOpen();

signals:
    void dataReady(const QString& data);

private slots:
    void handleData(const QString& data);

private:
    QScopedPointer<QSerialPort> m_serialPort;

    QThread* m_serialThread;
};

#endif // SERIALPORT_H
