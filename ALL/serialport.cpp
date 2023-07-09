#include <QDebug>
#include "serialport.h"
#include "logger.h"
#include <cstring>

static const int32_t BUF_SIZE = 128;
static const char* PORT_NAME = "/dev/ttyUSB0";
static const char* SET_OP = "G92 X0 Y0 Z0";
static const char* OK = "ok\r\n";
static const int32_t MAX_RETRY_TIMES = 10;
static const int32_t WAIT_FOR_READ = 1000;

static bool firstTime = true;

SerialPort::SerialPort()
{
    m_serialThread.start();
    this->moveToThread(&m_serialThread);
    init();
}

SerialPort::~SerialPort()
{
    m_serialThread.quit();
    m_serialThread.wait();
}

void SerialPort::init()
{
    QMetaObject::invokeMethod(this, "_init", Qt::QueuedConnection);
}

void SerialPort::write(const QString& data)
{
    QMetaObject::invokeMethod(this, "_handleData", Qt::QueuedConnection, Q_ARG(QString, data));
}

void SerialPort::write(const QStringList &strList)
{
    QMetaObject::invokeMethod(this, "_handleListData", Qt::QueuedConnection, Q_ARG(QStringList, strList));
}

void SerialPort::_init()
{
    m_serialPort.reset(new QSerialPort());

    m_serialPort->setPortName(PORT_NAME);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}

void SerialPort::open()
{
    QMetaObject::invokeMethod(this, "_open", Qt::QueuedConnection);
}

void SerialPort::_open()
{
    bool ret;
    if (m_serialPort->isOpen())
    {
        ret = true;
    }
    else
    {
        ret = m_serialPort->open(QIODevice::ReadWrite);
    }
    emit opened(ret);
}

void SerialPort::close()
{
    QMetaObject::invokeMethod(this, "_close", Qt::QueuedConnection);
}

void SerialPort::_close()
{
    m_serialPort->close();
}

bool SerialPort::isOpen()
{
    return m_serialPort->isOpen();
}

void SerialPort::_handleData(const QString& data)
{

    if (firstTime)
    {
        m_serialPort->write(SET_OP);
    }
    QStringList strList = data.split('\n');
    foreach (QString str, strList)
    {
        str += "\n";
        int32_t retryTimes = 0;
        if (m_serialPort)
        {
            retry:
            m_serialPort->write(str.toUtf8());
            SPD_INFO("{0}", str.toStdString());
            if (m_serialPort->waitForReadyRead(WAIT_FOR_READ))
            {
                char buf[BUF_SIZE];
                auto len = m_serialPort->read(buf, BUF_SIZE);
                buf[len] = '\0';
                SPD_INFO("{0}", buf);
                if (strcmp(buf, OK) != 0)
                {
                    retryTimes++;
                    if (retryTimes < MAX_RETRY_TIMES)
                    {
                        goto retry;
                    }
                }
            }
            else
            {
                assert(false);
            }
        }
    }
}

void SerialPort::_handleListData(const QStringList &strList)
{
    if (firstTime)
    {
        m_serialPort->write(SET_OP);
    }

    foreach (QString str, strList)
    {
        str += "\n";
        int32_t retryTimes = 0;
        if (m_serialPort)
        {
            retry:
            m_serialPort->write(str.toUtf8());
            SPD_INFO("{0}", str.toStdString());
            if (m_serialPort->waitForReadyRead(WAIT_FOR_READ))
            {
                char buf[BUF_SIZE];
                auto len = m_serialPort->read(buf, BUF_SIZE);
                buf[len] = '\0';
                SPD_INFO("{0}", buf);
                if (strcmp(buf, OK) != 0)
                {
                    retryTimes++;
                    if (retryTimes < MAX_RETRY_TIMES)
                    {
                        goto retry;
                    }
                }
            }
            else
            {
                assert(false);
            }
        }
    }
}



