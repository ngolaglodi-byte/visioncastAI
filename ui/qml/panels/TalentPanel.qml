// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// TalentPanel.qml — Talent database browser with search and detail view.

import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    property string filterText: ""
    property string selectedId: ""

    // ── Header ─────────────────────────────────────────────────────
    Column {
        id:      hdr
        anchors { top: parent.top; left: parent.left; right: parent.right }
        spacing: 0

        Rectangle {
            width:  parent.width
            height: 44
            color:  "#1C2128"

            Text {
                text:  "TALENT"
                color: "#8B949E"
                font.pixelSize: 11
                font.weight:    Font.DemiBold
                font.letterSpacing: 1.0
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
            }

            // Active talent indicator
            Text {
                visible: bridge.currentTalent.length > 0
                text:    "● " + bridge.currentTalent
                color:   "#3FB950"
                font.pixelSize: 10
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                elide:   Text.ElideRight
                anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter }
                width:   120
            }
        }

        Item {
            width:  parent.width
            height: 48

            VCSearchField {
                placeholderText: "Search talent…"
                anchors { left: parent.left; leftMargin: 8; right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
                onTextChanged: (t) => { root.filterText = t }
            }
        }
    }

    // ── List ───────────────────────────────────────────────────────
    ListView {
        id: talentList
        anchors {
            top:    hdr.bottom
            bottom: parent.bottom
            left:   parent.left
            right:  parent.right
            margins: 8
        }
        spacing:    6
        clip:       true
        model:      bridge.talents

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        delegate: VCTalentCard {
            width:         talentList.width
            talentId:      modelData.id
            name:          modelData.name
            role:          modelData.role
            overlayActive: modelData.overlayActive ?? false
            selected:      root.selectedId === modelData.id
            visible: root.filterText.length === 0 ||
                     modelData.name.toLowerCase().includes(root.filterText.toLowerCase()) ||
                     modelData.role.toLowerCase().includes(root.filterText.toLowerCase())

            onClicked: {
                root.selectedId = modelData.id
                bridge.selectTalent(modelData.id)
            }
        }
    }
}
