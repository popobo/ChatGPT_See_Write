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

signals:
    void signalInitDb();

private slots:
    void slotInitDb();

private:
    QSqlDatabase m_db;

    QThread* m_thread;
};

#endif // GCODEGENERATOR_H
