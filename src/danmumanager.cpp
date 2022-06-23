#include "danmumanager.h"
#include <QDebug>

DanmuManager* DanmuManager::m_manager = nullptr;
QMap<settings, QVariant> DanmuManager::m_settings = QMap<settings, QVariant>();
bool DanmuManager::isInitDefault = false;

DanmuManager* DanmuManager::instance(int speed, Direction from, QWidget *parent)
{
    if (m_manager == nullptr)
    {
        m_manager = new DanmuManager(speed, from);
    }
    return(m_manager);
}

DanmuManager::~DanmuManager()
{
    if (m_manager != nullptr)
    {
        delete m_manager;
        m_manager = nullptr;
    }
}

int DanmuManager::count()
{
    return Danmu::dmCount();
}

void DanmuManager::createDM(QString content)
{
    Danmu *dm = Danmu::create(content, m_nextOffset, this->m_parent);
    calculateOffset(dm->dmSize()); //计算下一个弹幕的位置 
}

void DanmuManager::set(settings key, QVariant value)
{
    if (m_settings.contains(key))
    {
        m_settings.remove(key);
    }
    m_settings.insert(key, value);
}

void DanmuManager::loadDefaultSettings()
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

void DanmuManager::configDanmuSettings()
{
    Danmu::setSettings(m_settings);
}

void DanmuManager::calculateOffset(QSize s)
{
    m_nextOffset = s.height() * 1.5 + m_nextOffset;
    m_nextOffset = m_nextOffset > 1080 - s.height() ? 0 : m_nextOffset;
    qDebug() << "m_nextOffset = " << QString::number(m_nextOffset);
}

DanmuManager::DanmuManager(int speed, Direction from, QWidget *parent)
    : m_parent(parent)
{
    this->setParent(parent); //设置父类
    set(settings::speed, speed);
    set(settings::direction, from);
    m_nextOffset = 0;

    loadDefaultSettings();
}

