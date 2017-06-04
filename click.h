#ifndef CLICK_H
#define CLICK_H

#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QSharedPointer>

class QThreadPool;
class QNetworkAccessManager;
class QNetworkReply;

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
    void get_proxy_list();
    void proxy_reply(QNetworkReply* reply);

private:
    QThreadPool* m_thread_pool;
    QNetworkAccessManager* m_network_mgr;
    QList<QString> m_proxy_list; // host, ip
    QStringList offers;
    QStringList uas;
    QStringList already_click_file;
    qint64 total_click;
    int pool_size;
};

#endif // CLICK_H
