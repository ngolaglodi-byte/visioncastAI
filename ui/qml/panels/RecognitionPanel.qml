// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// RecognitionPanel.qml — AI recognition status and detected faces/entities.

import QtQuick 2.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    // ── Header ─────────────────────────────────────────────────────
    Rectangle {
        id: hdr
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 44
        color:  "#1C2128"

        Text {
            text:  "AI RECOGNITION"
            color: "#8B949E"
            font.pixelSize: 11
            font.weight:    Font.DemiBold
            font.letterSpacing: 1.0
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
        }

        VCStatusIndicator {
            status: bridge.aiConnected ? "active" : "inactive"
            label:  bridge.aiConnected ? "CONNECTED" : "OFFLINE"
            anchors { right: parent.right; rightMargin: 12; verticalCenter: parent.verticalCenter }
        }
    }

    // ── Status area ────────────────────────────────────────────────
    Column {
        anchors {
            top:     hdr.bottom
            left:    parent.left
            right:   parent.right
            margins: 16
        }
        spacing: 16

        // AI service status card
        Rectangle {
            width:  parent.width
            height: 80
            radius: 8
            color:  "#21262D"
            border { color: "#30363D"; width: 1 }

            Column {
                anchors.centerIn: parent
                spacing: 6

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:  bridge.aiConnected ? "🟢  AI Service Active" : "⚫  AI Service Offline"
                    color: bridge.aiConnected ? "#3FB950" : "#484F58"
                    font.pixelSize: 13
                    font.weight:    Font.DemiBold
                    font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:  bridge.aiConnected
                           ? "Face detection running"
                           : "Start the engine to launch AI"
                    color: "#8B949E"
                    font.pixelSize: 11
                }
            }
        }

        // Detected talent placeholder
        Text {
            text:  "DETECTED"
            color: "#8B949E"
            font.pixelSize: 10
            font.letterSpacing: 1.0
            font.weight:    Font.DemiBold
        }

        Rectangle {
            width:  parent.width
            height: 60
            radius: 8
            color:  "#21262D"
            border { color: "#30363D"; width: 1 }
            visible: bridge.aiConnected

            Row {
                anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
                spacing: 10

                Rectangle {
                    width: 36; height: 36; radius: 18
                    color: "#1F6FEB22"
                    border { color: "#1F6FEB"; width: 1 }
                    Text {
                        anchors.centerIn: parent
                        text:  bridge.currentTalent.length > 0 ? bridge.currentTalent.charAt(0) : "?"
                        color: "#58A6FF"
                        font.pixelSize: 16
                        font.weight:    Font.Bold
                    }
                }

                Column {
                    spacing: 4
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        text:  bridge.currentTalent.length > 0 ? bridge.currentTalent : "No match"
                        color: "#E6EDF3"
                        font.pixelSize: 12
                        font.weight:    Font.DemiBold
                    }
                    Text {
                        text:  bridge.currentTalent.length > 0 ? "Identified • 97% confidence" : "Scanning…"
                        color: "#8B949E"
                        font.pixelSize: 10
                    }
                }
            }
        }
    }
}
