import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Window {
    id: mainWindow
    width: minimumWidth * zoom
    height: minimumHeight * zoom
    minimumWidth: 600
    minimumHeight: 400
    visible: true
    title: qsTr("BilibiliDMB")

    property int zoom: 1
    property int themeRadius: 8
    property string themeColor: "deeppink"

    RowLayout {
        id: roomidRow
        anchors.horizontalCenter: parent.horizontalCenter
        y: 10 * zoom
        
        height: 32 * zoom
        spacing: 10 * zoom
        
        Text {
            id: roomidLabel
            width: height * 3
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            text: "直播间："
            font.pixelSize: parent.height * 0.8
            color: themeColor
        }

        Rectangle {
            id: roomidRect
            width: 200 * zoom
            height: parent.height
            radius:themeRadius
            border.color: themeColor
            border.width: 2 * zoom

            TextInput {
                id: roomid
                x: height * 0.2 + roomidRect.border.width
                width: parent.width
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                selectByMouse: true
                font.pixelSize: parent.height * 0.5
            }
        }

        Rectangle {
            id: connectButtonRect
            width: height * 2
            height: parent.height
            radius: themeRadius
            color: themeColor

            Text {
                id: connectButtonText
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "连接"
                font.pixelSize: parent.height * 0.67
                color: "#FFFFFF"
            }

            MouseArea {
                id: connectButtonArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    if (connectButtonText.text == "连接") {
                        console.log("正在连接" + roomid.text + "...")
                        connectButtonText.text = "断开"
                    } else {
                        console.log("连接已断开")
                        connectButtonText.text = "连接"
                    }
                    // 创建新弹幕
                    var danmuView = Qt.createComponent("DanmuView.qml")
                    danmuView.createObject(danmuLayout, 
                    {
                        danmuTime: "2022-2-22",
                        danmuUser: "Test",
                        danmuContent: "我是一条动态弹幕",
                        danmuColor: "#1fd2ef",
                        medalName: "粉丝牌",
                        medalLevel: "20",
                        medalColor: "#ba214d",
                        fontSize: 20
                    })
                }
            }
        }
    }

    

    ScrollView {
        x: spacing
        y: roomidRow.y + roomidRow.height + spacing
        width: parent.width - spacing * 2
        height: parent.height - y - spacing
        spacing: 10 * zoom

        ColumnLayout {
            id: danmuLayout
            anchors.fill: parent
            DanmuView {
                danmuTime: "2022-2-22"
                danmuUser: "Test"
                danmuContent: "我是一条弹幕"
                danmuColor: "#FF0000"
                medalName: "粉丝牌"
                medalLevel: "20"
                medalColor: "#ab31dd"
                fontSize: 30
            }
            DanmuView {
                danmuTime: "2022-2-22"
                danmuUser: "Test"
                danmuContent: "我是一条弹幕"
                danmuColor: "#00FF00"
                medalName: "粉丝"
                medalLevel: "20"
                medalColor: "#AABBCC"
                fontSize: 30
            }
        }
        
    }
}

