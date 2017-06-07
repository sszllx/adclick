#ifndef CLICK_H
#define CLICK_H

#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QSharedPointer>
#include <QtCore>

class QThreadPool;
class QNetworkAccessManager;
class QNetworkReply;

class Click;

class ProxyThread : public QThread
{
public:
    ProxyThread(Click* click);

    void run();

private:
    Click* m_click;

    QNetworkAccessManager* m_network_mgr;
};

class Click : public QObject
{
    Q_OBJECT
public:
    explicit Click(QObject *parent = nullptr);

    QString get_proxy();
    QString get_ua();

signals:

public slots:
    void start_request();
    // void get_proxy_list();
    void set_proxy_list(QStringList pl);
    // void proxy_reply(QNetworkReply* reply);

private:
    QThreadPool* m_thread_pool;

    QList<QString> m_proxy_list; // host, ip
    QStringList offers;
    QStringList uas;
    QStringList already_click_file;
    qint64 total_click;
    int pool_size;

    ProxyThread* pt;
};

#endif // CLICK_H
