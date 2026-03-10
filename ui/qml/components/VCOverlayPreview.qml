// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCOverlayPreview.qml — Overlay template preview tile with name,
//                          type tag, and active toggle.

import QtQuick 2.15

Rectangle {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property string overlayId:  ""
    property string name:       ""
    property string type:       ""
    property bool   active:     false
    signal toggleRequested(string overlayId)

    implicitWidth:  parent ? parent.width : 300
    implicitHeight: 56

    radius:       8
    color:        active ? "#3FB9501A" : (hover.containsMouse ? "#1C2128" : "#161B22")
    border.color: active ? "#3FB950"   : "#30363D"
    border.width: active ? 2 : 1

    Behavior on color        { ColorAnimation { duration: 150 } }
    Behavior on border.color { ColorAnimation { duration: 150 } }

    // Type icon
    Text {
        id: typeIcon
        anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
        text: root.type === "lower_third" ? "⬇️"
            : root.type === "ticker"      ? "📰"
            : root.type === "score_bug"   ? "⚽"
            : root.type === "bug"         ? "🔴"
            : root.type === "transition"  ? "✨"
            : "🎞️"
        font.pixelSize: 18
    }

    // Name + type
    Column {
        anchors {
            left:    typeIcon.right
            leftMargin: 10
            right:   toggleBtn.left
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        spacing: 3

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
            text:           root.type.replace(/_/g, " ").toUpperCase()
            color:          "#8B949E"
            font.pixelSize: 9
            font.letterSpacing: 0.8
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        }
    }

    // Toggle button
    Rectangle {
        id:     toggleBtn
        width:  40; height: 22
        radius: 11
        color:  root.active ? "#3FB950" : "#30363D"
        anchors { right: parent.right; rightMargin: 12; verticalCenter: parent.verticalCenter }

        Behavior on color { ColorAnimation { duration: 200 } }

        Rectangle {
            id:     knob
            width:  16; height: 16
            radius: 8
            color:  "#FFFFFF"
            anchors.verticalCenter: parent.verticalCenter
            x: root.active ? parent.width - width - 3 : 3
            Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape:  Qt.PointingHandCursor
            onClicked:    root.toggleRequested(root.overlayId)
        }
    }

    MouseArea {
        id:           hover
        anchors.fill: parent
        hoverEnabled: true
        onClicked:    root.toggleRequested(root.overlayId)
    }
}
