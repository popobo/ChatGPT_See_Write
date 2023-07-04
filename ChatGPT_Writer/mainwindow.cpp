#include "mainwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include "logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    layoutInit();

    moduleInit();

    connect(m_serialButton, &QPushButton::clicked, this, &MainWindow::openCloseSerial);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::sendData);
    connect(m_serialPort.get(), &SerialPort::signalOpened, this, &MainWindow::serialOpened);
}

MainWindow::~MainWindow()
{
}

void MainWindow::layoutInit()
{
    QList<QScreen*> listScreens = QGuiApplication::screens();

#if __arm__
    this->resize(listScreens.at(0)->geometry().width(),
                 listScreens.at(0)->geometry().height());
#else
    this->resize(800, 480);
#endif

    m_receivedBrowser = new QTextBrowser(this);
    m_sendEdit = new QTextEdit();
    m_sendButton = new QPushButton();
    m_serialButton = new QPushButton();
    m_vboxLayout = new QVBoxLayout();
    m_gridLayout = new QGridLayout();
    m_funcWidget = new QWidget();
    m_mainWidget = new QWidget();

    m_sendButton->setText("send data");
    m_sendButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // m_sendButton->setEnabled(false);

    m_serialButton->setText("serial not opened");
    m_serialButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_gridLayout->addWidget(m_sendButton, 0, 1);
    m_gridLayout->addWidget(m_serialButton, 0, 2);

    /* layout */
    m_vboxLayout->addWidget(m_receivedBrowser, 2);
    m_vboxLayout->addWidget(m_sendEdit, 3);
    m_funcWidget->setLayout(m_gridLayout);
    m_vboxLayout->addWidget(m_funcWidget, 1);
    m_mainWidget->setLayout(m_vboxLayout);

    // m_sendEdit->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    m_sendEdit->setText("啊阿埃挨哎唉哀皑癌蔼矮艾碍爱隘鞍氨安俺按暗岸胺案肮昂盎凹敖熬翱袄傲奥懊");

    this->setCentralWidget(m_mainWidget);
}

void MainWindow::moduleInit()
{
    m_serialPort.reset(new SerialPort());
    m_gcodeGenerator.reset(new GcodeGenerator());
    m_gcodeGenerator->setSerialPort(m_serialPort);
    Logger::init();
}

void MainWindow::openCloseSerial()
{
    if (!m_serialPort->isOpen())
    {
        m_serialPort->open();
    }
    else
    {
        m_serialPort->close();
        m_serialButton->setText("serial not opened");
        m_sendButton->setEnabled(false);
    }
}

void MainWindow::sendData()
{
    if (!m_gcodeGenerator->isDbOpen())
    {
        qDebug() << "gcodeGenerator is not ready";
    }
    m_gcodeGenerator->sendData(m_sendEdit->toPlainText());
}

void MainWindow::serialOpened(bool ret)
{
    if (ret)
    {
        m_serialButton->setEnabled(true);
        m_serialButton->setText("serial opened");
        m_sendButton->setEnabled(true);
    }
    else
    {
        m_serialButton->setText("serial error, may be ocupied");
    }
}
