#include "gptcontroller.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkProxy>
#include <QJsonArray>

static const char* KEY = "Bearer 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";

GPTController::GPTController(): m_thread(new QThread())
{
    m_thread->start();
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    connect(m_thread, &QThread::finished, this, &GPTController::deleteLater);
    connect(this, &GPTController::request, this, &GPTController::handleRequest);

    connect(this, &GPTController::init, this, &GPTController::slotInit);
    init();
}

void GPTController::handleRequest(const QString &req)
{
    if (!m_manager)
        return;
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

                // Transform the response into a JSON object
                QJsonDocument responseDoc = QJsonDocument::fromJson(response);
                if (responseDoc.isNull())
                {
                    qDebug() << "Failed to parse response as JSON.";
                    return;
                }
                if (!responseDoc.isObject())
                {
                    qDebug() << "Response is not a JSON object.";
                    return;
                }
                QJsonObject responseObject = responseDoc.object();
                if (!responseObject.contains("choices")) {
                    qDebug() << "Response does not contain the 'choices' field.";
                    return;
                }
                QJsonArray choicesArray = responseObject.value("choices").toArray();
                if (choicesArray.isEmpty())
                {
                    qDebug() << "Choices array is empty.";
                    return;
                }
                QJsonObject choiceObject = choicesArray.at(0).toObject();
                if (!choiceObject.contains("text"))
                {
                    qDebug() << "Response does not contain the 'text' field.";
                    return;
                }

                QString text = choiceObject.value("text").toString();
                qDebug() << "Text:" << text;
                this->response(text);
        } else {
            qDebug() << "Request error:" << reply->errorString();
        }

        reply->deleteLater();
    });
}

void GPTController::replyFinished(QNetworkReply *reply)
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

void GPTController::slotInit()
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
