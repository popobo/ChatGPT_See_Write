#ifndef OCRCONTROLLER_H
#define OCRCONTROLLER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class OCRController : public QObject
{
    Q_OBJECT
public:
    explicit OCRController(QObject *parent = nullptr);
    ~OCRController();

signals:
    void request(const QString& imagePath);

    void init();

    void response(const QString& req);

private slots:
    void handleRequest(const QString& imagePath);

    void replyFinished(QNetworkReply *reply);

    void slotInit();

private:
    QThread* m_thread;

    QNetworkAccessManager* m_manager = nullptr;
};

#endif // OCRCONTROLLER_H
