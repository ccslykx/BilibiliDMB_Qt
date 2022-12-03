/**
 * @file Bilibili.cpp
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <QByteArray>
#include <QDateTime>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <Core/Bilibili.h>
#include <Core/utils.h>

namespace DanmuCore 
{

Bilibili::Bilibili() 
{
    qDebug() << "Bilibili Constructed!";
}

Bilibili::~Bilibili() 
{
    qDebug() << "Bilibili Deconstructed!";
};

void Bilibili::connect(QString roomid)
{
    qDebug() << "void Bilibili::connect(QString roomid)";
    getRealRoomID(roomid);
    getToken(roomid);
    
    m_socket = new QWebSocket;
    QObject::connect(m_socket, &QWebSocket::connected, 
                     this, &Bilibili::onConnected);
    QNetworkRequest request;
    request.setUrl(QUrl("wss://broadcastlv.chat.bilibili.com:443/sub"));
    m_socket->open(request);
}

void Bilibili::disconnect()
{
    qDebug() << "void Bilibili::disconnect()";
    if (m_heartbeatTimer->isActive())
    {
        m_heartbeatTimer->stop();
    }
    m_socket->close();

    m_heartbeatTimer->deleteLater();
    m_socket->deleteLater();
}

void Bilibili::onReceive(QByteArray data)
{
    quint16 version = data.mid(6, 2).toHex().toUShort(); // mid()从中间截取一段数据
    // qDebug() << "void Bilibili::onReceive(QByteArray data) version = " << version;

    data.remove(4, 12); //结合下面，少个插入操作
    //参考https://blog.csdn.net/doujianyoutiao/article/details/106236207
    changeHeaderSize(&data, 12, false); //因为上一步删除了12字节数据，所以调整Header中对应的大小

    //switch中使用的变量
    QJsonDocument jsonDocument;
    QJsonObject jsonObj;
    QByteArray unCompressedData;

    switch (version) {
    case 0: // JSON
        jsonDocument = QJsonDocument::fromJson(data);
        if (!jsonDocument.isObject())
        {
            return;
        }
        jsonObj = jsonDocument.object();
        parse(jsonObj);
        break;

    case 1: // 人气
        jsonDocument = QJsonDocument::fromJson(data);
        break;

    case 2: // zlib JSON
        unCompressedData = qUncompress(data);
        unpacket(unCompressedData);
        break;

    case 3: // brotli JSON
        break;

    default:
        jsonDocument = QJsonDocument::fromJson(data);
        if (!jsonDocument.isObject())
        {
            return;
        }
        jsonObj = jsonDocument.object();
        break;
    }
}

void Bilibili::onConnected()
{
    qDebug() << "void Bilibili::onConnected()";
    QObject::connect(m_socket, &QWebSocket::binaryMessageReceived,
                     this, &Bilibili::onReceive);
    m_socket->sendBinaryMessage(packet(7));
    m_socket->sendBinaryMessage(packet(2));

    // if no error
        m_heartbeatTimer = new QTimer(this);
        QObject::connect(m_heartbeatTimer, &QTimer::timeout, this, [this](){
            this->m_socket->sendBinaryMessage(packet(2));
        });
        m_heartbeatTimer->start(30000);
    // end if
}

QByteArray Bilibili::packet(int op) 
{
    QJsonObject jsonObj;
    switch (op)
    {
    case 7:
        jsonObj.insert("uid", 0);
        jsonObj.insert("roomid", m_realRoomID);
        jsonObj.insert("protover", 2); // 对应onReceive中version
        jsonObj.insert("platform", "web");
        jsonObj.insert("type", 2);
        jsonObj.insert("clientver", "1.14.3");
        jsonObj.insert("key", m_token);
        break;
    
    default:
        break;
    }

    QByteArray body = QJsonDocument(jsonObj).toJson();

    quint32 len = body.size() + 16;
    quint16 headerLen = 16;
    quint16 version = 1;
    quint32 optionCode = op;
    quint32 bodyHeaderLen = 1;

    QByteArray package;
    QDataStream writeStream(&package, QIODevice::WriteOnly);
    writeStream << len 
                << headerLen
                << version
                << optionCode
                << bodyHeaderLen
                << body; //这里QT会自动在body（json）前加入4个字节，表示json的大小，而这里不需要
    package.remove(16, 4); //故删除之

    return package; 
}

void Bilibili::unpacket(QByteArray &data)
{
    qsizetype pos = 0;

    while (pos < data.size())
    {
        QByteArray bodyLenData = data.mid(pos, 4);
        quint32 bodyLen;
        QDataStream readStream(&bodyLenData, QIODevice::ReadOnly);
        readStream >> bodyLen;

        QByteArray body = data.mid(pos + 16, bodyLen - 16);

        if (bodyLen > 16)
        {
            QJsonDocument jsonDocument = QJsonDocument::fromJson(body);
            if (!jsonDocument.isObject())
            {
                // qWarning()
                return;
            }
            QJsonObject jsonObj = jsonDocument.object();
            parse(jsonObj);
        }

        pos += bodyLen;
    }
}


void Bilibili::parse(QJsonObject &jsonObj)
{
    // qDebug() << "";
    // qDebug() << jsonObj;
    // return;

    if (!jsonObj.contains("cmd"))
    {
        // qDebug() << "无效json"
        return;
    }

    QString cmd = jsonObj.value("cmd").toString();
    if (cmd.contains("DANMU_MSG"))
    {
        QJsonArray danmuInfo = jsonObj.value("info").toArray();
        Danmu danmu(
            danmuInfo.at(1).toString(), // content
            danmuInfo.at(0).toArray().at(3).toInteger(), // content color
            danmuInfo.at(9).toObject().value("ts").toInteger(), // timestamp
            danmuInfo.at(2).toArray().at(0).toInteger(), // uid
            danmuInfo.at(2).toArray().at(1).toString() // uname
        );
        if (danmuInfo.at(3).toArray().size() > 0)
        {
            danmu.medalName = danmuInfo.at(3).toArray().at(1).toString(); // medal name
            danmu.medalLevel = danmuInfo.at(3).toArray().at(0).toInt(); // medal level
            danmu.medalColor = 0; //danmuInfo.at(3).toArray().at(1).(), // medal color
        }
        qDebug() << QDateTime::fromSecsSinceEpoch(danmu.timestamp).toString("yy-MM-dd HH:mm:ss")
                 << danmu.uname << "\t[" << danmu.medalLevel << danmu.medalName << "]:\t"
                 << danmu.content;
        danmuList.push_back(danmu);
        emit newDanmu(danmu);
    } 
    else if (cmd.contains("SEND_GIFT"))
    {
        QJsonObject giftData = jsonObj.value("data").toObject();
        Gift gift(
            giftData.value("is_first").toBool(),
            giftData.value("combo_stay_time").toInt(),
            giftData.value("giftId").toInt(),
            giftData.value("price").toInt(),
            giftData.value("num").toInt(),
            giftData.value("giftName").toString(),
            0,
            giftData.value("timestamp").toInteger(),
            giftData.value("uid").toInteger(),
            giftData.value("uname").toString(),
            giftData.value("medal_info").toObject().value("medal_name").toString(),
            giftData.value("medal_info").toObject().value("medal_level").toInteger(),
            giftData.value("medal_info").toObject().value("medal_color").toInteger()
        );
        qDebug() << QDateTime::fromSecsSinceEpoch(gift.timestamp).toString("yy-MM-dd HH:mm:ss")
                 << gift.uname << "\t[" << gift.medalLevel << gift.medalName << "]\tsend gift:"
                 << gift.content << "*" << gift.number;
        giftList.push_back(gift);
        emit newGift(gift);
    }
    else if (cmd.contains("INTERACT_WORD"))
    {
        QJsonObject entryData = jsonObj.value("data").toObject();
        Entry entry(
            entryData.value("timestamp").toInteger(),
            entryData.value("uid").toInteger(),
            entryData.value("uname").toString(),
            entryData.value("fans_medal").toObject().value("medal_name").toString(),
            entryData.value("fans_medal").toObject().value("medal_level").toInteger(),
            entryData.value("fans_medal").toObject().value("medal_color").toInteger()
        );
        qDebug() << QDateTime::fromSecsSinceEpoch(entry.timestamp).toString("yy-MM-dd HH:mm:ss")
                 << entry.uname << "\t[" << entry.medalLevel << entry.medalName << "] come";
        entryList.push_back(entry);
        emit newEntry(entry);
    }
}

void Bilibili::getRealRoomID(QString roomid)
//参考https://blog.csdn.net/hellokandy/article/details/122664900
{
    qDebug() << "void Bilibili::getRealRoomID(QString roomid)";
    QNetworkAccessManager *roomInfoManager = new QNetworkAccessManager(this);
    QNetworkRequest roomInfoRequest;
    roomInfoRequest.setUrl(QUrl(m_apiGetInfoByRoom + roomid));
    QNetworkReply *reply = roomInfoManager->get(roomInfoRequest);

    QEventLoop eventLoop;
    QTimer::singleShot(5000, &eventLoop, [&]() { 
        if (eventLoop.isRunning()) { eventLoop.quit(); } 
    });
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QJsonDocument roomInfo = QJsonDocument::fromJson(reply->readAll());
    if (!roomInfo.isObject())
    {
        return;
    }

    QJsonObject _obj = roomInfo.object();
    if (!_obj.contains("data"))
    {
        return;
    }

    QJsonObject _data = _obj.value("data").toObject();
    if (!_data.contains("room_info"))
    {
        return;
    }

    QJsonObject _room_info = _data.value("room_info").toObject();
    if (!_room_info.contains("room_id"))
    {
        return;
    }

    this->m_realRoomID = _room_info.value("room_id").toInteger();
}

void Bilibili::getToken(QString roomid)
{
    qDebug() << "void Bilibili::getToken(QString roomid)";
    QNetworkAccessManager *danmuInfoManager = new QNetworkAccessManager(this);
    QNetworkRequest danmuInfoRequest;
    danmuInfoRequest.setUrl(QUrl(m_apiGetDanmuInfo + roomid));
    QNetworkReply *reply = danmuInfoManager->get(danmuInfoRequest);

    QEventLoop eventLoop;
    QTimer::singleShot(5000, &eventLoop, [&]() { if (eventLoop.isRunning()) { eventLoop.quit(); } });
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QJsonDocument danmuInfo = QJsonDocument::fromJson(reply->readAll());
    if (!danmuInfo.isObject())
    {
        return;
    }

    QJsonObject _obj = danmuInfo.object();
    if (!_obj.contains("data"))
    {
        return;
    }

    QJsonObject _data = _obj.value("data").toObject();
    if (!_data.contains("token"))
    {
        return;
    }

    this->m_token = _data.value("token").toString();
}

} // namespace DanmuCore