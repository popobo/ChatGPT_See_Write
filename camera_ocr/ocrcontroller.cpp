#include "ocrcontroller.h"
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QNetworkProxy>

OCRController::OCRController(QObject *parent) : QObject(parent),
                                                m_thread(new QThread())
{
    m_thread->start();

    this->moveToThread(m_thread);

    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    connect(m_thread, &QThread::finished, this, &OCRController::deleteLater);
    connect(this, &OCRController::request, this, &OCRController::handleRequest);

    connect(this, &OCRController::init, this, &OCRController::slotInit);
    init();
}

OCRController::~OCRController()
{
    delete m_manager;
    m_manager = nullptr;
}

void OCRController::handleRequest(const QString &imagePath)
{
    qDebug() << __FILE__ << __LINE__;
    if (!m_manager)
        return;
    QNetworkRequest request;
    QUrl url("https://aip.baidubce.com/rest/2.0/ocr/v1/handwriting?access_token=24.eb81d508cbd45df6535c612433c2f5bc.2592000.1691303300.282335-35827675");
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    // 构建请求体参数
    QByteArray postData;
    QFile imageFile(imagePath);
    if (imageFile.open(QIODevice::ReadOnly))
    {
        QByteArray imageData = imageFile.readAll();
        postData.append("image=" + QUrl::toPercentEncoding(imageData.toBase64()));
        imageFile.close();
    }
    else
    {
        qDebug() << "Failed to open image file. " << imagePath;
    }

    QNetworkReply *reply = m_manager->post(request, postData);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 请求成功，处理响应数据
            QByteArray response = reply->readAll();
            QString responseStr(response);
            qDebug() << "Response:" << responseStr;
        } else {
            // 请求出错，处理错误信息
            qDebug() << "Error:" << reply->errorString();
        }

        // 清理资源
        reply->deleteLater();
    });

}

void OCRController::replyFinished(QNetworkReply *reply)
{
    qDebug() << QSslSocket::sslLibraryBuildVersionString();
    if (reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        auto data = reply->readAll();
        qDebug() << data;
    }
    delete reply;
}

void OCRController::slotInit()
{
    m_manager = new QNetworkAccessManager();

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // Temporarily disable peer verification for debugging purposes
    QSslConfiguration::setDefaultConfiguration(sslConfig);
}
