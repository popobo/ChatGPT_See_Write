#include "gptcontroller.h"
#include "logger.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkProxy>

static const char* KEY = "Bearer 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";

GPTController::GPTController(QObject *parent) : QObject(parent)
{
    init();
}

GPTController::~GPTController() {}

void GPTController::request(const QString &imagePath)
{
    QMetaObject::invokeMethod(this, "_request", Qt::QueuedConnection, Q_ARG(QString, imagePath));
}

void GPTController::init()
{
    QMetaObject::invokeMethod(this, "_init", Qt::QueuedConnection);
}

QString GPTController::parseResponse(const QByteArray &byteArray)
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
    if (!responseObject.contains("choices")) {
        qDebug() << "Response does not contain the 'words_result' field.";
        return result;
    }
    QJsonArray resultArray = responseObject.value("choices").toArray();
    if (resultArray.isEmpty())
    {
        qDebug() << "Choices array is empty.";
        return result;
    }
    for (const auto& val: resultArray)
    {
        auto obj = val.toObject();
        if (obj.contains("text"))
        {
            result += obj.value("text").toString();
        }
    }
    return result;
}

void GPTController::_request(const QString &req)
{
    if (!m_manager)
    {
        SPD_ERROR("m_manager is nullptr");
        return;
    }

    if (req.isEmpty())
    {
        SPD_ERROR("req is empty");
        return;
    }

    SPD_INFO("req {0}", req.toStdString());
    QNetworkRequest request;
    QUrl url("https://api.openai.com/v1/completions");
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", KEY);

    QJsonObject requestBody;
    requestBody["model"] = "text-davinci-003";
    requestBody["prompt"] = req;
    requestBody["max_tokens"] = 100;
    requestBody["temperature"] = 0;

    QJsonDocument requestDoc(requestBody);

    QNetworkReply *reply = m_manager->post(request, requestDoc.toJson());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                QString text = parseResponse(response);
                this->response(text);
        } else {
            qDebug() << "Request error:" << reply->errorString();
        }

        reply->deleteLater();
    });
}

void GPTController::_init()
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
