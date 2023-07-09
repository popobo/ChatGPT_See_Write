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

    ~SerialPort();

    void init();

    void open();

    void close();

    bool isOpen();

signals:
    void opened(bool result);

public slots:
    void handleData(const QString& data);
    void handleListData(const QStringList& strList);

private slots:
    void _close();
    void _open();
    void _init();

private:
    QScopedPointer<QSerialPort> m_serialPort;

    QThread m_serialThread;
};

#endif // SERIALPORT_H
