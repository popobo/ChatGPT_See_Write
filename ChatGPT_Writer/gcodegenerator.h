#ifndef GCODEGENERATOR_H
#define GCODEGENERATOR_H

#include <QtSql>

class GcodeGenerator: public QObject
{
    Q_OBJECT

public:
    GcodeGenerator();

    ~GcodeGenerator();

    bool isDbOpen();

    void sendData(const QString& data);

signals:
    void signalInitDb();
    void signalDataReady(const QString& data);

private slots:
    void slotInitDb();
    void slotHandleData(const QString& data);

private:
    QSqlDatabase m_db;

    QThread* m_thread;
};

#endif // GCODEGENERATOR_H
