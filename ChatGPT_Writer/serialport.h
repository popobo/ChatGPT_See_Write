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

    void open();

    void close();

    bool isOpen();

    void write(const QString& data);

    void write(const QStringList& strList);

signals:
    void signalClose();
    void signalOpen();
    void signalInit();
    void dataReady(const QString& data);
    void listDataReady(const QStringList& strList);
    void signalOpened(bool result);

private slots:
    void slotClose();
    void slotOpen();
    void slotInit();
    void handleData(const QString& data);
    void handleListData(const QStringList& strList);

private:
    QScopedPointer<QSerialPort> m_serialPort;

    QThread* m_serialThread;
};

#endif // SERIALPORT_H
