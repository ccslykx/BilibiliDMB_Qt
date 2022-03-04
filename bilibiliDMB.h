#ifndef BILIBILIDMB_H
#define BILIBILIDMB_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class BilibiliDMB; }
QT_END_NAMESPACE

class BilibiliDMB: public QMainWindow {
    Q_OBJECT

private:
    void sendAuth();
    void sendHeartbeat();
    QByteArray packet(int);
    void unpack(QByteArray &);
    void A(QJsonObject &);
    void setRoomID();
    void setToken();

private slots:
    //QByteArray receive();
    void onConnected();
    void onReceive(QByteArray);

public:
    explicit BilibiliDMB(QWidget *parent = nullptr);
    ~BilibiliDMB();

public slots:

    void _connect();
    void _disconnect();
    void _saveHistory();
    void _clear();

private:
    Ui::BilibiliDMB *ui;

    qint64 realRoomID = 0;
    QString address = "broadcastlv.chat.bilibili.com";
    quint16 wss_port = 443; // ws:2245 wss:443
    QString token = "token";

    QString apiGetInfoByRoom = "https://api.live.bilibili.com/xlive/web-room/v1/index/getInfoByRoom?room_id=";
    QString apiGetDanmuInfo = "https://api.live.bilibili.com/xlive/web-room/v1/index/getDanmuInfo?id=";

    QWebSocket *socket;
    QTimer *heartbeatTimer;

    enum CMD {
        whatever,
        dm, //弹幕消息
        gift, //投喂礼物
        comboGift, //连击礼物
        //LIVE_INTERACTIVE_GAME
        entry, //进入房间
        //case ENTRY_EFFECT //欢迎舰长进入房间
    };

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

    QMap<QString, CMD> cmd;
    QList<DM> DMs;
    QList<GIFT> GIFTs;
    QList<ENTRY> ENTRYs;
};



#endif // BILIBILIDMB_H
