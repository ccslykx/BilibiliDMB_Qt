/**
 * @file danmu.cpp
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 基于Qt的弹幕类实现
 * @version 0.1
 * @date 2022-06-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "danmu.h"
#include <QDebug>
#include <QVariant>
#include <QPropertyAnimation>
#include <QFont>
#include <QColor>

int Danmu::m_count = 0;
QMap<settings, QVariant> Danmu::m_settings = QMap<settings, QVariant>();
bool Danmu::isInitDefault = false;

// [public]
Danmu* Danmu::create(QString dm, int offset, QWidget *parent)
{
    return new Danmu(dm, offset, m_settings.value(settings::speed).toInt(), (Direction) m_settings.value(settings::direction).toInt(), parent);
}

Danmu* Danmu::create(QString dm, int offset, int speed, Direction from, QWidget *parent)
{
    return new Danmu(dm, offset, speed, from, parent);
}

int Danmu::dmCount()
{
    return(m_count);
}

QSize Danmu::dmSize()
{
    return QSize(this->sizeHint().width(), this->sizeHint().height());
}

QUuid Danmu::uuid()
{
    return(m_uuid);
}

void Danmu::destory()
{
    delete this;
}

void Danmu::set(settings key, QVariant value)
{
    if (m_settings.contains(key))
    {
        m_settings.remove(key);
    }
    
    m_settings.insert(key, value);
}

void Danmu::setSettings(QMap<settings, QVariant> &mappedSettings)
{
    m_settings = mappedSettings;
}

void Danmu::loadDefaultSettings()
{
    if (!m_settings.contains(settings::screenWidth)) 
        m_settings.insert(settings::screenWidth, 1920);
    if (!m_settings.contains(settings::screenHeight))
        m_settings.insert(settings::screenHeight, 1080);
    if (!m_settings.contains(settings::speed))
        m_settings.insert(settings::speed, 1);
    if (!m_settings.contains(settings::direction))
        m_settings.insert(settings::direction, Direction::RightToLeft);
    if (!m_settings.contains(settings::font))
        m_settings.insert(settings::font, QFont("SimHei", 20));
    if (!m_settings.contains(color))
        m_settings.insert(settings::color, QColor("Red"));

    isInitDefault = true;
}

// [protected]
Danmu::Danmu(QString dm, int offset, int speed, Direction from, QWidget *parent)
    : animation(new QPropertyAnimation(this, "pos"))
    , m_uuid(QUuid::createUuid())
    , m_dm(dm)
    , m_offset(offset)
{
    this->setParent(parent);
    init();
    m_count += 1;
    qDebug() << "成功创建弹幕：" << m_dm << "；当前屏幕上弹幕总数为：" << m_count;
}

Danmu::~Danmu()
{
    if (animation)
    {
        animation->deleteLater();
        animation = nullptr;
    }

    m_count -= 1;
    qDebug() << "成功删除弹幕：" << m_dm << "；当前屏幕上弹幕总数为：" << m_count;
}

// [private]
void Danmu::init()
{
    if (!isInitDefault)
    {
        loadDefaultSettings();
        isInitDefault = true;
    }

    connect(animation, &QPropertyAnimation::finished, this, &Danmu::destory);

    this->setText(m_dm);
    
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); //无边框、窗口最上层
    this->setAttribute(Qt::WA_TranslucentBackground); //透明背景
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true); //屏蔽鼠标事件

    this->setFont(m_settings.value(settings::font).value<QFont>()); // QVariant to QFont
    
    QPalette p;
    p.setColor(QPalette::WindowText, m_settings.value(color).value<QColor>());
    this->setPalette(p);
    this->move(m_settings.value(screenWidth).toInt(), m_offset);

    QSize danmuSize = dmSize();

    /**
     * @bug 不知道为啥，动画开始和结束都不在屏幕外 (Arch Linux 5.18.5)
     */

    qDebug() << QString("当前弹幕的位置为: (%1, %2)，大小为: (%3, %4)")
                .arg(   QString::number(this->x()), 
                        QString::number(this->y()), 
                        QString::number(danmuSize.width()), 
                        QString::number(danmuSize.height()));
    animation->setStartValue(QPoint(this->x(), this->y())); 
    animation->setEndValue(QPoint(-danmuSize.width() , this->y())); // 直接使用width()可能有问题？使用SizeHint().width()更合适 
    animation->setDirection(QAbstractAnimation::Direction::Forward);
    animation->setDuration(10.0 / m_settings.value(speed).toInt() * 1000);
    
    this->show();
    animation->start();
}
