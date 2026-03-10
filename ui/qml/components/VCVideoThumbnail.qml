// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCVideoThumbnail.qml — Video source thumbnail tile with selection
//                         glow border and hover animation.

import QtQuick 2.15

Rectangle {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property string sourceName:  ""
    property string deviceType:  ""
    property string thumbnailUrl: ""
    property bool   selected:    false
    signal clicked()

    implicitWidth:  280
    implicitHeight: 72

    radius:       8
    color:        selected ? "#1F6FEB1A" : (hoverArea.containsMouse ? "#1C2128" : "#161B22")
    border.color: selected ? "#1F6FEB"   : (hoverArea.containsMouse ? "#484F58"  : "#30363D")
    border.width: selected ? 2 : 1

    Behavior on color        { ColorAnimation { duration: 150 } }
    Behavior on border.color { ColorAnimation { duration: 150 } }

    // Glow animation on selection
    SequentialAnimation on border.width {
        running: root.selected
        loops:   1
        NumberAnimation { to: 3;  duration: 120 }
        NumberAnimation { to: 2;  duration: 120 }
    }

    // Thumbnail / placeholder
    Rectangle {
        id:     thumb
        width:  108; height: 60
        radius: 6
        color:  "#0D1117"
        anchors { left: parent.left; leftMargin: 6; verticalCenter: parent.verticalCenter }

        // Device-type icon as placeholder
        Text {
            anchors.centerIn: parent
            text:  root.deviceType === "NDI"     ? "📡"
                 : root.deviceType === "DeckLink" ? "🎬"
                 : root.deviceType === "AJA"      ? "📹"
                 : root.deviceType === "Virtual"  ? "🖥️"
                 : "📷"
            font.pixelSize: 22
            opacity: 0.7
        }

        Image {
            anchors.fill: parent
            source:       root.thumbnailUrl
            visible:      root.thumbnailUrl.length > 0
            fillMode:     Image.PreserveAspectCrop
        }
    }

    // Labels
    Column {
        anchors {
            left:    thumb.right
            leftMargin: 10
            right:   parent.right
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        spacing: 4

        Text {
            text:           root.sourceName
            color:          root.selected ? "#E6EDF3" : "#C9D1D9"
            font.pixelSize: 12
            font.weight:    Font.DemiBold
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            elide:          Text.ElideRight
            width:          parent.width
        }
        Text {
            text:           root.deviceType
            color:          "#8B949E"
            font.pixelSize: 10
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        }
    }

    // Scale on hover
    transform: Scale {
        origin.x: root.width  / 2
        origin.y: root.height / 2
        xScale: hoverArea.containsMouse ? 1.01 : 1.0
        yScale: xScale
        Behavior on xScale { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
    }

    MouseArea {
        id:           hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape:  Qt.PointingHandCursor
        onClicked:    root.clicked()
    }
}
