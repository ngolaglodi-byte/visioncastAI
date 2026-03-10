// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// PreviewView.qml — Preview output display.
//                    Green border = preview channel.

import QtQuick 2.15

Rectangle {
    id: root
    color:        "#0D1117"
    border.color: "#30363D"
    border.width: 1
    radius:       4

    // ── PREVIEW badge ──────────────────────────────────────────────
    Rectangle {
        anchors { top: parent.top; topMargin: -1; left: parent.left; leftMargin: 12 }
        width:   pvwLbl.implicitWidth + 16
        height:  22
        radius:  4
        color:   "#3FB950"
        z:       2

        Text {
            id:    pvwLbl
            text:  "PVW"
            color: "#FFFFFF"
            font.pixelSize: 10
            font.weight:    Font.Bold
            font.letterSpacing: 1.5
            anchors.centerIn: parent
        }
    }

    // ── Green border accent ────────────────────────────────────────
    Rectangle {
        anchors { fill: parent; margins: -1 }
        color:        "transparent"
        border.color: "#3FB95033"
        border.width: 3
        radius:       root.radius + 1
        z:            -1
    }

    // ── Placeholder ────────────────────────────────────────────────
    Column {
        anchors.centerIn: parent
        spacing: 10
        opacity: 0.35

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  "📺"
            font.pixelSize: 32
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  "PREVIEW"
            color: "#8B949E"
            font.pixelSize: 11
            font.weight:    Font.DemiBold
            font.letterSpacing: 2.0
        }
    }
}
