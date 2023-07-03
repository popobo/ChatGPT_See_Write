#include <QDebug>
#include "serialport.h"
#include "logger.h"

static const int32_t INTERVAL_TIME = 25;
static const int32_t BUF_SIZE = 128;
static const char* PORT_NAME = "/dev/ttyUSB0";
static const char* SET_OP = "G92 X0 Y0 Z0";
static bool firstTime = true;

SerialPort::SerialPort()
{
    m_serialThread = new QThread();
    connect(m_serialThread, &QThread::finished, m_serialThread, &QObject::deleteLater);
    connect(this, &SerialPort::dataReady, this, &SerialPort::handleData);
    connect(this, &SerialPort::listDataReady, this, &SerialPort::handleListData);
    connect(this, &SerialPort::signalInit, this, &SerialPort::slotInit);
    connect(this, &SerialPort::signalOpen, this, &SerialPort::slotOpen);
    connect(this, &SerialPort::signalClose, this, &SerialPort::slotClose);
    this->moveToThread(m_serialThread);
    m_serialThread->start();

    emit signalInit();
}

void SerialPort::write(const QString& data)
{
    dataReady(data);
}

void SerialPort::write(const QStringList &strList)
{
    listDataReady(strList);
}

void SerialPort::slotInit()
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
    emit signalOpen();
}

void SerialPort::slotOpen()
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
    emit signalOpened(ret);
}

void SerialPort::close()
{
    emit signalClose();
}

void SerialPort::slotClose()
{
    m_serialPort->close();
}

bool SerialPort::isOpen()
{
    return m_serialPort->isOpen();
}

void SerialPort::handleData(const QString& data)
{

    if (firstTime)
    {
        m_serialPort->write(SET_OP);
    }
    QStringList strList = data.split('\n');
    foreach (QString str, strList)
    {
        qDebug() << str;
        str += "\n";
        if (m_serialPort)
        {
            m_serialPort->write(str.toUtf8());
            QThread::msleep(INTERVAL_TIME);
        }
    }
}

void SerialPort::handleListData(const QStringList &strList)
{
    if (firstTime)
    {
        m_serialPort->write(SET_OP);
    }

    foreach (QString str, strList)
    {
        str += "\n";
        if (m_serialPort)
        {
            m_serialPort->write(str.toUtf8());
            SPD_INFO("{0}", str.toStdString());
            if (m_serialPort->waitForReadyRead(1000))
            {
                char buf[BUF_SIZE];
                auto len = m_serialPort->read(buf, BUF_SIZE);
                SPD_INFO("{0}", buf);
            }
            else
            {
                assert(false);
            }
//            m_serialPort->flush();
//            QThread::msleep(INTERVAL_TIME);
        }
    }
}



