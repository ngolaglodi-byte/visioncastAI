// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCStatusIndicator.qml — Pulsing LED indicator for engine/AI status.
//   status: "active"   → green  (pulsing)
//           "live"     → red    (pulsing fast)
//           "warning"  → amber  (slow pulse)
//           "inactive" → grey   (static)

import QtQuick 2.15

Item {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property string status:  "inactive"   // "active"|"live"|"warning"|"inactive"
    property string label:   ""
    property int    dotSize: 10

    implicitWidth:  dotSize + (label.length > 0 ? labelText.implicitWidth + 6 : 0)
    implicitHeight: Math.max(dotSize, label.length > 0 ? labelText.implicitHeight : dotSize)

    // ── Dot ───────────────────────────────────────────────────────
    Rectangle {
        id: dot
        width:  root.dotSize
        height: root.dotSize
        radius: root.dotSize / 2
        anchors.verticalCenter: parent.verticalCenter
        color: _dotColor

        // Glow ring
        Rectangle {
            id: glowRing
            anchors.centerIn: parent
            width:  parent.width  + 4
            height: parent.height + 4
            radius: width / 2
            color:  "transparent"
            border { color: Qt.rgba(_rgb.r, _rgb.g, _rgb.b, glowOpacity); width: 2 }
            opacity: 1
            property real glowOpacity: 0.0
        }

        SequentialAnimation {
            running:  root.status !== "inactive"
            loops:    Animation.Infinite
            NumberAnimation {
                target:   glowRing
                property: "glowOpacity"
                to:       0.8
                duration: _pulseDuration
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                target:   glowRing
                property: "glowOpacity"
                to:       0.0
                duration: _pulseDuration
                easing.type: Easing.InOutSine
            }
        }
    }

    // ── Optional label ─────────────────────────────────────────────
    Text {
        id:             labelText
        visible:        root.label.length > 0
        text:           root.label
        color:          _dotColor
        font.pixelSize: 11
        font.weight:    Font.DemiBold
        font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
        anchors {
            left:           dot.right
            leftMargin:     6
            verticalCenter: parent.verticalCenter
        }
    }

    // ── Colour helpers ─────────────────────────────────────────────
    readonly property color _dotColor: status === "active"  ? "#3FB950"
                                     : status === "live"    ? "#F85149"
                                     : status === "warning" ? "#D29922"
                                     : "#484F58"
    readonly property int _pulseDuration: status === "live" ? 600 : 900

    // rgb decomposition for glow ring (keep in sync with _dotColor)
    readonly property color _rgb: _dotColor
}
