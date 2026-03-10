// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// OverlayPanel.qml — Overlay template browser with live toggle per template.

import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    property string filterText: ""

    // ── Header ─────────────────────────────────────────────────────
    Column {
        id: hdr
        anchors { top: parent.top; left: parent.left; right: parent.right }
        spacing: 0

        Rectangle {
            width:  parent.width
            height: 44
            color:  "#1C2128"

            Text {
                text:  "OVERLAYS"
                color: "#8B949E"
                font.pixelSize: 11
                font.weight:    Font.DemiBold
                font.letterSpacing: 1.0
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
            }

            // Active count badge
            Rectangle {
                visible: activeCount > 0
                width:   activeCountLbl.implicitWidth + 10
                height:  20
                radius:  10
                color:   "#3FB9501A"
                border { color: "#3FB950"; width: 1 }
                anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter }

                property int activeCount: {
                    var n = 0
                    for (var i = 0; i < bridge.overlayTemplates.length; i++)
                        if (bridge.overlayTemplates[i].active) n++
                    return n
                }

                Text {
                    id:    activeCountLbl
                    text:  parent.activeCount
                    color: "#3FB950"
                    font.pixelSize: 10
                    font.weight:    Font.DemiBold
                    anchors.centerIn: parent
                }
            }
        }

        Item {
            width:  parent.width
            height: 48

            VCSearchField {
                placeholderText: "Search overlays…"
                anchors { left: parent.left; leftMargin: 8; right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
                onTextChanged: (t) => { root.filterText = t }
            }
        }
    }

    // ── List ───────────────────────────────────────────────────────
    ListView {
        anchors { top: hdr.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 8 }
        spacing:    6
        clip:       true
        model:      bridge.overlayTemplates

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        delegate: VCOverlayPreview {
            width:      parent ? parent.width : 300
            overlayId:  modelData.id
            name:       modelData.name
            type:       modelData.type
            active:     modelData.active ?? false
            visible: root.filterText.length === 0 ||
                     modelData.name.toLowerCase().includes(root.filterText.toLowerCase())

            onToggleRequested: (id) => bridge.toggleOverlay(id)
        }
    }
}
