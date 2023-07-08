#include "ocrcontroller.h"
#include "logger.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkProxy>

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

QString OCRController::parseResponse(const QByteArray &byteArray)
{
    QString result;
    QJsonDocument responseDoc = QJsonDocument::fromJson(byteArray);
    if (responseDoc.isNull())
    {
        qDebug() << "Failed to parse response as JSON.";
        return result;
    }
    if (!responseDoc.isObject())
    {
        qDebug() << "Response is not a JSON object.";
        return result;
    }
    QJsonObject responseObject = responseDoc.object();
    if (!responseObject.contains("words_result")) {
        qDebug() << "Response does not contain the 'words_result' field.";
        return result;
    }
    QJsonArray resultArray = responseObject.value("words_result").toArray();
    if (resultArray.isEmpty())
    {
        qDebug() << "words_result array is empty.";
        return result;
    }
    for (const auto& val: resultArray)
    {
        auto obj = val.toObject();
        if (obj.contains("words"))
        {
            result += obj.value("words").toString();
        }
    }
    return result;
}

void OCRController::_request(const QString &imagePath)
{
    if (!m_manager)
    {
        SPD_ERROR("m_manager is nullptr");
        return;
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
            // Transform the response into a JSON object
            QString result = parseResponse(response);
            emit this->response(result);
        } else {
            // 请求出错，处理错误信息
            qDebug() << "Error:" << reply->errorString();
        }

        reply->deleteLater();
    });
}

void OCRController::_init()
{
    m_manager = new QNetworkAccessManager();

#if __arm__
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("127.0.0.1");
    proxy.setPort(7890);
    m_manager->setProxy(proxy);
#endif

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // Temporarily disable peer verification for debugging purposes
    QSslConfiguration::setDefaultConfiguration(sslConfig);
}
