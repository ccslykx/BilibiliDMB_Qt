//设置字体颜色宏定义
//https://blog.csdn.net/duapple/article/details/120101489


#include <QString>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QFile>
#include <QtZlib/zlib.h>
#include <QTimer>

#include "bilibiliDMB.h"
#include "ui_bilibiliDMB.h"
#include "plugin.h"

// Construct
BilibiliDMB::BilibiliDMB(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BilibiliDMB)
    //参考https://blog.csdn.net/rjc_lihui/article/details/88397009
{
    ui->setupUi(this);
    ui->disconnectButton->setEnabled(false);


    QObject::connect(ui->connectButton, &QPushButton::clicked, this, &BilibiliDMB::_connect);
    QObject::connect(ui->disconnectButton, &QPushButton::clicked, this, &BilibiliDMB::_disconnect);
    QObject::connect(ui->saveHistoryButton, &QPushButton::clicked, this, &BilibiliDMB::_saveHistory);
    QObject::connect(ui->clearButton, &QPushButton::clicked, this, &BilibiliDMB::_clear);

    cmd["__DEFAULT__"] = whatever;
    cmd["DANMU_MSG"] = dm;
    cmd["SEND_GIFT"] = gift;
    cmd["COMBO_SEND"] = comboGift;
    cmd["INTERACT_WORD"] = entry;

    // for test
    ui->roomid->setText("8765806");
}

BilibiliDMB::~BilibiliDMB()
{
    delete ui;
}


// Private functions
void BilibiliDMB::sendAuth()
{
    //socket->write(packet(7));
}

void BilibiliDMB::sendHeartbeat()
{
    //socket->write(packet(2));
}

QByteArray BilibiliDMB::packet(int op)
{
    QJsonObject json;
    switch (op)
    {
    case 7: // 认证包
        json.insert("uid", 0);
        json.insert("roomid", realRoomID);
        json.insert("protover", 2);
        json.insert("platform", "web");
        json.insert("type", 2);
        json.insert("clientver", "1.14.3");
        json.insert("key", token);
        break;

    default: // 心跳包
        break;
    }

    QByteArray body = QJsonDocument(json).toJson();

    quint32 len = body.size() + 16;
    quint16 headerLen = 16;
    quint16 version = 1;
    quint32 optionCode = op;
    quint32 bodyHeaderLen = 1;

    QByteArray package;
    QDataStream stream(&package, QIODevice::WriteOnly);
    stream << len
           << headerLen
           << version
           << optionCode
           << bodyHeaderLen
           << body; //这里会自动在body（json）前加入4个字节，表示json的大小，而这里不需要
    package.remove(16, 4); //故删除之

    qDebug() << "packet(" << op << ")";

    return package;
}

void BilibiliDMB::onReceive(QByteArray data)
{
    qDebug() << "Received: " + QString::number(data.size()) + " bytes";

    quint16 version = data.mid(6, 2).toHex().toUShort(); // mid()从中间截取一段数据

    data.remove(4, 12); //结合下面，少个插入操作
    //参考https://blog.csdn.net/doujianyoutiao/article/details/106236207
    changeHeaderSize(&data, 12, false);

    //为了switch
    QJsonDocument jsonDocument;
    QJsonObject json;
    QByteArray unCompressedData;

    switch (version) {
    case 0: // JSON
        jsonDocument = QJsonDocument::fromJson(data);
        if (!jsonDocument.isObject())
        {
            qDebug() << "[unpack] !jsonDocument.isObject()";
            return;
        }
        json = jsonDocument.object();
        A(json);
        qDebug() << "case 0 JSON";
        break;

    case 1: // 人气
        qDebug() << "case 1 人气";
        break;

    case 2: // zlib JSON
        qDebug() << "case 2 zlib JSON";
        unCompressedData = qUncompress(data);
        unpack(unCompressedData);
        break;

    case 3: // brotli JSON
        qDebug() << "case 3 brotli JSON";
        break;

    default:
        qDebug() << "case default";
        break;
    }

}

void BilibiliDMB::unpack(QByteArray &data)
{
    QByteArray bodyLenData = data.mid(0, 4);
    quint32 bodyLen;
    QDataStream read(&bodyLenData, QIODevice::ReadOnly);
    read >> bodyLen;
    QByteArray cur = data.mid(16, bodyLen-16);

    if (bodyLen > 16)
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(cur);
        if (!jsonDocument.isObject())
        {
            qDebug() << "[unpack] !jsonDocument.isObject()";
            return;
        }
        QJsonObject json = jsonDocument.object();
        A(json);

        if (data.size() > bodyLen)
        {
            QByteArray res = data.mid(bodyLen, data.size()-bodyLen);
            unpack(res);
        }
    }
}

