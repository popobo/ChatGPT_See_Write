#ifndef GPTCONTROLLER_H
#define GPTCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class GPTController: public QObject
{
    Q_OBJECT

public:
    GPTController();

signals:
    void request(const QString& req);

    void init();

    void response(const QString& req);

private slots:
    void handleRequest(const QString& req);

    void replyFinished(QNetworkReply *reply);

    void slotInit();

private:
    QThread* m_thread;

    QNetworkAccessManager* m_manager = nullptr;
};

#endif // GPTCONTROLLER_H
