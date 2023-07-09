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

    void penUp();

    void penDown();

    void xMoveLeft(float value = 2.5);

    void xMoveRight(float value = 2.5);

    void yMoveForward(float value = 2.5);

    void yMoveBackwards(float value = 2.5);

    void setOriginPoint();

private:

    void initDb();

private slots:
    void _initDb();
    void _handleData(const QString& data);

signals:
    void gcodeReady(const QString& str);
    void gcodeListReady(const QStringList& strList);

private:
    QSqlDatabase m_db;
    QThread m_thread;
    int32_t m_chLineIndex = 0;
    int32_t m_lineIndex = 0;
};

#endif // GCODEGENERATOR_H
