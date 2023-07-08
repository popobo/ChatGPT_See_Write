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

    void request(const QString& imagePath);

signals:
    void response(const QString& req);

private:
    void init();
    QString parseResponse(const QByteArray& byteArray);

private slots:
    void _request(const QString& imagePath);
    void _init();

private:
    QNetworkAccessManager* m_manager = nullptr;
};

#endif // OCRCONTROLLER_H
