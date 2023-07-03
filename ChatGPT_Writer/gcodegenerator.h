#ifndef GCODEGENERATOR_H
#define GCODEGENERATOR_H

#include <QtSql>

class SerialPort;

class GcodeGenerator: public QObject
{
    Q_OBJECT

public:
    GcodeGenerator();

    ~GcodeGenerator();

    bool isDbOpen();

    void sendData(const QString& data);

    void setSerialPort(const QSharedPointer<SerialPort>& serialPort);

private:
    struct Point
    {
        float x = 0.0;
        float y = 0.0;
    };


signals:
    void signalInitDb();
    void signalDataReady(const QString& data);

private slots:
    void slotInitDb();
    void slotHandleData(const QString& data);

private:
    QSqlDatabase m_db;

    QThread* m_thread;

    QSharedPointer<SerialPort> m_serialPort;
};

#endif // GCODEGENERATOR_H
