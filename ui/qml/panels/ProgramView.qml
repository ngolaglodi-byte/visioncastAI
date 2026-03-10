// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// ProgramView.qml — Main program output display.
//                    Red border + LIVE indicator when on air.

import QtQuick 2.15
import "../components"

Rectangle {
    id: root
    color:         "#0D1117"
    border.color:  bridge.isLive ? "#F85149" : "#30363D"
    border.width:  bridge.isLive ? 3 : 1
    radius:        4

    Behavior on border.color { ColorAnimation { duration: 200 } }
    Behavior on border.width { NumberAnimation { duration: 200 } }

    // ── LIVE badge ─────────────────────────────────────────────────
    Rectangle {
        visible: bridge.isLive
        anchors { top: parent.top; topMargin: -1; left: parent.left; leftMargin: 12 }
        width:   liveLbl.implicitWidth + 16
        height:  24
        radius:  4
        color:   "#F85149"
        z:       2

        Row {
            anchors.centerIn: parent
            spacing: 6

            Rectangle {
                width: 8; height: 8; radius: 4
                color: "#FFFFFF"
                anchors.verticalCenter: parent.verticalCenter

                SequentialAnimation on opacity {
                    running: bridge.isLive
                    loops:   Animation.Infinite
                    NumberAnimation { to: 0.2; duration: 500; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 1.0; duration: 500; easing.type: Easing.InOutSine }
                }
            }

            Text {
                id:    liveLbl
                text:  "LIVE"
                color: "#FFFFFF"
                font.pixelSize: 11
                font.weight:    Font.Bold
                font.letterSpacing: 1.5
            }
        }
    }

    // ── Placeholder when engine is off ─────────────────────────────
    Column {
        visible:          !bridge.engineRunning
        anchors.centerIn: parent
        spacing:          12

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  "🎬"
            font.pixelSize: 48
            opacity: 0.3
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  "PROGRAM OUTPUT"
            color: "#484F58"
            font.pixelSize: 14
            font.weight:    Font.DemiBold
            font.letterSpacing: 2.0
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  "Start the engine to begin"
            color: "#30363D"
            font.pixelSize: 11
        }
    }

    // ── Simulated video feed placeholder (engine running) ─────────
    Rectangle {
        visible:       bridge.engineRunning
        anchors.fill:  parent
        color:         "transparent"

        // Scanline effect placeholder
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                orientation: Gradient.Vertical
                GradientStop { position: 0.0; color: "#0D1117" }
                GradientStop { position: 0.5; color: "#111820" }
                GradientStop { position: 1.0; color: "#0D1117" }
            }
        }

        // Center "ON AIR" text
        Column {
            anchors.centerIn: parent
            spacing: 8
            visible: !bridge.isLive

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "ENGINE RUNNING"
                color: "#3FB950"
                font.pixelSize: 13
                font.weight:    Font.DemiBold
                font.letterSpacing: 2.0
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  bridge.fps + " fps"
                color: "#8B949E"
                font.pixelSize: 11
                font.family:    "JetBrains Mono, Cascadia Code, monospace"
            }
        }
    }

    // ── FPS counter ────────────────────────────────────────────────
    Text {
        visible:  bridge.engineRunning
        text:     bridge.fps + " fps"
        color:    "#3FB950"
        font.pixelSize: 10
        font.family:    "JetBrains Mono, Cascadia Code, monospace"
        anchors { bottom: parent.bottom; right: parent.right; margins: 8 }
    }
}
