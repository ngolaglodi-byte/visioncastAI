// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// MonitoringPanel.qml — System metrics: CPU, GPU, Memory, FPS, Network.

import QtQuick 2.15
import QtQuick.Layouts 1.15
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
            text:  "SYSTEM MONITORING"
            color: "#8B949E"
            font.pixelSize: 11
            font.weight:    Font.DemiBold
            font.letterSpacing: 1.0
            font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
        }

        // Engine status
        VCStatusIndicator {
            status: bridge.engineRunning ? "active" : "inactive"
            label:  bridge.engineRunning ? "ENGINE ON" : "ENGINE OFF"
            anchors { right: parent.right; rightMargin: 12; verticalCenter: parent.verticalCenter }
        }
    }

    // ── Gauges ─────────────────────────────────────────────────────
    Grid {
        anchors {
            top:    hdr.bottom
            bottom: parent.bottom
            left:   parent.left
            right:  parent.right
            margins: 16
        }
        columns:    2
        rowSpacing: 16
        columnSpacing: 24

        VCMetricGauge {
            label:     "CPU"
            value:     bridge.cpuUsage
            barColor:  "#1F6FEB"
            width:     (parent.width - parent.columnSpacing) / 2
        }
        VCMetricGauge {
            label:     "GPU"
            value:     bridge.gpuUsage
            barColor:  "#A855F7"
            width:     (parent.width - parent.columnSpacing) / 2
        }
        VCMetricGauge {
            label:     "MEMORY"
            value:     bridge.memoryUsage
            barColor:  "#58A6FF"
            width:     (parent.width - parent.columnSpacing) / 2
        }

        // FPS panel
        Item {
            width:  (parent.width - parent.columnSpacing) / 2
            height: 42

            Text {
                text:  "FPS"
                color: "#8B949E"
                font.pixelSize: 11
                font.letterSpacing: 0.8
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
                anchors { left: parent.left; top: parent.top }
            }
            Text {
                text:           bridge.fps
                color:          bridge.fps >= 24 ? "#3FB950" : "#F85149"
                font.pixelSize: 24
                font.weight:    Font.Bold
                font.family:    "JetBrains Mono, Cascadia Code, monospace"
                anchors { left: parent.left; bottom: parent.bottom }
                Behavior on color { ColorAnimation { duration: 300 } }
            }
            Text {
                text:  "fps"
                color: "#484F58"
                font.pixelSize: 11
                anchors { bottom: parent.bottom; left: parent.left; leftMargin: bridge.fps >= 10 ? 38 : 28 }
            }
        }
    }
}
