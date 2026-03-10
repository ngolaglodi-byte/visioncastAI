// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCSectionHeader.qml — Section title with uppercase lettering,
//                        optional right-side badge.

import QtQuick 2.15

Item {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property string title:      ""
    property string badgeText:  ""
    property color  badgeColor: "#1F6FEB"

    implicitHeight: 28
    implicitWidth:  titleText.implicitWidth + (badge.visible ? badge.width + 8 : 0)

    Text {
        id: titleText
        text:           root.title.toUpperCase()
        color:          "#8B949E"
        font.pixelSize: 11
        font.weight:    Font.DemiBold
        font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        font.letterSpacing: 1.0
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id:      badge
        visible: root.badgeText.length > 0
        width:   badgeLbl.implicitWidth + 10
        height:  18
        radius:  9
        color:   Qt.rgba(root.badgeColor.r, root.badgeColor.g, root.badgeColor.b, 0.2)
        border { color: root.badgeColor; width: 1 }
        anchors {
            left:           titleText.right
            leftMargin:     8
            verticalCenter: parent.verticalCenter
        }

        Text {
            id:             badgeLbl
            text:           root.badgeText
            color:          root.badgeColor
            font.pixelSize: 10
            font.weight:    Font.DemiBold
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            anchors.centerIn: parent
        }
    }

    // Bottom rule
    Rectangle {
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: 1
        color:  "#30363D"
    }
}
