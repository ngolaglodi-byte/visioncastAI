// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// OutputPanel.qml — Output configuration: SDI, NDI, SRT, RTMP toggles.

import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    // ── Header ─────────────────────────────────────────────────────
    Rectangle {
        id: hdr
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 36
        color:  "#1C2128"

        Text {
            text:  "OUTPUT CONFIGURATION"
            color: "#8B949E"
            font.pixelSize: 11
            font.weight:    Font.DemiBold
            font.letterSpacing: 1.0
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
        }
    }

    // ── Output rows ────────────────────────────────────────────────
    ListView {
        anchors { top: hdr.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 8 }
        spacing:    6
        clip:       true
        model:      bridge.outputConfigs

        delegate: Rectangle {
            width:   parent ? parent.width : 300
            height:  48
            radius:  8
            color:   modelData.enabled ? "#1F6FEB0D" : "#161B22"
            border { color: modelData.enabled ? "#1F6FEB55" : "#30363D"; width: 1 }

            Behavior on color        { ColorAnimation { duration: 200 } }
            Behavior on border.color { ColorAnimation { duration: 200 } }

            // Protocol badge
            Rectangle {
                id:    badge
                width: protocolLbl.implicitWidth + 12
                height: 22
                radius: 4
                color: modelData.protocol === "SDI"  ? "#21262D"
                     : modelData.protocol === "NDI"  ? "#1F6FEB22"
                     : modelData.protocol === "RTMP" ? "#F8514922"
                     : "#3FB95022"
                border {
                    width: 1
                    color: modelData.protocol === "SDI"  ? "#484F58"
                         : modelData.protocol === "NDI"  ? "#1F6FEB"
                         : modelData.protocol === "RTMP" ? "#F85149"
                         : "#3FB950"
                }
                anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }

                Text {
                    id:    protocolLbl
                    text:  modelData.protocol
                    color: modelData.protocol === "SDI"  ? "#8B949E"
                         : modelData.protocol === "NDI"  ? "#58A6FF"
                         : modelData.protocol === "RTMP" ? "#F85149"
                         : "#3FB950"
                    font.pixelSize: 10
                    font.weight:    Font.Bold
                    anchors.centerIn: parent
                }
            }

            Text {
                text:  modelData.name
                color: "#E6EDF3"
                font.pixelSize: 12
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                elide: Text.ElideRight
                anchors {
                    left:    badge.right
                    leftMargin: 10
                    right:   toggle.left
                    rightMargin: 8
                    verticalCenter: parent.verticalCenter
                }
            }

            VCToggleSwitch {
                id:      toggle
                checked: modelData.enabled
                anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter }
                // Note: toggling output config is a future backend hook
            }
        }
    }
}
