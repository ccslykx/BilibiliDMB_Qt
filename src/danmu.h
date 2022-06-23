/**
 * @file danmu.h
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 基于Qt的弹幕类
 * @version 0.1
 * @date 2022-06-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef DANMU_H
#define DANMU_H

#include "Danmu_global.h"

#include <QLabel>
#include <QString>
#include <QUuid>
#include <QPropertyAnimation>

#include <any>

/**
 * @brief 弹幕的方向
 * 
 */
enum Direction {
    TopToBottom = 0,
    RightToLeft = 1,
    BottomToTop = 2,
    LeftToRight = 3
};

enum settings {
    screenWidth = 0, // int
    screenHeight, // int
    speed, // int
    direction, // enum Direction
    font, // QFont
    color // QPalette
};

class DANMU_EXPORT Danmu : public QLabel
{
    Q_OBJECT
public:
    /**
     * @brief 创建弹幕的方法（不可用常规方法创建）
     * 
     * @param dm 弹幕内容
     * @param offset 弹幕位置偏移量
     * @param speed 弹幕的速度
     * @param from 弹幕开始出现的位置
     * @return Danmu* 
     */
    static Danmu        *create(QString dm, int offset, QWidget *parent = nullptr);
    static Danmu        *create(QString dm, int offset, int speed, Direction from, QWidget *parent = nullptr);

    /**
     * @brief 返回当前屏幕上现存弹幕总数
     * 
     * @return int 
     */
    static int          dmCount();

    QSize               dmSize();

    /**
     * @brief UUID，暂时没用到
     * 
     * @return QUuid 
     */
    QUuid               uuid();

    /**
     * @brief 属性设置
     * 
     */
    static void         set(settings, QVariant);
    static void         setSettings(QMap<settings, QVariant>&);
    static void         loadDefaultSettings();

public slots:
    /**
     * @brief 析构当前弹幕（由Danmu类自己管理生命周期）
     * 
     */
    void                destory();
    
protected:
    /**
     * @brief Construct a new Danmu object，参数说明详见create函数
     * 
     * @param dm 
     * @param offset 
     * @param speed 
     * @param from 
     */
    Danmu(QString dm, int offset, int speed, Direction from, QWidget *parent = nullptr);
    ~Danmu();

private:
    /**
     * @brief 初始化
     * 
     */
    void                init();

private:
    static int          m_count;        // 当前屏幕上现存弹幕总数
    QPropertyAnimation  *animation;     // 绘制动画
    QUuid               m_uuid;         // UUID，暂时没有用到
    QString             m_dm;           // 弹幕内容
    int                 m_offset;       // 弹幕出现位置的偏移量

    static QMap<settings, QVariant> m_settings; // 设置
    static bool                     isInitDefault; // 是否对未设置的内容初始化
};

#endif // DANMU_H
