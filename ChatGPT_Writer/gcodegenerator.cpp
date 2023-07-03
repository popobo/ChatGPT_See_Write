#include "gcodegenerator.h"
#include "logger.h"
#include "serialport.h"

static const char* db_resource_path = ":/gcode_database/gcode.sqlite";
static const char* db_temp_name = "/gcode.sqlite";

GcodeGenerator::GcodeGenerator(): m_thread(new QThread())
{
    m_thread->start();
    this->moveToThread(m_thread);

    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(this, &GcodeGenerator::signalInitDb, this, &GcodeGenerator::slotInitDb);
    connect(this, &GcodeGenerator::signalDataReady, this, &GcodeGenerator::slotHandleData);

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

void GcodeGenerator::sendData(const QString &data)
{
    emit signalDataReady(data);
}

void GcodeGenerator::setSerialPort(const QSharedPointer<SerialPort> &serialPort)
{
    m_serialPort = serialPort;
}

void GcodeGenerator::slotInitDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QFile dbFile(db_resource_path);
    if (!dbFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening database file from resources!";
        assert(false);
        return;
    }

    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + db_temp_name;
    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly))
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

void GcodeGenerator::slotHandleData(const QString &data)
{
    QSqlQuery query;
    query.prepare(R"(SELECT * FROM gcode WHERE ch = :ch)");

    QString gcode;
    float centerX = 0.0;
    float centerY = 0.0;
    QStringList strList;
    for (const QChar& c : data)
    {
        query.bindValue(":ch", c);
        // qDebug() << query.lastQuery();
        if (!query.exec())
        {
            qDebug() << "unable to find ch = " << query.lastError().text();
            return;
        }   
        if (query.next())
        {
            gcode = query.value("gcode").toString();
            centerX = query.value("center_x").toFloat();
            centerY = query.value("center_y").toFloat();
            // qDebug() << "gcode: " << gcode << " centerX: " << centerX << " centerY: " << centerY;
        }

//        strList = gcode.split("\n");
//        for (const auto& str : strList)
//        {
//            SPD_INFO("{0}", str.toStdString());

//        }
        assert(m_serialPort != nullptr);
        if (!m_serialPort)
        {
            SPD_ERROR("m_serial is nullptr");
            return;
        }

        m_serialPort->write(gcode);
    }
}
