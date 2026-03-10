// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCToggleSwitch.qml — Modern toggle switch with label and animated knob.

import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property bool   checked:   false
    property string label:     ""
    property bool   enabled:   true
    signal toggled(bool checked)

    implicitWidth:  labelText.visible ? labelText.implicitWidth + 52 : 48
    implicitHeight: 28

    opacity: root.enabled ? 1.0 : 0.5

    // Label
    Text {
        id:       labelText
        visible:  root.label.length > 0
        text:     root.label
        color:    "#E6EDF3"
        font.pixelSize: 12
        font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        anchors { left: parent.left; verticalCenter: parent.verticalCenter }
    }

    // Track
    Rectangle {
        id:     track
        width:  44; height: 24
        radius: 12
        color:  root.checked ? "#1F6FEB" : "#30363D"
        anchors {
            right:          parent.right
            verticalCenter: parent.verticalCenter
        }
        Behavior on color { ColorAnimation { duration: 200 } }

        // Knob
        Rectangle {
            id:     knob
            width:  18; height: 18
            radius: 9
            color:  "#FFFFFF"
            anchors.verticalCenter: parent.verticalCenter
            x: root.checked ? track.width - width - 3 : 3
            Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

            // Subtle shadow
            layer.enabled: true
            layer.effect: null
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled:      root.enabled
        cursorShape:  Qt.PointingHandCursor
        onClicked: {
            root.checked = !root.checked
            root.toggled(root.checked)
        }
    }

    // Keyboard
    Keys.onSpacePressed:  { if (root.enabled) { root.checked = !root.checked; root.toggled(root.checked) } }
    Keys.onReturnPressed: { if (root.enabled) { root.checked = !root.checked; root.toggled(root.checked) } }
}
