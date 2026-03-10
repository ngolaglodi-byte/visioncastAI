// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCCard.qml — Card container with subtle elevation shadow and
//              configurable border colour.

import QtQuick 2.15

Rectangle {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property alias  content:       contentLoader.sourceComponent
    property color  borderColor:   "#30363D"
    property bool   elevated:      true
    property int    contentPadding: 12

    // ── Layout ────────────────────────────────────────────────────
    color:         "#21262D"
    radius:        8
    border.color:  borderColor
    border.width:  1

    layer.enabled: elevated
    layer.effect: null   // shadow via drop-shadow rectangle underneath

    // Drop-shadow illusion via slightly-larger blurred rect beneath
    Rectangle {
        id: shadow
        anchors { fill: parent; margins: -1 }
        color:   "transparent"
        z:       -1
        radius:  root.radius + 1
        border { color: Qt.rgba(0,0,0,0.4); width: 2 }
        visible: elevated
    }

    // Content loader so callers can supply any Item via content property
    Loader {
        id:              contentLoader
        anchors {
            fill:    parent
            margins: contentPadding
        }
    }

    // Allow plain children to be placed inside without using content alias
    default property alias children: innerItem.data

    Item {
        id:      innerItem
        anchors { fill: parent; margins: root.contentPadding }
    }
}
