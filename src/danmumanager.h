#ifndef DANMUMANAGER_H
#define DANMUMANAGER_H

#include "danmu.h"
#include <QObject>

class DanmuManager : public QObject
{
    Q_OBJECT
public:
    static DanmuManager *instance(int speed = 1, Direction from = RightToLeft, QWidget *parent = nullptr);
    ~DanmuManager();
    int count();
    void createDM(QString content);

    static void set(settings, QVariant);
    static void loadDefaultSettings();
    void configDanmuSettings();

private:
    explicit DanmuManager(int speed, Direction from, QWidget *parent = nullptr);

private slots:
    void calculateOffset(QSize);

private:
    static DanmuManager             *m_manager;
    QWidget                         *m_parent;
    int                             m_nextOffset;

    // Settings
    static QMap<settings, QVariant> m_settings; // 设置
    static bool                     isInitDefault; // 是否对未设置的内容初始化
};

#endif // DANMUMANAGER_H
