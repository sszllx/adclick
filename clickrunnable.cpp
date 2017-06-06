#include "clickrunnable.h"
#include "click.h"

#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QTimer>

#define test_url "https://global.ymtracking.com/trace?offer_id=5065577&aff_id=104991&idfa=9B0B2561-5BF6-4132-A3E0-80ECDCA1CFAB"

ClickRunnable::ClickRunnable(Click* click) :
    m_click(click)
{

}

ClickRunnable::~ClickRunnable()
{
    // qDebug() << "delete click runnable";
}

void ClickRunnable::run()
{
    HttpHandle http;

    QString p_str = m_click->get_proxy();
    if (p_str.isEmpty()) {
        return;
    }

    QString ua = m_click->get_ua();

//    qDebug() << "proxy: " << p_str;
//    qDebug() << "request url: " << m_url;
//    qDebug() << "\n\n";
    QNetworkProxy proxy;
    QStringList tmp_p = p_str.split(":");
    proxy.setHostName(tmp_p[0]);
    proxy.setPort(tmp_p[1].toInt());
    http.setProxy(proxy);
    http.setUA(ua);
    http.request(QUrl(m_url));
}

HttpHandle::HttpHandle()
{

}

void HttpHandle::request(QUrl url/*, QNetworkAccessManager* mgr*/)
{
    // qDebug() << "request: " << url;
    QEventLoop eventLoop;

    QTimer timer;
    timer.setInterval(500);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    QNetworkAccessManager mgr;
    m_proxy.setType(QNetworkProxy::HttpProxy);
    m_proxy.setUser("lij80");
    m_proxy.setPassword("noh67wef");
    mgr.setProxy(m_proxy);
    QNetworkRequest qnr(url);
    qnr.setHeader(QNetworkRequest::UserAgentHeader, m_ua);
//    qDebug() << "proxy: " << m_proxy;
    QNetworkReply* reply = mgr.get(qnr);
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    timer.start();
    eventLoop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        // qDebug() << "err url:" << reply->url();
        qDebug() << "error:" << reply->error() << "reply error: " << reply->errorString();
        reply->close();
        reply->deleteLater();
        // delete mgr;
        return;
    }

    int http_status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // qDebug() << "http:" << http_status;

    reply->close();
    reply->deleteLater();

    if (http_status == 302 ||
            http_status == 301) {
        QVariant possibleRedirectUrl =
                reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        // qDebug() << possibleRedirectUrl.toString();
        request(QUrl(possibleRedirectUrl.toString())/*, mgr*/);
    }
}
