// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCTalentCard.qml — Compact talent info card showing name, role,
//                     and overlay active status.

import QtQuick 2.15

Rectangle {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property string talentId:     ""
    property string name:         ""
    property string role:         ""
    property bool   overlayActive: false
    property bool   selected:     false
    signal clicked()

    implicitWidth:  parent ? parent.width : 300
    implicitHeight: 64

    radius:       8
    color:        selected ? "#1F6FEB1A" : (hoverArea.containsMouse ? "#1C2128" : "#161B22")
    border.color: selected ? "#1F6FEB"   : "#30363D"
    border.width: selected ? 2 : 1

    Behavior on color        { ColorAnimation { duration: 150 } }
    Behavior on border.color { ColorAnimation { duration: 150 } }

    // Avatar placeholder
    Rectangle {
        id:     avatar
        width:  42; height: 42
        radius: 21
        color:  "#21262D"
        border { color: "#30363D"; width: 1 }
        anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }

        Text {
            anchors.centerIn: parent
            text:  root.name.length > 0 ? root.name.charAt(0).toUpperCase() : "?"
            color: "#58A6FF"
            font.pixelSize: 18
            font.weight:    Font.Bold
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        }
    }

    // Labels
    Column {
        anchors {
            left:    avatar.right
            leftMargin: 10
            right:   overlayDot.left
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        spacing: 4

        Text {
            text:           root.name
            color:          "#E6EDF3"
            font.pixelSize: 12
            font.weight:    Font.DemiBold
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            elide:          Text.ElideRight
            width:          parent.width
        }
        Text {
            text:           root.role
            color:          "#8B949E"
            font.pixelSize: 10
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        }
    }

    // Overlay active indicator
    VCStatusIndicator {
        id:           overlayDot
        status:       root.overlayActive ? "active" : "inactive"
        dotSize:      8
        anchors { right: parent.right; rightMargin: 12; verticalCenter: parent.verticalCenter }
    }

    MouseArea {
        id:           hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape:  Qt.PointingHandCursor
        onClicked:    root.clicked()
    }
}
