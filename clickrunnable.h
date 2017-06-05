#ifndef CLICKRUNNABLE_H
#define CLICKRUNNABLE_H

#include <QNetworkProxy>
#include <QObject>
#include <QRunnable>
#include <QUrl>

class Click;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;

class HttpHandle : public QObject
{
    Q_OBJECT
public:
    HttpHandle();

    void request();
    void setProxy(QNetworkProxy proxy) { m_proxy = proxy; }
    void setUA(QString ua) { m_ua = ua; }

signals:
    void exit();

public slots:
    void request(QUrl url/*, QNetworkAccessManager* mgr = NULL*/);

private:
    QNetworkProxy m_proxy;
    QString m_ua;
};

class ClickRunnable : public QRunnable
{
public:
    ClickRunnable(Click* click);
    ~ClickRunnable();

    void setUrl(QString url) { m_url = url; }

    void run() Q_DECL_OVERRIDE;

private:
    Click* m_click;
    QString m_url;
};

#endif // CLICKRUNNABLE_H
