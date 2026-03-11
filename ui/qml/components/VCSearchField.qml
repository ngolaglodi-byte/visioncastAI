// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCSearchField.qml — Search input with magnifier icon and clear button.

import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    // ── Public API ───��─────────────────────────────────────────────
    property alias text:            field.text
    property string placeholder:    "Search…"
    signal searchTextChanged(string text)

    implicitHeight: 34
    implicitWidth:  240
    radius:         6
    color:          "#0D1117"
    border.color:   field.activeFocus ? "#1F6FEB" : "#30363D"
    border.width:   1

    Behavior on border.color { ColorAnimation { duration: 150 } }

    // Search icon
    Text {
        id: icon
        text: "\u{1F50D}"
        font.pixelSize: 13
        anchors.left:           parent.left
        anchors.leftMargin:     10
        anchors.verticalCenter: parent.verticalCenter
        opacity: 0.5
    }

    // Input field
    TextInput {
        id: field
        anchors.left:           icon.right
        anchors.leftMargin:     6
        anchors.right:          clearBtn.left
        anchors.rightMargin:    4
        anchors.verticalCenter: parent.verticalCenter
        color:              "#E6EDF3"
        selectionColor:     "#1F6FEB"
        selectedTextColor:  "#FFFFFF"
        font.pixelSize:     13
        font.family:        "Segoe UI, Inter, Helvetica Neue, Arial"
        clip:               true
        focus:              true

        Text {
            visible: parent.text.length === 0
            text:    root.placeholder
            color:   "#484F58"
            font:    parent.font
            anchors.verticalCenter: parent.verticalCenter
        }

        onTextChanged: root.searchTextChanged(text)

        Keys.onEscapePressed: { text = ""; root.searchTextChanged("") }
    }

    // Clear button
    Text {
        id:      clearBtn
        visible: field.text.length > 0
        text:    "\u2717"
        color:   "#8B949E"
        font.pixelSize: 11
        anchors.right:          parent.right
        anchors.rightMargin:    8
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            anchors.fill: parent
            cursorShape:  Qt.PointingHandCursor
            onClicked:    { field.text = ""; root.searchTextChanged(""); field.forceActiveFocus() }
        }

        HoverHandler { onHoveredChanged: clearBtn.color = hovered ? "#E6EDF3" : "#8B949E" }
    }

    // Click anywhere on the card to focus the input
    MouseArea {
        anchors.fill: parent
        onPressed:    { field.forceActiveFocus(); mouse.accepted = false }
    }
}