#include "gcodegenerator.h"
#include "logger.h"
#include "serialport.h"

static const char* DB_RESOURCE_PATH = ":/gcode_database/gcode.sqlite";
static const char* DB_TEMP_NAME = "/gcode.sqlite";
static int32_t MAX_CH_A_LINE = 16;
static float DIS_BT_CHS = 7;
static float DIS_BT_LINE = 7;
static float DEFAULT_MIN_Z = 0.5;
static float DEFAULT_MAX_Z = 7.0;
static float NEW_MIN_Z = -8.0;
static float NEW_MAX_Z = 0.5;
static float STANDARD_CENTER_X = 2.0;
static float STANDARD_CENTER_Y = -2.0;

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
    QFile dbFile(DB_RESOURCE_PATH);
    if (!dbFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening database file from resources!";
        assert(false);
        return;
    }

    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + DB_TEMP_NAME;
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
    QRegularExpression reXY("G0 X([-+]?[0-9]*\\.?[0-9]+)Y([-+]?[0-9]*\\.?[0-9]+)");
    QRegularExpression reZ("G1G90 Z([-+]?[0-9]*\\.?[0-9]+)F8000");
    for (const QChar& c : data)
    {
        /* get gcode of c from db */
        QString gcode;
        float centerX = 0.0;
        float centerY = 0.0;
        query.bindValue(":ch", c);
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
        }

        /* modify x, y, z and generate new gcode */
        QStringList strList = gcode.split("\n");
        QStringList newGcodeList;

        float fixX = STANDARD_CENTER_X - centerX;
        float fixY = STANDARD_CENTER_Y - centerY;

        for (const auto& str : strList)
        {
            auto matchXY = reXY.match(str);
            QString newStr;
            if (matchXY.hasMatch())
            {
                float x = matchXY.captured(1).toFloat();
                float y = matchXY.captured(2).toFloat();
                x = x + fixX + DIS_BT_CHS * m_chLineIndex;
                y = y + fixY - DIS_BT_LINE * m_lineIndex;

                newStr.sprintf("G0 X%.3fY%.3fF8000", x, y);
                newGcodeList.append(newStr);
            }
            auto matchZ = reZ.match(str);
            if (matchZ.hasMatch())
            {
                float z = matchZ.captured(1).toFloat();
                if (z == DEFAULT_MIN_Z)
                {
                    z = NEW_MIN_Z;
                }
                else if (z == DEFAULT_MAX_Z)
                {
                    z = NEW_MAX_Z;
                }
                newStr.sprintf("G1G90 Z%.3fF8000", z);
                newGcodeList.append(newStr);
            }
            // SPD_INFO("newStr: {0}", newStr.toStdString());
        }

        m_serialPort->write(newGcodeList);

        m_chLineIndex++;
        if (m_chLineIndex >= MAX_CH_A_LINE)
        {
            m_lineIndex++;
            m_chLineIndex = 0;
        }
    }
}
