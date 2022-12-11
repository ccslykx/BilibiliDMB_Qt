import QtQuick
import QtQuick.Layouts

Rectangle {
    id: danmuView
    height: fontSize * 10 / 9 // 使用 ColumnLayout ，需要 height
    
    RowLayout {
        spacing: fontSize * 0.5

        // 弹幕
        Text {
            id: danmuTimeText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: danmuColor
            font.pixelSize: fontSize
            text: danmuTime
        }
        Text {
            id: danmuUserText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: danmuColor
            font.pixelSize: fontSize
            text: danmuUser
        }
        Text {
            id: danmuContentText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: danmuColor
            font.pixelSize: fontSize
            text: danmuContent
        }
    }
}