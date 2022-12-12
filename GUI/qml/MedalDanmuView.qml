import QtQuick
import QtQuick.Layouts

Rectangle {
    id: danmuView
    height: fontSize * 10 / 9 // 使用 ColumnLayout ，需要 height
    
    RowLayout {
        spacing: fontSize * 0.5
        // 时间
        Text {
            id: danmuTimeText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: danmuColor
            font.pixelSize: fontSize
            text: danmuTime
        }
        // 粉丝牌
        Rectangle {
            id: medalView
            // 粉丝牌宽度                  + 边界宽度(0.25 + 0.05) + 粉丝牌等级宽度
            width: medalNameText.contentWidth + fontSize * 0.3 + medalLevelRect.width 
            height: fontSize * 10 / 8
            radius: 4
            color: medalColor
            Text {
                id: medalNameText
                x: height * 0.1                         // 左边界
                width: contentWidth                     // 根据字长定宽
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#FFFFFF"
                font.pixelSize: fontSize
                text: medalName
            }
            Rectangle{
                id:medalLevelRect
                x: parent.width - y - width             // 从右数，用上边界做为右边界
                y: parent.height * 0.05                 // 上边界
                width: parent.height * 1.2
                height: parent.height * 0.9
                radius: parent.radius
                color: "#FFFFFF"
                Text {
                    id: medalLevelText
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: medalColor
                    font.pixelSize: fontSize
                    text: medalLevel
                }
            }
        }
        // 弹幕
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