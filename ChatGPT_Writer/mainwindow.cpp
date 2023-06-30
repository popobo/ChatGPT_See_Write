#include "mainwindow.h"
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    layoutInit();
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
    m_sendEdit = new QTextEdit(this);
    m_sendButton = new QPushButton(this);
    m_serialButton = new QPushButton(this);
    m_vboxLayout = new QVBoxLayout(this);
    m_gridLayout = new QGridLayout(this);
    m_funcWidget = new QWidget(this);
    m_mainWidget = new QWidget(this);

    m_sendButton->setText("send data");
    m_sendButton->setMinimumSize(80, 30);
    m_sendButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_sendButton->setEnabled(false);

    m_serialButton->setText("reset serial");
    m_serialButton->setMinimumSize(80, 30);
    m_serialButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_serialButton->setEnabled(false);

    m_gridLayout->addWidget(m_sendButton, 0, 1);
    m_gridLayout->addWidget(m_serialButton, 0, 2);

    /* layout */
    m_vboxLayout->addWidget(m_receivedBrowser, 2);
    m_vboxLayout->addWidget(m_sendEdit, 3);
    m_funcWidget->setLayout(m_gridLayout);
    m_vboxLayout->addWidget(m_funcWidget, 1);
    m_mainWidget->setLayout(m_vboxLayout);

    this->setCentralWidget(m_mainWidget);
}
