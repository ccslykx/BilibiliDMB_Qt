/**
 * @file Danmu.h
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 通用弹幕类（目前基于B站直播间弹幕设计）
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <QObject>
#include <QVector>
#include <QString>

namespace DanmuCore
{

class Entry : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    Entry(quint64 timestamp, quint64 uid, QString uname,
          QString medalName = "", uint medalLevel = 0, qint32 medalColor = 0,
          QString title = "", qint32 titleColor = 0) 
        : timestamp(timestamp)
        , uid(uid)
        , uname(uname)
        , medalName(medalName)
        , medalLevel(medalLevel)
        , medalColor(medalColor)
        , title(title)
        , titleColor(titleColor) {};
    Entry(const Entry& e) 
        : timestamp(e.timestamp)
        , uid(e.uid)
        , uname(e.uname)
        , medalName(e.medalName)
        , medalLevel(e.medalLevel)
        , medalColor(e.medalColor)
        , title(e.title)
        , titleColor(e.titleColor) {};
    Entry& operator=(const Entry& e) {
        this->timestamp = e.timestamp;
        this->uid = e.uid;
        this->uname = e.uname;
        this->medalName = e.medalName;
        this->medalLevel = e.medalLevel;
        this->medalColor = e.medalColor;
        this->title = e.title;
        this->titleColor = e.titleColor;
        return *this;
    };
    ~Entry() {};

public:
    quint64 timestamp;
    
    quint64 uid;
    QString uname;

    QString medalName;
    uint    medalLevel;
    qint32  medalColor;

    QString title;
    qint32  titleColor;
};

class Danmu : public Entry
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString content READ Content)

public:
    Danmu(QString content, qint32 contentColor,
          quint64 timestamp, quint64 uid, QString uname,
          QString medalName = "", uint medalLevel = 0, qint32 medalColor = 0,
          QString title = "", qint32 titleColor = 0) 
        : Entry(timestamp, uid, uname, medalName, medalLevel,
                medalColor, title, titleColor)
        , content(content)
        , contentColor(contentColor) {};
    Danmu(const Danmu& e) 
        : Entry(e.timestamp, e.uid, e.uname, e.medalName, e.medalLevel,
                e.medalColor, e.title, e.titleColor)
        , content(e.content)
        , contentColor(e.contentColor) {};
    Danmu& operator=(const Danmu& e) 
    {
        this->content = e.content;
        this->contentColor = e.contentColor;
        this->timestamp = e.timestamp;
        this->uid = e.uid;
        this->uname = e.uname;
        this->medalName = e.medalName;
        this->medalLevel = e.medalLevel;
        this->medalColor = e.medalColor;
        this->title = e.title;
        this->titleColor = e.titleColor;
        return *this;
    }
    ~Danmu() {};

    QString Content() { return content; };


public:
    QString content;
    qint32  contentColor;
};

class Gift : public Danmu
{
    Q_OBJECT
    QML_ELEMENT

public:
    Gift(bool isFirst, int comboTime, int giftID, int price, int number,
         QString content, qint32 contentColor,
         quint64 timestamp, quint64 uid, QString uname,
         QString medalName = "", uint medalLevel = 0, qint32 medalColor = 0,
         QString title = "", qint32 titleColor = 0)
        : Danmu(content, contentColor, timestamp, uid, uname, medalName, 
                medalLevel, medalColor, title, titleColor)
        , isFirst(isFirst)
        , comboTime(comboTime)
        , giftID(giftID)
        , price(price)
        , number(number) {};
    Gift(const Gift& e)
        : Danmu(e.content, e.contentColor, e.timestamp, e.uid, e.uname, 
                e.medalName, e.medalLevel, e.medalColor, e.title, e.titleColor)
        , isFirst(e.isFirst)
        , comboTime(e.comboTime)
        , giftID(e.giftID)
        , price(e.price)
        , number(e.number) {};
    Gift& operator=(const Gift& e) 
    {
        this->isFirst = e.isFirst;
        this->comboTime = e.comboTime;
        this->giftID = e.giftID;
        this->price = e.price;
        this->number = e.number;
        this->content = e.content;
        this->contentColor = e.contentColor;
        this->timestamp = e.timestamp;
        this->uid = e.uid;
        this->uname = e.uname;
        this->medalName = e.medalName;
        this->medalLevel = e.medalLevel;
        this->medalColor = e.medalColor;
        this->title = e.title;
        this->titleColor = e.titleColor;
        return *this;
    }
    ~Gift() {};

public:
    bool isFirst;
    int comboTime;
    int giftID;
    int price;
    int number;
};

template<typename T>
class TmpList : public QObject
{

public:
    TmpList<T>() { m_data = QVector<T>(); };
    ~TmpList<T>() {};

    void push_back(T &e) { m_data.push_back(e); }

    QVector<T> get(qsizetype start, qsizetype len = -1LL) 
    { 
        if (start > m_data.size) { return QVector<T>(); }
        if (len == -1LL || start + len > m_data.size) 
        { 
            return QVector<T>(m_data.begin() + start, m_data.end()); 
        }
        return QVector<T>(m_data.begin() + start, m_data.begin() + start + len);
    };
    QVector<T> getNew() { return get(m_pushStart); };
    QVector<T> getAll() { return m_data; };

    /* TODO
        完成保存历史记录的功能
    */
    QVector<T> saveNew();
    QVector<T> saveAll();

    void setSavePath();

private:
    QVector<T>  m_data;
    QString     m_path = ".";
    qsizetype   m_saveStart = 0; 
    qsizetype   m_pushStart = 0;
};

} // namespace DanmuCore