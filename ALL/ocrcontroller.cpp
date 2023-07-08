#include "ocrcontroller.h"
#include "logger.h"
#include <QFile>

OCRController::OCRController(QObject *parent) : QObject(parent)
{
    init();
}

OCRController::~OCRController() {}

void OCRController::request(const QString &imagePath)
{
    QMetaObject::invokeMethod(this, "_request", Qt::QueuedConnection, Q_ARG(QString, imagePath));
}

void OCRController::init()
{
    QMetaObject::invokeMethod(this, "_init", Qt::QueuedConnection);
}

void OCRController::_request(const QString &imagePath)
{
    if (!m_manager)
    {
        SPD_ERROR("m_manager is nullptr");
    }
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

void OCRController::_init()
{
    m_manager = new QNetworkAccessManager();
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // Temporarily disable peer verification for debugging purposes
    QSslConfiguration::setDefaultConfiguration(sslConfig);
}
