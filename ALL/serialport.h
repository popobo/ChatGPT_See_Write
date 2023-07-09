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

    void write(const QString& data);

    void write(const QStringList& strList);

signals:
    void opened(bool result);

private slots:
    void _close();
    void _open();
    void _init();
    void _handleData(const QString& data);
    void _handleListData(const QStringList& strList);

private:
    QScopedPointer<QSerialPort> m_serialPort;

    QThread m_serialThread;
};

#endif // SERIALPORT_H
