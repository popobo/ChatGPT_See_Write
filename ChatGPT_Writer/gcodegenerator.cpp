#include "gcodegenerator.h"

static const char* db_resource_path = ":/gcode_database/gcode.sqlite";
static const char* db_temp_name = "/gcode.sqlite";

GcodeGenerator::GcodeGenerator(): m_db(QSqlDatabase::addDatabase("QSQLITE")),
                                  m_thread(new QThread())
{
    m_thread->start();
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(this, &GcodeGenerator::signalInitDb, this, &GcodeGenerator::slotInitDb);

    emit signalInitDb();
}

GcodeGenerator::~GcodeGenerator()
{
    m_db.close();
}

bool GcodeGenerator::isDbOpen()
{
    return m_db.isOpen();
}

void GcodeGenerator::slotInitDb()
{
    QFile dbFile(db_resource_path);
    if (!dbFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening database file from resources!";
        assert(false);
        return;
    }

    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + db_temp_name;
    QFile tempFile(tempPath);
    if (tempFile.exists() || !tempFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error creating temprory file!";
        assert(false);
        return;
    }

    tempFile.write(dbFile.readAll());
    tempFile.close();

    m_db.setDatabaseName(tempPath);
    if (!m_db.open())
    {
        qDebug() << "Error opening database: " << m_db.lastError().text();
        assert(false);
        return;
    }
}
