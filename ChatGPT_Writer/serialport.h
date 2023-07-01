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
    SerialPort(QObject* parent = nullptr);

    void write(QString data);

    bool open();

    void close();

    bool isOpen();

signals:
    void dataReady(QString data);

private slots:
    void handleData(QString data);

private:
    QSerialPort* m_serialPort;

    QThread* m_serialThread;
};

#endif // SERIALPORT_H
