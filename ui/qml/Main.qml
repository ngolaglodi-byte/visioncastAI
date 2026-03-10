// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// Main.qml — Root ApplicationWindow for the QML Broadcast Control Room.
//             Broadcast layout: TopBar | [SourcePanel | Center | RightPanel]
//                                      | BottomBar

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

    // ── Notification toast ─────────────────────────────────────────
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

    // ═══════════════════════════════════════════════════════════════
    // TOP BAR (48 px)
    // ═══════════════════════════════════════════════════════════════
    Rectangle {
        id:     topBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 48
        color:  "#161B22"
        z:      10

        // Bottom border
        Rectangle {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
            height: 1; color: "#30363D"
        }

        RowLayout {
            anchors { fill: parent; leftMargin: 16; rightMargin: 16 }
            spacing: 16

            // Logo
            Row {
                spacing: 10
                Rectangle {
                    width: 28; height: 28; radius: 6
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "#1F6FEB" }
                        GradientStop { position: 1.0; color: "#A855F7" }
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "VC"
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        font.weight:    Font.Bold
                    }
                }
                Text {
                    text:  "VisionCast-AI"
                    color: "#E6EDF3"
                    font.pixelSize: 15
                    font.weight:    Font.Bold
                    font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            // Status indicators
            Row {
                spacing: 20
                anchors.verticalCenter: parent.verticalCenter

                VCStatusIndicator {
                    status: bridge.engineRunning ? "active" : "inactive"
                    label:  "ENGINE"
                    dotSize: 10
                }
                VCStatusIndicator {
                    status: bridge.aiConnected ? "active" : "inactive"
                    label:  "AI"
                    dotSize: 10
                }
                VCStatusIndicator {
                    status: bridge.isLive ? "live" : "inactive"
                    label:  bridge.isLive ? "ON AIR" : "OFF AIR"
                    dotSize: 10
                }

                // FPS display
                Text {
                    visible: bridge.engineRunning
                    text:    bridge.fps + " fps"
                    color:   "#8B949E"
                    font.pixelSize: 11
                    font.family:    "JetBrains Mono, Cascadia Code, monospace"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Item { Layout.fillWidth: true }   // spacer

            // Broadcast controls
            Row {
                spacing: 8
                anchors.verticalCenter: parent.verticalCenter

                VCButton {
                    text:    bridge.engineRunning ? "Stop Engine" : "Start Engine"
                    variant: bridge.engineRunning ? "default" : "primary"
                    width:   110; height: 32
                    onClicked: bridge.engineRunning ? bridge.stopEngine() : bridge.startEngine()
                }

                VCButton {
                    text:    bridge.isLive ? "⏹  Stop" : "🔴  Go Live"
                    variant: bridge.isLive ? "danger" : "primary"
                    width:   100; height: 32
                    enabled: bridge.engineRunning
                    onClicked: bridge.isLive ? bridge.stopBroadcast() : bridge.goLive()
                }

                VCButton {
                    text:    "About"
                    width:   70; height: 32
                    onClicked: aboutDialog.open()
                }
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // MAIN CONTENT AREA
    // ═══════════════════════════════════════════════════════════════
    RowLayout {
        anchors {
            top:    topBar.bottom
            bottom: bottomBar.top
            left:   parent.left
            right:  parent.right
        }
        spacing: 0

        // ── Left column: Source panel ───────────────────────────────
        SourcePanel {
            Layout.preferredWidth:  300
            Layout.minimumWidth:    220
            Layout.fillHeight:      true
        }

        // Column separator
        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }

        // ── Center: Program (top) + Preview (bottom) ────────────────
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

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#30363D"
            }

            PreviewView {
                Layout.fillWidth:  true
                Layout.fillHeight: true
                Layout.preferredHeight: parent.height * 0.45
                Layout.margins: 8
            }
        }

        // Column separator
        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }

        // ── Right column: Tabbed panel ──────────────────────────────
        Rectangle {
            Layout.preferredWidth: 360
            Layout.minimumWidth:   260
            Layout.fillHeight:     true
            color: "#161B22"

            ColumnLayout {
                anchors.fill: parent
                spacing:      0

                // Tab bar
                Rectangle {
                    Layout.fillWidth: true
                    height: 42
                    color:  "#1C2128"

                    Row {
                        anchors { left: parent.left; bottom: parent.bottom }
                        spacing: 0

                        Repeater {
                            model: ["Talent", "Overlays", "AI Recog."]
                            delegate: Rectangle {
                                width:  80; height: 42
                                color:  rightPanel.currentTab === index ? "#21262D" : "transparent"
                                border.bottom: rightPanel.currentTab === index ? 2 : 0

                                Rectangle {
                                    visible: rightPanel.currentTab === index
                                    anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                                    height: 2
                                    color:  "#1F6FEB"
                                }

                                Text {
                                    anchors.centerIn: parent
                                    text:  modelData
                                    color: rightPanel.currentTab === index ? "#1F6FEB" : "#8B949E"
                                    font.pixelSize: 11
                                    font.weight:    rightPanel.currentTab === index ? Font.DemiBold : Font.Normal
                                    font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape:  Qt.PointingHandCursor
                                    onClicked:    rightPanel.currentTab = index
                                }
                            }
                        }
                    }
                }

                // Tab content
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

    // ═══════════════════════════════════════════════════════════════
    // BOTTOM BAR (~200 px): Monitoring + Output
    // ═══════════════════════════════════════════════════════════════
    Rectangle {
        id:     bottomBar
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        height: 200
        color:  "#161B22"
        z:      5

        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 1; color: "#30363D"
        }

        RowLayout {
            anchors { fill: parent }
            spacing: 0

            MonitoringPanel {
                Layout.fillWidth:  true
                Layout.fillHeight: true
            }

            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#30363D" }

            OutputPanel {
                Layout.preferredWidth: 420
                Layout.fillHeight:     true
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // TOAST NOTIFICATION
    // ═══════════════════════════════════════════════════════════════
    Rectangle {
        id: toast
        visible:  opacity > 0
        opacity:  toastTimer.running ? 1.0 : 0.0
        anchors { bottom: bottomBar.top; bottomMargin: 16; horizontalCenter: parent.horizontalCenter }
        width:    toastText.implicitWidth + 32
        height:   36
        radius:   8
        color:    root._toastLevel === "error"   ? "#F85149"
                : root._toastLevel === "success"  ? "#3FB950"
                : root._toastLevel === "warning"  ? "#D29922"
                : root._toastLevel === "live"     ? "#F85149"
                : "#21262D"
        border { color: Qt.lighter(color, 1.3); width: 1 }
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

    Timer {
        id:       toastTimer
        interval: 3000
    }

    // ═══════════════════════════════════════════════════════════════
    // ABOUT DIALOG
    // ═══════════════════════════════════════════════════════════════
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
            border { color: "#30363D"; width: 1 }
        }

        header: Rectangle {
            height: 48
            color:  "#1C2128"
            radius: 10
            Rectangle { anchors { bottom: parent.bottom; left: parent.left; right: parent.right }; height: 1; color: "#30363D" }
            Text {
                text:  "About VisionCast-AI"
                color: "#E6EDF3"
                font.pixelSize: 14
                font.weight:    Font.Bold
                anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
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
                anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
                onClicked: aboutDialog.close()
            }
        }
    }
}
