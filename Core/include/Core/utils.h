/**
 * @file utils.h
 * @author Ccslykx (ccslykx@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

void changeHeaderSize (QByteArray *data, int num, bool plus)
{
    QByteArray lenData = data->mid(0, 4);
    quint32 len;
    QDataStream readStream(&lenData, QIODevice::ReadOnly);
    readStream >> len;
    QDataStream writeStream(&lenData, QIODevice::WriteOnly);
    writeStream << quint32(len + (plus?num:-num));
    data->replace(0, 4, lenData);
}