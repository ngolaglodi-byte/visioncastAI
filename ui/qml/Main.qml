// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// Main.qml — Root ApplicationWindow for the QML Broadcast Control Room.

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "panels"
import "components"

ApplicationWindow {
    id: root
    visible:    true
    width:      1920
    height:     1080
    minimumWidth:  1280
    minimumHeight: 720
    title: "VisionCast-AI — Broadcast Control Room"
    color: "#0D1117"

    // == Menu bar modernisé ==
    menuBar: MenuBar {
        background: Rectangle { color: "#161B22" }
        Menu {
            title: "File"
            Action { text: "Import Project..."; onTriggered: { /* TODO: bridge.importProject() */ } }
            Action { text: "Export Project..."; onTriggered: { /* TODO: bridge.exportProject() */ } }
            MenuSeparator {}
            Action { text: "Settings..."; onTriggered: settingsDialog.open() }
            Action { text: "Exit"; onTriggered: Qt.quit() }
        }
        Menu {
            title: "View"
            Action { text: "Show Monitoring"; checked: true; checkable: true
                onToggled: { bottomBar.visible = checked } }
            Action { text: "Dark/Light Theme"; checkable: true; checked: true
                onToggled: { /* TODO: Theme switch */ } }
        }
        Menu {
            title: "Broadcast"
            Action { text: "Start Engine"; onTriggered: bridge.startEngine() }
            Action { text: "Stop Engine";  onTriggered: bridge.stopEngine() }
            MenuSeparator {}
            Action { text: "Go Live";      onTriggered: bridge.goLive() }
            Action { text: "Stop";         onTriggered: bridge.stopBroadcast() }
        }
        Menu {
            title: "Help"
            Action { text: "Documentation"; onTriggered: { Qt.openUrlExternally("https://visioncast.prestige.tech/docs") } }
            Action { text: "About"; onTriggered: aboutDialog.open() }
        }
    }

    // ---- Notification toast ----
    property string _toastMsg:   ""
    property string _toastLevel: "info"

    Connections {
        target: bridge
        function onNotification(message, level) {
            root._toastMsg   = message
            root._toastLevel = level
            toastTimer.restart()
        }
    }

    // == Interface principale ==
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0.05,0.07,0.09,1)

        // TOP BAR (déjà moderne)
        Rectangle {
            id:     topBar
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 48
            color:  "#161B22"
            z:      10
            // TODO: tu peux déplacer ici le contenu du RowLayout/logo/statuts si tu veux le rendre sticky même si tu scrolles
            // .... [inchangé]
        }

        // MAIN
        RowLayout {
            anchors {
                top:    topBar.bottom
                bottom: bottomBar.top
                left:   parent.left
                right:  parent.right
            }
            spacing: 0

            // Left column
            SourcePanel {
                Layout.preferredWidth:  300
                Layout.minimumWidth:    220
                Layout.fillHeight:      true
            }
            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }

            // Center: Program/Preview
            ColumnLayout {
                Layout.fillWidth:  true
                Layout.fillHeight: true
                spacing: 0

                ProgramView {
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    Layout.preferredHeight: parent.height * 0.55
                    Layout.margins: 8
                }
                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#30363D" }
                PreviewView {
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    Layout.preferredHeight: parent.height * 0.45
                    Layout.margins: 8
                }
            }
            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }

            // Right: Tabbed panel
            Rectangle {
                Layout.preferredWidth: 360
                Layout.minimumWidth:   260
                Layout.fillHeight:     true
                color: "#161B22"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Tab bar/code inchangé
                    Rectangle {
                        Layout.fillWidth: true
                        height: 42
                        color:  "#1C2128"
                        // ... [inchangé]
                    }

                    // Tab contents inchangé
                    Item {
                        id:               rightPanel
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        property int currentTab: 0

                        TalentPanel {
                            anchors.fill: parent
                            visible:      rightPanel.currentTab === 0

                            Behavior on opacity { NumberAnimation { duration: 200 } }
                            opacity: visible ? 1.0 : 0.0
                        }
                        OverlayPanel {
                            anchors.fill: parent
                            visible:      rightPanel.currentTab === 1

                            Behavior on opacity { NumberAnimation { duration: 200 } }
                            opacity: visible ? 1.0 : 0.0
                        }
                        RecognitionPanel {
                            anchors.fill: parent
                            visible:      rightPanel.currentTab === 2

                            Behavior on opacity { NumberAnimation { duration: 200 } }
                            opacity: visible ? 1.0 : 0.0
                        }
                    }
                }
            }
        }

        // BOTTOM BAR
        Rectangle {
            id:     bottomBar
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
            height: 200
            color:  "#161B22"
            z:      5

            Rectangle { anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right; height: 1; color: "#30363D" }

            RowLayout {
                anchors { fill: parent }
                spacing: 0

                MonitoringPanel { Layout.fillWidth:  true; Layout.fillHeight: true }
                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }
                MultiStreamPanel { Layout.preferredWidth: 480; Layout.minimumWidth: 340; Layout.fillHeight: true }
                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }
                OutputPanel { Layout.preferredWidth: 300; Layout.fillHeight: true }
            }
        }
    } // end Rectangle

    // == TOAST notification ==
    Rectangle {
        id: toast
        visible:  opacity > 0
        opacity:  toastTimer.running ? 1.0 : 0.0
        anchors.bottom: bottomBar.top
        anchors.bottomMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        width:    toastText.implicitWidth + 32
        height:   36
        radius:   8
        color:    root._toastLevel === "error"   ? "#F85149"
                : root._toastLevel === "success"  ? "#3FB950"
                : root._toastLevel === "warning"  ? "#D29922"
                : root._toastLevel === "live"     ? "#F85149"
                : "#21262D"
        border.color: Qt.lighter(color, 1.3)
        border.width: 1
        z: 100
        Behavior on opacity { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
        Text {
            id:    toastText
            text:  root._toastMsg
            color: "#FFFFFF"
            font.pixelSize: 12
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            anchors.centerIn: parent
        }
    }
    Timer { id: toastTimer; interval: 3000 }

    // == DIALOGS ==
    Dialog {
        id:       aboutDialog
        title:    "About VisionCast-AI"
        modal:    true
        anchors.centerIn: parent
        width:    480
        height:   320
        background: Rectangle {
            color:  "#161B22"
            radius: 10
            border.color: "#30363D"
            border.width: 1
        }
        header: Rectangle {
            height: 48
            color:  "#1C2128"
            radius: 10
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left:   parent.left
                anchors.right:  parent.right
                height: 1
                color:  "#30363D"
            }
            Text {
                text:  "About VisionCast-AI"
                color: "#E6EDF3"
                font.pixelSize: 14
                font.weight:    Font.Bold
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        contentItem: Column {
            anchors.centerIn: parent
            spacing: 12

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 64; height: 64; radius: 12
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "#1F6FEB" }
                    GradientStop { position: 1.0; color: "#A855F7" }
                }
                Text { anchors.centerIn: parent; text: "VC"; color: "#FFFFFF"; font.pixelSize: 24; font.weight: Font.Bold }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "VisionCast-AI"
                color: "#E6EDF3"
                font.pixelSize: 18
                font.weight:    Font.Bold
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "Broadcast Control Room — QML Edition"
                color: "#8B949E"
                font.pixelSize: 12
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "Version 1.0.0"
                color: "#484F58"
                font.pixelSize: 11
            }

            Rectangle { width: 300; height: 1; color: "#30363D"; anchors.horizontalCenter: parent.horizontalCenter }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "Licence officielle Prestige Technologie Company\ndéveloppée par Glody Dimputu Ngola"
                color: "#8B949E"
                font.pixelSize: 11
                horizontalAlignment: Text.AlignHCenter
            }
        }
        footer: Rectangle {
            height: 52
            color: "transparent"
            VCButton {
                text: "Close"
                width: 80; height: 32
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                onClicked: aboutDialog.close()
            }
        }
    }

    // == Settings dialog placeholder ==
    Dialog {
        id: settingsDialog
        title: "Settings"
        modal: true
        anchors.centerIn: parent
        width: 420
        height: 320
        standardButtons: Dialog.Ok | Dialog.Cancel
        visible: false
        // TODO: Remplis avec des réglages réels si besoin
        contentItem: Column {
            anchors.centerIn: parent
            spacing: 16
            Text {
                text: "[Settings à venir...]"
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#8B949E"
            }
        }
    }
}