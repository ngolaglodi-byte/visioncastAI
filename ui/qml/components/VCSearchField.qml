// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// VCSearchField.qml — Search input with magnifier icon and clear button.

import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    // ── Public API ─────────────────────────────────────────────────
    property alias text:        field.text
    property alias placeholderText: field.placeholderText
    signal textChanged(string text)

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
        text: "🔍"
        font.pixelSize: 13
        anchors {
            left:           parent.left
            leftMargin:     10
            verticalCenter: parent.verticalCenter
        }
        opacity: 0.5
    }

    // Input field
    TextInput {
        id: field
        anchors {
            left:           icon.right
            leftMargin:     6
            right:          clearBtn.left
            rightMargin:    4
            verticalCenter: parent.verticalCenter
        }
        color:              "#E6EDF3"
        selectionColor:     "#1F6FEB"
        selectedTextColor:  "#FFFFFF"
        font.pixelSize:     13
        font.family:        "Segoe UI, Inter, Helvetica Neue, Arial"
        clip:               true
        focus:              true

        property string placeholderText: "Search…"

        Text {
            visible: parent.text.length === 0
            text:    parent.placeholderText
            color:   "#484F58"
            font:    parent.font
            anchors.verticalCenter: parent.verticalCenter
        }

        onTextChanged: root.textChanged(text)

        Keys.onEscapePressed: { text = ""; root.textChanged("") }
    }

    // Clear button
    Text {
        id:      clearBtn
        visible: field.text.length > 0
        text:    "✕"
        color:   "#8B949E"
        font.pixelSize: 11
        anchors {
            right:          parent.right
            rightMargin:    8
            verticalCenter: parent.verticalCenter
        }

        MouseArea {
            anchors.fill: parent
            cursorShape:  Qt.PointingHandCursor
            onClicked:    { field.text = ""; root.textChanged(""); field.forceActiveFocus() }
        }

        HoverHandler { onHoveredChanged: clearBtn.color = hovered ? "#E6EDF3" : "#8B949E" }
    }

    // Click anywhere on the card to focus the input
    MouseArea {
        anchors.fill: parent
        onPressed:    { field.forceActiveFocus(); mouse.accepted = false }
    }
}
