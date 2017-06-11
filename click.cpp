#include "click.h"
#include "clickrunnable.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThreadPool>
#include <QTime>
#include <QTimer>

#define MAX_THREAD_NUM 200

ProxyThread::ProxyThread(Click *click) : m_click(click) {}

void ProxyThread::run()
{
    qDebug() << "Request threadid: " << QThread::currentThreadId();

    QUrl url("http://www.httpsdaili.com/api.asp?key=20170610114439653&getnum=10&anonymoustype=3&area=1&splits=%7C");
    m_network_mgr = new QNetworkAccessManager;

    while (1) {
        QEventLoop eventLoop;
        QNetworkReply* reply = m_network_mgr->get(QNetworkRequest(url));
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        QString data = reply->readAll();
        qDebug() << data;
        if (data.size() == 0 ||
                data.contains("error", Qt::CaseInsensitive)) {
            QTimer::singleShot(1000*2, this, SLOT(get_proxy_list()));
            return;
        }

        QStringList tmplist;
        QStringList proxies = data.split("|");
        foreach(QString p, proxies) {
            tmplist << p;
        }

        m_click->set_proxy_list(tmplist);

        QThread::sleep(5);
    }
}

// QThread::idealThreadCount()
Click::Click(QObject *parent) : QObject(parent),
    m_thread_pool(QThreadPool::globalInstance()),
//    m_network_mgr(new QNetworkAccessManager(this)),
    total_click(0),
    pool_size(MAX_THREAD_NUM)
//    bf(new BloomFilter)
{
    qDebug() << "thread ideal count:" << pool_size;
    m_thread_pool->setMaxThreadCount(pool_size);

    QFile offer_file("offers.txt");
    if (!offer_file.open(QIODevice::ReadOnly)) {
        offers << "https://global.ymtracking.com/trace?offer_id=5107479&aff_id=104991";
        offers << "https://global.ymtracking.com/trace?offer_id=5065577&aff_id=104991";
        offers << "http://svr.dotinapp.com/ics?sid=1217&adid=4006512";
    } else {
        while (!offer_file.atEnd()) {
            QString f = offer_file.readLine().trimmed();
            qDebug() << "==========" << f;
            offers << f;
        }
    }

    QFile ua_file("ua.txt");
    if (!ua_file.open(QIODevice::ReadOnly)) {
        uas << "Mozilla/5.0 (iPad; CPU OS 7_0 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Version/7.0 Mobile/11A465 Safari/9537.53";
        uas << "Mozilla/5.0 (iPhone; CPU iPhone OS 6_1_4 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10B350 Safari/8536.25";
        uas << "Mozilla/5.0 (iPad; CPU OS 5_1_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9B206 Safari/7534.48.3";
        uas << "Mozilla/5.0 (iPhone; CPU iPhone OS 7_0 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/11A465 Twitter for iPhone";
    } else {
        while (!ua_file.atEnd()) {
            QString f = ua_file.readLine().trimmed();
            uas << f;
        }
    }

#if 0
    QFile file("already_read_id_file.txt");
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            while (!file.atEnd()) {
                already_click_file << file.readLine().trimmed();
            }
        }
    }
#endif

//    connect(m_network_mgr, SIGNAL(finished(QNetworkReply*)),
//            this, SLOT(proxy_reply(QNetworkReply*)));
//    get_proxy_list();

//    pt = new ProxyThread(this);
//    pt->start();
}

QString Click::proxyRequest()
{
    qDebug() << "proxyRequest";
    QEventLoop eventLoop;
    // QUrl url("http://dps.kuaidaili.com/api/getdps/?orderid=929666638416410&num=50&sep=4");
    QUrl url("http://www.xdaili.cn/ipagent/privateProxy/applyStaticProxy?count=1&spiderId=eae82458fb354e628f25f5ee7b874291&returnType=1");
    // QUrl url("http://www.httpsdaili.com/api.asp?key=20170610114439653&getnum=1000&area=1&splits=%7C&proxytype=1");
    QNetworkAccessManager mgr;
    QNetworkRequest qnr(url);
    QNetworkReply* reply = mgr.get(qnr);
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "error:" << reply->error() << "reply error: " << reply->errorString();
        reply->close();
        reply->deleteLater();
        return QString("");
    }

    QString data = reply->readAll().trimmed();
    return data;
}

