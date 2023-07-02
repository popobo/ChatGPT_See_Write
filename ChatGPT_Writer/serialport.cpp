#include "serialport.h"
#include <QDebug>

static const int32_t INTERVAL_TIME = 25;
static const char* PORT_NAME = "/dev/ttyUSB0";

SerialPort::SerialPort()
{
    m_serialPort.reset(new QSerialPort());

    m_serialPort->setPortName(PORT_NAME);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    m_serialThread = new QThread();
    connect(m_serialThread, &QThread::finished, m_serialThread, &QObject::deleteLater);
    connect(this, &SerialPort::dataReady, this, &SerialPort::handleData);
    this->moveToThread(m_serialThread);
    m_serialThread->start();
}

void SerialPort::write(QString data)
{
    dataReady(data);
}

bool SerialPort::open()
{
    if (m_serialPort->isOpen())
        return true;
    return m_serialPort->open(QIODevice::ReadWrite);
}

void SerialPort::close()
{
    m_serialPort->close();
}

bool SerialPort::isOpen()
{
    return m_serialPort->isOpen();
}

void SerialPort::handleData(QString data)
{
    QStringList strList = data.split('\n');
    foreach (QString str, strList) {
        qDebug() << str;
        str += "\n";
        if (m_serialPort)
        {
            m_serialPort->write(str.toUtf8());
            QThread::msleep(INTERVAL_TIME);
        }
    }
}



