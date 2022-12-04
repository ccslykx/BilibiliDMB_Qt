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

    property int zoom: 3
    property int themeRadius: 8
    property string themeColor: "#FC88A9" // B站粉

    property var danmuView: Qt.createComponent("DanmuView.qml")

    signal connect(string roomid)
    signal disconnect()

    // 创建新弹幕
    function createDanmu(time, usr, content, danmu_color, 
                         medal_name, medal_level, medal_color, size = 20) {                
        danmuView.createObject(danmuLayout, 
        {
            danmuTime: time,
            danmuUser: usr,
            danmuContent: content, 
            danmuColor: danmu_color,
            medalName: medal_name,
            medalLevel: medal_level,
            medalColor: medal_color,
            fontSize: size
        })
    }

    Connections {
        target: Bilibili
        // 礼物被视作特殊的“弹幕”出现，共用一个信号
        onNewDanmu: (time, usr, content, danmu_color, medal_name, medal_level, medal_color) => {
            createDanmu(time, usr, content, danmu_color, medal_name, medal_level, medal_color, 20 * zoom)
            danmuScroll.ScrollBar.vertical.position = 1
        }
        onNewEntry: (time, usr, medal_name, medal_level, medal_color) => {
            createDanmu(time, usr, "进入直播间", themeColor, medal_name, medal_level, medal_color, 20 * zoom)
            danmuScroll.ScrollBar.vertical.position = 1
        }
    }


    // 直播间信息行
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
        // ”连接“按钮
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
                        mainWindow.connect(roomid.text)
                        createDanmu("", "系统提示", "连接成功", themeColor, "系统", "0", themeColor, 20 * zoom)
                        connectButtonText.text = "断开"
                    } else {
                        mainWindow.disconnect()
                        console.log("连接已断开")
                        createDanmu("", "系统提示", "断开连接", themeColor, "系统", "0", themeColor, 20 * zoom)
                        connectButtonText.text = "连接"
                    }
                }
            }
        }
    }

    // 弹幕显示区
    ScrollView {
        id: danmuScroll
        x: spacing
        y: roomidRow.y + roomidRow.height + spacing
        width: parent.width - spacing * 2
        height: parent.height - y - spacing
        spacing: 10 * zoom

        ColumnLayout {
            id: danmuLayout
            anchors.fill: parent
        }
    }
}