void BilibiliDMB::A(QJsonObject &json)
{
    if (!json.contains("cmd"))
    {
        qDebug() << "[A] !json.contains(\"cmd\")";
        return;
    }
    QString currentCMD = json.value("cmd").toString();
    qDebug() << "[A] cmd = " << currentCMD;

    QString dmMessage;
    QString giftMessage;
    QString entryMessage;

    switch (cmd.value(currentCMD))
    {
    case dm:
        if (json.value("info")[3].toObject().size() <= 0)
        {
            DMs.append({
                           json.value("info").toArray().at(9).toObject().value("ts").toInteger(),
                           json.value("info").toArray().at(2).toArray().at(0).toInteger(),
                           json.value("info").toArray().at(0).toArray().at(3).toInt(),
                           0,
                           16777215,
                           "",
                           json.value("info").toArray().at(2).toArray().at(1).toString(),
                           json.value("info").toArray().at(1).toString()
                       });
        } else {
            DMs.append({
                           json.value("info").toArray().at(9).toObject().value("ts").toInteger(),
                           json.value("info").toArray().at(2).toArray().at(0).toInteger(),
                           json.value("info").toArray().at(0).toArray().at(3).toInt(),
                           json.value("info").toArray().at(3).toArray().at(0).toInt(),
                           json.value("info").toArray().at(3).toArray().at(4).toInt(),
                           json.value("info").toArray().at(3).toArray().at(1).toString(),
                           json.value("info").toArray().at(2).toArray().at(1).toString(),
                           json.value("info").toArray().at(1).toString()
                       });
        }

        dmMessage += QDateTime::fromSecsSinceEpoch(DMs.last().timestamp).toString("HH:mm:ss") + " "
                + "<font color=\"#4169E1\">" + DMs.last().uname + "</font>" + " : "
                + DMs.last().content;
        ui->dmBoard->append(dmMessage);

        break;

    case gift:
        GIFTs.append({
                         json.value("data").toObject().value("is_first").toBool(),
                         json.value("data").toObject().value("timestamp").toInteger(),
                         json.value("data").toObject().value("combo_stay_time").toInt(),
                         json.value("data").toObject().value("giftId").toInt(),
                         json.value("data").toObject().value("price").toInt(),
                         json.value("data").toObject().value("num").toInt(),
                         json.value("data").toObject().value("medal_info").toObject().value("medal_level").toInt(),
                         json.value("data").toObject().value("medal_info").toObject().value("medal_color").toInt(),
                         json.value("data").toObject().value("medal_info").toObject().value("medal_name").toString(),
                         json.value("data").toObject().value("uname").toString(),
                         json.value("data").toObject().value("giftName").toString()
                     });

        giftMessage += QDateTime::fromSecsSinceEpoch(GIFTs.last().timestamp).toString("HH:mm:ss") + " "
                + "<font color=\"#4169E1\">" + GIFTs.last().uname + "</font>" + tr(" 送出了 ")
                + GIFTs.last().giftName
                + " * " + QString::number(GIFTs.last().num);
        ui->giftBoard->append(giftMessage);
        break;

    case comboGift:
        //【未完成】
        break;

    case entry:
        //【未完成】
        ENTRYs.append({
                          json.value("data").toObject().value("timestamp").toInteger(),
                          json.value("data").toObject().value("fans_medal").toObject().value("metal_level").toInt(),
                          json.value("data").toObject().value("fans_medal").toObject().value("metal_color").toInt(),
                          json.value("data").toObject().value("fans_medal").toObject().value("metal_name").toString(),
                          json.value("data").toObject().value("uname").toString()
                      });
        entryMessage += ENTRYs.last().uname + tr(" 进入了直播间");
        ui->entryLabel->setText(entryMessage);
        break;

    default:
        break;
    }
}

/* Qt的Http Get请求是异步的，而我需要阻塞进行，故弃用preConnec()
void BilibiliDMB::preConnect()
{
    // get info from api
    QNetworkAccessManager *roomInfoManager = new QNetworkAccessManager(this);
    QNetworkRequest roomInfoRequest;
    roomInfoRequest.setUrl(QUrl(apiGetInfoByRoom + ui->roomid->text()));

    QNetworkAccessManager *danmuInfoManager = new QNetworkAccessManager(this);
    QNetworkRequest danmuInfoRequest;
    danmuInfoRequest.setUrl(QUrl(apiGetDanmuInfo + ui->roomid->text()));

    connect(roomInfoManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(setRoomID(QNetworkReply*)));
    connect(danmuInfoManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(setToken(QNetworkReply*)));

    roomInfoManager->get(roomInfoRequest);
    danmuInfoManager->get(danmuInfoRequest);
}
*/

