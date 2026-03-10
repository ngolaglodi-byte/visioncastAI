// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// SourcePanel.qml — Left column: camera/source list with search and
//                   refresh button.

import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    // ── Header ─────────────────────────────────────────────────────
    Column {
        id:      headerArea
        anchors { top: parent.top; left: parent.left; right: parent.right }
        spacing: 0

        // Title bar
        Rectangle {
            width:  parent.width
            height: 44
            color:  "#1C2128"
            border.bottom: 1

            Text {
                text:  "VIDEO SOURCES"
                color: "#8B949E"
                font.pixelSize: 11
                font.weight:    Font.DemiBold
                font.letterSpacing: 1.0
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
            }

            VCButton {
                id:      refreshBtn
                text:    "⟳"
                width:   28; height: 28
                anchors { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
                onClicked: bridge.refreshSources()
            }
        }

        // Search
        Item {
            width:  parent.width
            height: 48

            VCSearchField {
                placeholderText: "Search sources…"
                anchors { left: parent.left; leftMargin: 8; right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
                onTextChanged: (t) => { filterText = t }
            }
        }
    }

    property string filterText: ""

    // ── Source List ────────────────────────────────────────────────
    ListView {
        id:      sourceList
        anchors {
            top:    headerArea.bottom
            bottom: parent.bottom
            left:   parent.left
            right:  parent.right
            margins: 8
        }
        spacing:     6
        clip:        true
        model:       bridge.videoSources
        currentIndex: -1

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        delegate: VCVideoThumbnail {
            width:      sourceList.width
            sourceName: modelData.name       ?? ""
            deviceType: modelData.deviceType ?? ""
            selected:   sourceList.currentIndex === index
            visible:    root.filterText.length === 0 ||
                        modelData.name.toLowerCase().includes(root.filterText.toLowerCase())

            onClicked: {
                sourceList.currentIndex = index
                bridge.selectSource(index)
            }
        }

        // Empty state
        Text {
            visible:        sourceList.count === 0
            anchors.centerIn: parent
            text:           "No video sources found.\nClick ⟳ to scan devices."
            color:          "#484F58"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }
    }
}
