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
    void penUp();

    void penDown();

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
    int32_t m_chLineIndex = 0;
    int32_t m_lineIndex = 0;
};

#endif // GCODEGENERATOR_H