void BilibiliDMB::setRoomID()
//参考https://blog.csdn.net/hellokandy/article/details/122664900
{
    QNetworkAccessManager *roomInfoManager = new QNetworkAccessManager(this);
    QNetworkRequest roomInfoRequest;
    roomInfoRequest.setUrl(QUrl(apiGetInfoByRoom + ui->roomid->text()));
    QNetworkReply *reply = roomInfoManager->get(roomInfoRequest);

    QEventLoop eventLoop;
    QTimer::singleShot(5000, &eventLoop, [&]() { if (eventLoop.isRunning()) { eventLoop.quit(); } });
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QJsonDocument roomInfo = QJsonDocument::fromJson(reply->readAll());
    if (!roomInfo.isObject())
    {
        qDebug() << "!roomInfo.isObject()    @setRoomID";
        return;
    }

    QJsonObject _obj = roomInfo.object();
    if (!_obj.contains("data"))
    {
        qDebug() << "!_obj.contains(\"data\")    @setRoomID";
        return;
    }

    QJsonObject _data = _obj.value("data").toObject();
    if (!_data.contains("room_info"))
    {
        qDebug() << "!_data.contains(\"room_info\")    @setRoomID";
        return;
    }

    QJsonObject _room_info = _data.value("room_info").toObject();
    if (!_room_info.contains("room_id"))
    {
        qDebug() << "!_room_info.contains(\"room_id\")    @setRoomID";
        return;
    }

    this->realRoomID = _room_info.value("room_id").toInteger();

    qDebug() << "realRoomID = " + QString::number(realRoomID) + "    @setRoomID";
}

void BilibiliDMB::setToken()
//参考https://blog.csdn.net/hellokandy/article/details/122664900
{
    QNetworkAccessManager *danmuInfoManager = new QNetworkAccessManager(this);
    QNetworkRequest danmuInfoRequest;
    danmuInfoRequest.setUrl(QUrl(apiGetDanmuInfo + ui->roomid->text()));
    QNetworkReply *reply = danmuInfoManager->get(danmuInfoRequest);

    QEventLoop eventLoop;
    QTimer::singleShot(5000, &eventLoop, [&]() { if (eventLoop.isRunning()) { eventLoop.quit(); } });
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QJsonDocument danmuInfo = QJsonDocument::fromJson(reply->readAll());
    if (!danmuInfo.isObject())
    {
        qDebug() << "!danmuInfo.isObject()    @setToken";
        return;
    }

    QJsonObject _obj = danmuInfo.object();
    if (!_obj.contains("data"))
    {
        qDebug() << "!_obj.contains(\"data\")    @setToken";
        return;
    }

    QJsonObject _data = _obj.value("data").toObject();
    if (!_data.contains("token"))
    {
        qDebug() << "!_data.contains(\"token\")    @setToken";
        return;
    }

    this->token = _data.value("token").toString();
    qDebug() << "token = " + token + "    @setToken";
}

// Public slots
void BilibiliDMB::_connect()
{
    setRoomID();
    setToken();
    qDebug() << "Got RealRoomID and Token.";

    // connect
    socket = new QWebSocket();
    QObject::connect(this->socket, &QWebSocket::connected, this, &BilibiliDMB::onConnected);
    QNetworkRequest request;
    request.setUrl(QUrl("wss://broadcastlv.chat.bilibili.com:443/sub"));
    socket->open(request);

    ui->disconnectButton->setEnabled(true);
    ui->connectButton->setEnabled(false);
}

void BilibiliDMB::onConnected() {
    qDebug() << "WebSocket connected";
    QObject::connect(this->socket, &QWebSocket::binaryMessageReceived,
            this, &BilibiliDMB::onReceive);
    socket->sendBinaryMessage(packet(7));
    socket->sendBinaryMessage(packet(2));
    // if no error
        heartbeatTimer = new QTimer(this);
        QObject::connect(heartbeatTimer, &QTimer::timeout, this, [this](){
            this->socket->sendBinaryMessage(packet(2));
        });
        heartbeatTimer->start(30000);
    // end if
}

void BilibiliDMB::_disconnect()
{
    if (heartbeatTimer->isActive()) heartbeatTimer->stop();
    socket->close();

    delete heartbeatTimer;
    delete socket;

    qDebug() << "WebSocket disconnected";

    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
}

void BilibiliDMB::_saveHistory()
{
//    QFile textFile = QFile("D:/text.txt");
//    if (!textFile.open(QIODevice::WriteOnly))
//        return;
//    QTextStream textOut(&textFile);
//    textOut << packet(7);
//    textFile.close();
//    qDebug() << "TextSteam Done";

//    QFile binFile = QFile("D:/bin.txt");
//    if (!binFile.open(QIODevice::WriteOnly))
//        return;
//    QDataStream binOut(&binFile);
//    binOut << packet(7);
//    binFile.close();
//    qDebug() << "DataSteam Done";
}

void BilibiliDMB::_clear()
{
    ui->dmBoard->clear();
    ui->giftBoard->clear();
    ui->entryLabel->clear();
}
