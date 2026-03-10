// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCButton.qml — Branded button with hover/press animations and
//                three visual variants: "default", "primary", "danger".

import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    /// "default" | "primary" | "danger"
    property string variant: "default"

    // ── Internals ──────────────────────────────────────────────────
    implicitHeight: 34
    implicitWidth:  contentItem.implicitWidth + 32
    focusPolicy:    Qt.TabFocus

    // ── Visual ────────────────────────────────────────────────────
    background: Rectangle {
        id: bg
        radius: 6
        color: {
            if (!root.enabled)                      return "#161B22"
            if (root.pressed)                       return _pressColor
            if (root.hovered)                       return _hoverColor
            return _baseColor
        }
        border.color: {
            if (!root.enabled)  return "#21262D"
            if (root.pressed || root.hovered) return _hoverBorder
            return _baseBorder
        }
        border.width: 1

        Behavior on color       { ColorAnimation { duration: 150 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }
    }

    contentItem: Text {
        text:              root.text
        color:             root.enabled ? _textColor : "#484F58"
        font.pixelSize:    12
        font.weight:       Font.DemiBold
        font.family:       "Segoe UI, Inter, Helvetica Neue, Arial"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment:   Text.AlignVCenter
        Behavior on color { ColorAnimation { duration: 150 } }
    }

    transform: Scale {
        id: scaleTransform
        origin.x: root.width  / 2
        origin.y: root.height / 2
        xScale: root.hovered && root.enabled ? 1.02 : 1.0
        yScale: xScale
        Behavior on xScale { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
    }

    // ── Keyboard ──────────────────────────────────────────────────
    Keys.onReturnPressed: clicked()
    Keys.onSpacePressed:  clicked()

    // ── Colour palette by variant ─────────────────────────────────
    readonly property color _baseColor:  variant === "primary" ? "#1F6FEB"
                                       : variant === "danger"  ? "#F85149"
                                       : "#21262D"
    readonly property color _hoverColor: variant === "primary" ? "#388BFD"
                                       : variant === "danger"  ? "#FF6E6A"
                                       : "#30363D"
    readonly property color _pressColor: variant === "primary" ? "#1A5FCC"
                                       : variant === "danger"  ? "#D43F39"
                                       : "#1F6FEB"
    readonly property color _baseBorder: variant === "primary" ? "#1F6FEB"
                                       : variant === "danger"  ? "#F85149"
                                       : "#30363D"
    readonly property color _hoverBorder: variant === "primary" ? "#58A6FF"
                                        : variant === "danger"  ? "#FF8A87"
                                        : "#484F58"
    readonly property color _textColor:  (variant === "primary" || variant === "danger")
                                          ? "#FFFFFF" : "#E6EDF3"
}
