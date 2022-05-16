//设置字体颜色宏定义
//https://blog.csdn.net/duapple/article/details/120101489

#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QTimer>

#include "bilibiliDMB.h"
#include "utils.h"
#include "ui_bilibiliDMB.h"

// Construct
BilibiliDMB::BilibiliDMB(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BilibiliDMB)
    //参考https://blog.csdn.net/rjc_lihui/article/details/88397009
{
    CALL("BilibiliDMB::BilibiliDMB(QWidget *parent)");

    ui->setupUi(this);
    ui->disconnectButton->setEnabled(false);

    QObject::connect(ui->connectButton, &QPushButton::clicked, this, &BilibiliDMB::_connect);
    QObject::connect(ui->disconnectButton, &QPushButton::clicked, this, &BilibiliDMB::_disconnect);
    QObject::connect(ui->saveHistoryButton, &QPushButton::clicked, this, &BilibiliDMB::_saveHistory);
    QObject::connect(ui->clearButton, &QPushButton::clicked, this, &BilibiliDMB::_clear);

    cmd["__DEFAULT__"] = defaultCMD;
    cmd["DANMU_MSG"] = dm;
    cmd["SEND_GIFT"] = gift;
    cmd["COMBO_SEND"] = comboGift;
    cmd["INTERACT_WORD"] = entry;

    // for test
    ui->roomid->setText("23165114");
    INFO(tr("%1“BilibiliDMB”%2").arg("应用程序", "已启动"));
}

BilibiliDMB::~BilibiliDMB()
{
    CALL("BilibiliDMB::~BilibiliDMB()");
    delete ui;
    INFO(tr("%1“BilibiliDMB”%2").arg("应用程序", "已退出"));
}

QByteArray BilibiliDMB::packet(int op)
{
    CALL("QByteArray BilibiliDMB::packet(int op)");

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

    INFO("packet(" << op << ")");

    return package;
}

void BilibiliDMB::onReceive(QByteArray data)
{
    CALL("void BilibiliDMB::onReceive(QByteArray data)");

    INFO(tr("%1").arg("接收到") + QString::number(data.size()) + tr("%2").arg("字节数据"));

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
        INFO("case 0 JSON");
        jsonDocument = QJsonDocument::fromJson(data);
        if (!jsonDocument.isObject())
        {
            WARNING("!jsonDocument.isObject()");
            return;
        }
        json = jsonDocument.object();
        A(json);
        break;

    case 1: // 人气
        INFO("case 1 人气");
        jsonDocument = QJsonDocument::fromJson(data);

        INFO(QString::fromUtf8(data));
        break;

    case 2: // zlib JSON
        INFO("case 2 zlib JSON");
        unCompressedData = qUncompress(data);
        unpack(unCompressedData);
        break;

    case 3: // brotli JSON
        INFO("case 3 brotli JSON");
        break;

    default:
        INFO("case default");
        jsonDocument = QJsonDocument::fromJson(data);
        if (!jsonDocument.isObject())
        {
            WARNING("!jsonDocument.isObject()");
            return;
        }
        json = jsonDocument.object();
        INFO(json);
        break;
    }

}

