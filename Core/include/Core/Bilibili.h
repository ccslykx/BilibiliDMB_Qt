/**
 * @file Bilibili.h
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 接收B站直播间弹幕的后端程序
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

#include <Core/Danmu.h>
#include <Core/AbstractDanmuServer.h>

namespace DanmuCore
{

class Bilibili : public AbstractDanmuServer::AbstractDanmuServer
{
    Q_OBJECT

public:
    explicit Bilibili();
    ~Bilibili();

public slots:
    void connect(QString roomid);
    void disconnect();

    void onReceive(QByteArray);
    void onConnected();

private:
    QByteArray packet(int);
    void unpacket(QByteArray &);
    void parse(QJsonObject &);
    void getRealRoomID(QString roomid);
    void getToken(QString roomid);

private:
    qint64     m_realRoomID = -1;
    QString     m_apiGetInfoByRoom = "https://api.live.bilibili.com/xlive/web-room/v1/index/getInfoByRoom?room_id=";
    QString     m_apiGetDanmuInfo = "https://api.live.bilibili.com/xlive/web-room/v1/index/getDanmuInfo?id=";
};

} // namespace DanmuCore
