#pragma once

#ifndef VERBOSE
    #define VERBOSE true
#endif

#ifndef SHOWCALL
    #define SHOWCALL true
#endif

#define DEBUG(level, x)    if (VERBOSE) qDebug().noquote() << QObject::tr("[%1]").arg(level) << x;

#define ERROR(X) DEBUG("ERROR", x)
#define WARNING(x)  DEBUG("WARNING", x)
#define INFO(x)     DEBUG("INFO", x)

#if SHOWCALL
    #define CALL(func)  DEBUG("CALL", func)
#endif

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

void httpGET(const QString& strUrl, QString& response, int& statusCode)
//没用到
{
    CALL("void httpGET(const QString& strUrl, QString& response, int& statusCode)");

    assert(!strUrl.isEmpty());
    const QUrl url = QUrl::fromUserInput(strUrl);
    assert(url.isValid());

    //
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QScopedPointer<QNetworkReply> reply(manager->get(request));

    //wait.
    QEventLoop eventLoop;
    QTimer::singleShot(5000, &eventLoop, [&]() { if (eventLoop.isRunning()) { eventLoop.quit(); } });
    QObject::connect(reply.data(), &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    //status code
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //response
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        response = QString::fromLocal8Bit(data);
    }
    else
    {
        response = reply->errorString().toUtf8();
    }
    reply->deleteLater();
}

void packetByte (int start, int end, int &value, QByteArray &qByteArray)
{
    CALL("void packetByte (int start, int end, int &value, QByteArray &qByteArray)");

    if (qByteArray.size() < end + 1)
    {
        qDebug() << "Error: ";
        return;
    }
    int pow = end - start;
    int res = value;
    for (int i = start; i <= end; i++)
    {
        int temp = res / std::pow(256, pow);
        res -= temp > 0 ? std::pow(256, pow)*temp : 0;
        qByteArray[i] = temp;
        pow--;
    }

    return;
}

void packetByte (int start, int end, qint16 &value, QByteArray &qByteArray)
{
    CALL("void packetByte (int start, int end, qint16 &value, QByteArray &qByteArray)");

    if (qByteArray.size() < end + 1)
    {
        qDebug() << "Error: ";
        return;
    }
    int pow = end - start;
    int res = value;
    for (int i = start; i <= end; i++)
    {
        int temp = res / std::pow(256, pow);
        res -= temp > 0 ? std::pow(256, pow)*temp : 0;
        qByteArray[i] = temp;
        pow--;
    }

    return;
}

void printByte (QByteArray const data, int start, int length)
{
    CALL("void printByte (QByteArray const data, int start, int length)");

    for (int i = start; i < start + length - 1; i++) {
        qDebug() << "data[" << i << "] = " << quint8(data[i]); // QT按字节（quint8)输出
    }
}
void printByte (QByteArray const data)
{
    CALL("void printByte (QByteArray const data)");
    for (int i = 0; i < data.size(); i++) {
        qDebug() << "data[" << i << "] = " << quint8(data[i]); // QT按字节（quint8)输出
    }
}

void changeHeaderSize (QByteArray *data, int num, bool plus)
{
    CALL("void changeHeaderSize (QByteArray *data, int num, bool plus)");

    QByteArray lenData = data->mid(0, 4);
    quint32 len;
    QDataStream read(&lenData, QIODevice::ReadOnly);
    read >> len;
    QDataStream write(&lenData, QIODevice::WriteOnly);
    write << quint32(len + (plus?num:-num));
    data->replace(0, 4, lenData);
}
