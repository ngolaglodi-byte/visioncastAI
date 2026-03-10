// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCMetricGauge.qml — Compact horizontal bar gauge for system metrics.

import QtQuick 2.15

Item {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property string label:     "CPU"
    property real   value:     0.0     // 0–100
    property string unit:      "%"
    property color  barColor:  "#1F6FEB"

    implicitWidth:  200
    implicitHeight: 42

    // Label + value text
    Item {
        id:     header
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: labelText.implicitHeight

        Text {
            id:             labelText
            text:           root.label
            color:          "#8B949E"
            font.pixelSize: 11
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            font.letterSpacing: 0.8
            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
        }
        Text {
            text:           root.value.toFixed(0) + root.unit
            color:          "#E6EDF3"
            font.pixelSize: 11
            font.weight:    Font.DemiBold
            font.family:    "\"JetBrains Mono\", \"Cascadia Code\", monospace"
            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
        }
    }

    // Track
    Rectangle {
        id:     track
        anchors { top: header.bottom; topMargin: 4; left: parent.left; right: parent.right }
        height: 6
        radius: 3
        color:  "#21262D"

        // Fill
        Rectangle {
            id:     fill
            height: parent.height
            width:  track.width * Math.min(root.value / 100, 1.0)
            radius: parent.radius
            color: {
                if (root.value > 85) return "#F85149"
                if (root.value > 65) return "#D29922"
                return root.barColor
            }
            Behavior on width { NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
            Behavior on color { ColorAnimation  { duration: 200 } }
        }
    }
}
