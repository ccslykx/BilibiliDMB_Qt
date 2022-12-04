/**
 * @file main.cpp
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief Bilibili test
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/**
 * USAGE:
 * Bilibili_test roomid Display
*/


#include <QCoreApplication>

#include <Core/Bilibili.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DanmuCore::Bilibili bili( DanmuCore::Display(reinterpret_cast<quint64>(argv[2])) );
    bili.connect(argv[1]);
    // 25836241 Xin_Q

    return a.exec();
}