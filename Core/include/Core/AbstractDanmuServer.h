/**
 * @file AbstractDanmuServer.h
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 
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

namespace DanmuCore
{

class AbstractDanmuServer : public QObject 
{
    Q_OBJECT

public:
    explicit AbstractDanmuServer() { qDebug() << "AbstractDanmuServer Constructed!"; };
    virtual ~AbstractDanmuServer() { qDebug() << "AbstractDanmuServer Deconstructed!"; };

signals:
    void newEntry(Entry);
    void newDanmu(Danmu);
    void newGift(Gift);

public slots:
    virtual void connect(QString roomid) = 0;
    virtual void disconnect() = 0;

protected:
    virtual QByteArray packet(int) = 0;
    virtual void unpacket(QByteArray &) = 0;
    virtual void parse(QJsonObject &) = 0;

protected slots:
    virtual void onReceive(QByteArray) = 0;
    virtual void onConnected() = 0;
    
protected:
    QString     m_roomID = "";
    QString     m_token = "";

    QWebSocket  *m_socket = nullptr;
    QTimer      *m_heartbeatTimer = nullptr;
    
    TmpList<Danmu>    danmuList;
    TmpList<Entry>    entryList;
    TmpList<Gift>     giftList;
};

} // namespace DanmuCore
