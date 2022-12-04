import QtQuick
import QtQuick.Layouts

Rectangle {
    id: danmuView
    height: fontSize * 10 / 9 // 使用 ColumnLayout ，需要 height
    
    property int fontSize

    property string danmuTime
    property string danmuUser
    property string danmuContent
    property string danmuColor

    property string medalName
    property string medalLevel
    property string medalColor
    
    RowLayout {
        spacing: fontSize * 0.5

        MedalView { 
            fontSize: danmuView.fontSize * 0.8
            name: medalName
            level: medalLevel
            color: medalColor
        }
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