QString Click::getProxy()
{
#if 0
    QTime time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);
    int rand = qrand();
    return m_proxy_list.at(rand % m_proxy_list.size());
#endif

    if (m_proxy_list.size() == 0) {
        QString data = proxyRequest();
        while (data.contains("ERROR") ||
               data.size() < 100) {
            qDebug() << "get proxy error!";
            return "";
        }

        QStringList proxies = data.split("\n");
        if (proxies.size() == 1 ||
                proxies.size() == 0) {
            return "";
        }

        foreach(QString p, proxies) {
            m_proxy_list << p;
        }
    }

    if (m_proxy_list.size() == 0) {
        return "";
    }
    return m_proxy_list.takeFirst();
}

QString Click::get_ua()
{
    QTime time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);
    int rand = qrand();
    return uas.at(rand % uas.size());
}

void Click::start_request()
{
    qDebug() << "in start_request thread id:" << QThread::currentThreadId();
#if 0
    if (m_proxy_list.size() == 0) {
        // retry
        QTimer::singleShot(1000, this, SLOT(start_request()));
        return;
    }
#endif

    qDebug() << "start request";

    // qDebug() << "dir: " << QCoreApplication::applicationDirPath();
    QDir dir(QCoreApplication::applicationDirPath() + "/ioslogs/");
    QStringList files_type;
    files_type << "*.id";
    QFileInfoList file_list = dir.entryInfoList(files_type, QDir::Files);
    if (file_list.size() == 0) {
        qDebug() << "no id files";
    }
    foreach (QFileInfo fi, file_list) {
        // qDebug() << fi.absoluteFilePath();
        if (already_click_file.contains(fi.absoluteFilePath())) {
            continue;
        }
        already_click_file << fi.absoluteFilePath();
        QFile id_file(fi.absoluteFilePath());
        if (!id_file.open(QIODevice::ReadOnly)) {
            continue;
        }

        while (!id_file.atEnd()) {
            QString idfa = id_file.readLine().trimmed();
            QString proxy = getProxy();
            while (proxy.size() == 0 ||
                   used_ip_list.contains(proxy)) {
                qDebug() << "duplicate " << used_ip_list.size()
                         << "proxy size: " << proxy.size();
                QThread::sleep(1);
                proxy = getProxy();
            }

            used_ip_list << proxy;

            foreach(QString offer, offers) {
                // QString url = offer + "&idfa=" + idfa;
                QString url("http://www.baidu.com");
                ClickRunnable* click = new ClickRunnable(this);
                click->setUrl(url);
                click->setAutoDelete(true);

                while (m_thread_pool->activeThreadCount() == pool_size) {
                    QThread::sleep(1);
                }

                click->setProxy(proxy);

                total_click++;
                qDebug() << "total click: " << total_click;
                QThread::msleep(10);
                m_thread_pool->start(click);
            }
        }
    }
}

void Click::set_proxy_list(QStringList pl)
{
    m_proxy_list.clear();
    m_proxy_list = pl;
}

#if 0
void Click::get_proxy_list()
{
    qDebug() << "get proxy list";
    QUrl url("http://dps.kuaidaili.com/api/getdps/?orderid=929666638416410&num=100&sep=4");
    m_network_mgr->get(QNetworkRequest(url));
}

void Click::proxy_reply(QNetworkReply *reply)
{
    qDebug() << "main thread id:" << QThread::currentThreadId();
    QString data = reply->readAll();
    qDebug() << data;
    if (data.size() == 0 ||
            data.contains("error", Qt::CaseInsensitive)) {
        QTimer::singleShot(1000*2, this, SLOT(get_proxy_list()));
        return;
    }

    // TODO: thread safe?
    m_proxy_list.clear();
    QStringList proxies = data.split("|");
    foreach(QString p, proxies) {
        m_proxy_list << p;
    }

    qDebug() << "proxy size:" << m_proxy_list.size();

    QTimer::singleShot(1000*5, this, SLOT(get_proxy_list()));
}
#endif
