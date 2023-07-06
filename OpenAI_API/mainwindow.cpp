#include <QGuiApplication>
#include <QScreen>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gptcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    layoutInit();

    moduleInit();
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

    m_sendButton->setText("send");
    m_sendButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // m_sendButton->setEnabled(false);

    m_serialButton->setText("clear");
    m_serialButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_gridLayout->addWidget(m_sendButton, 0, 1);
    m_gridLayout->addWidget(m_serialButton, 0, 2);

    /* layout */
    m_vboxLayout->addWidget(m_receivedBrowser, 3);
    m_vboxLayout->addWidget(m_sendEdit, 2);
    m_funcWidget->setLayout(m_gridLayout);
    m_vboxLayout->addWidget(m_funcWidget, 1);
    m_mainWidget->setLayout(m_vboxLayout);

    this->setCentralWidget(m_mainWidget);

    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::sendData);
}

void MainWindow::moduleInit()
{
    m_gptController = new GPTController();
    connect(m_gptController, &GPTController::response, this, &MainWindow::getResponse);
}

void MainWindow::sendData()
{
    QString data = m_sendEdit->toPlainText();
    m_gptController->request(data);
}

void MainWindow::getResponse(const QString &str)
{
    m_receivedBrowser->setText(str);
}
