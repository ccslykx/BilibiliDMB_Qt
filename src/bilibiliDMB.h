#pragma once

#include "danmumanager.h"

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class BilibiliDMB; }
QT_END_NAMESPACE

struct DM {
    qint64 timestamp;
    qint64 uid;
    qint32 color;
    int metal_level;
    qint32 metal_color;
    QString metal_name;
    QString uname;
    QString content;
};

struct GIFT {
    bool is_first;
    qint64 timestamp;
    int combo_stay_time;
    int giftId;
    int price;
    int num;
    int medal_level;
    qint32 medal_color;
    QString medal_name;
    QString uname;
    QString giftName;
};

struct ENTRY {
    qint64 timestamp;
    int medal_level;
    qint32 medal_color;
    QString medal_name;
    QString uname;
};

enum CMD {
    defaultCMD = 999,
    dm = 1, //弹幕消息
    gift = 2, //投喂礼物
    comboGift = 3, //连击礼物
    //LIVE_INTERACTIVE_GAME
    entry = 4, //进入房间
    //case ENTRY_EFFECT //欢迎舰长进入房间
};

class BilibiliDMB: public QMainWindow {
    Q_OBJECT

private:
    QByteArray packet(int);
    void unpack(QByteArray &);
    void A(QJsonObject &);
    void setRoomID();
    void setToken();

private slots:
    void onConnected();
    void onReceive(QByteArray);

public:
    explicit BilibiliDMB(QWidget *parent = nullptr);
    ~BilibiliDMB();

public slots:
    void connect();
    void disconnect();
    void saveHistory();
    void clear();

private:
    Ui::BilibiliDMB *ui;

    qint64 realRoomID = 0;
    //QString address = "broadcastlv.chat.bilibili.com";
    //quint16 wss_port = 443; // ws:2244 wss:443

    QString apiGetInfoByRoom = "https://api.live.bilibili.com/xlive/web-room/v1/index/getInfoByRoom?room_id=";
    QString apiGetDanmuInfo = "https://api.live.bilibili.com/xlive/web-room/v1/index/getDanmuInfo?id=";
    QString token = "";

    QWebSocket *socket;
    QTimer *heartbeatTimer;

    QMap<QString, CMD> cmd;
    QList<DM> DMs;
    QList<GIFT> GIFTs;
    QList<ENTRY> ENTRYs;

    static DanmuManager *manager;
};