void BilibiliDMB::unpack(QByteArray &data)
{
    CALL("void BilibiliDMB::unpack(QByteArray &data)");

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
            WARNING("@unpack !jsonDocument.isObject()");
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
    CALL("void BilibiliDMB::A(QJsonObject &json)");

    if (!json.contains("cmd"))
    {
        INFO("@A !json.contains(\"cmd\")");
        return;
    }
    QString currentCMD = json.value("cmd").toString();
    INFO("@A CMD = " << currentCMD);

    QString message;
    QString textColor;

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
        textColor = DMs.last().color == 16777215 ? "000000" : QString::number(DMs.last().color, 16);
        message += "<font color=\"gray\">" + QDateTime::fromSecsSinceEpoch(DMs.last().timestamp).toString("HH:mm:ss") + "</font> "
                + DMs.last().uname + " : "
                + "<font color=\"#" + textColor + "\">" + DMs.last().content + "</font>";

        ui->dmBoard->append(message);

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

        message += "<font color=\"gray\">" + QDateTime::fromSecsSinceEpoch(GIFTs.last().timestamp).toString("HH:mm:ss") + "</font> "
                + "<font color=\"#4169E1\">" + GIFTs.last().uname + "</font>" + tr(" 送出了 ")
                + GIFTs.last().giftName
                + " * " + QString::number(GIFTs.last().num);
        ui->giftBoard->append(message);
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
        message += ENTRYs.last().uname + tr(" <font color = \"gray\">进入了直播间</font>");
        ui->entryLabel->setText(message);
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
    CALL("void BilibiliDMB::setRoomID()");

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
        WARNING("!roomInfo.isObject()    @setRoomID");
        return;
    }

    QJsonObject _obj = roomInfo.object();
    if (!_obj.contains("data"))
    {
        WARNING("!_obj.contains(\"data\")    @setRoomID");
        return;
    }

    QJsonObject _data = _obj.value("data").toObject();
    if (!_data.contains("room_info"))
    {
        WARNING("!_data.contains(\"room_info\")    @setRoomID");
        return;
    }

    QJsonObject _room_info = _data.value("room_info").toObject();
    if (!_room_info.contains("room_id"))
    {
        WARNING("!_room_info.contains(\"room_id\")    @setRoomID");
        return;
    }

    this->realRoomID = _room_info.value("room_id").toInteger();

    INFO("realRoomID = " + QString::number(realRoomID) + "    @setRoomID");
}

void BilibiliDMB::setToken()
//参考https://blog.csdn.net/hellokandy/article/details/122664900
{
    CALL("void BilibiliDMB::setToken()");

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
        WARNING("!danmuInfo.isObject()    @setToken");
        return;
    }

    QJsonObject _obj = danmuInfo.object();
    if (!_obj.contains("data"))
    {
        WARNING("!_obj.contains(\"data\")    @setToken");
        return;
    }

    QJsonObject _data = _obj.value("data").toObject();
    if (!_data.contains("token"))
    {
        WARNING("!_data.contains(\"token\")    @setToken");
        return;
    }

    this->token = _data.value("token").toString();
    INFO("token = " + token + "    @setToken");
}

// Public slots
void BilibiliDMB::_connect()
{
    CALL("void BilibiliDMB::_connect()");

    setRoomID();
    setToken();
    INFO(tr("%1 RealRoomID %2 Token.").arg("成功获取到", "和"));

    // connect
    socket = new QWebSocket();
    QObject::connect(this->socket, &QWebSocket::connected, this, &BilibiliDMB::onConnected);
    QNetworkRequest request;
    request.setUrl(QUrl("wss://broadcastlv.chat.bilibili.com:443/sub"));
    socket->open(request);

    ui->disconnectButton->setEnabled(true);
    ui->connectButton->setEnabled(false);
    INFO(tr("%1").arg("连接成功"));

}

void BilibiliDMB::onConnected()
{
    CALL("void BilibiliDMB::onConnected()");

    INFO(tr("WebSocket %1").arg("连接成功"));
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
    CALL("void BilibiliDMB::_disconnect()");

    if (heartbeatTimer->isActive()) heartbeatTimer->stop();
    socket->close();

    delete heartbeatTimer;
    delete socket;

    INFO(tr("WebSocket %1").arg("断开连接"));

    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
}

void BilibiliDMB::_saveHistory()
{
    CALL("void BilibiliDMB::_saveHistory()");
}

void BilibiliDMB::_clear()
{
    CALL("void BilibiliDMB::_clear()");

    ui->dmBoard->clear();
    ui->giftBoard->clear();
    ui->entryLabel->clear();
    DMs.clear();
    GIFTs.clear();
    ENTRYs.clear();
}

