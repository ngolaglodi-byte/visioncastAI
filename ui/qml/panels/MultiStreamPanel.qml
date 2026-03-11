// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// MultiStreamPanel.qml — Multi-platform simultaneous RTMP streaming panel.
//
// Allows operators to configure, start, stop, and monitor independent
// RTMP streams to YouTube Live, Facebook Live, Twitch, and custom
// destinations — all simultaneously alongside the broadcast TV outputs.

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    // ── Header ─────────────────────────────────────────────────────
    Rectangle {
        id: hdr
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 42
        color:  "#1C2128"

        RowLayout {
            anchors { fill: parent; leftMargin: 12; rightMargin: 8 }
            spacing: 8

            // Icon + title
            Rectangle {
                width: 8; height: 8; radius: 4
                color: root._anyLive ? "#3FB950" : "#484F58"
                Behavior on color { ColorAnimation { duration: 300 } }
            }
            Text {
                text:  "MULTI-STREAMING"
                color: "#8B949E"
                font.pixelSize: 11
                font.weight:    Font.DemiBold
                font.letterSpacing: 1.0
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            }

            // Live counter badge
            Rectangle {
                visible: root._liveCount > 0
                width:  liveCountLbl.implicitWidth + 10
                height: 18
                radius: 9
                color:  "#3FB95033"
                border { color: "#3FB950"; width: 1 }
                Text {
                    id: liveCountLbl
                    text:  root._liveCount + " LIVE"
                    color: "#3FB950"
                    font.pixelSize: 10
                    font.weight:    Font.Bold
                    anchors.centerIn: parent
                }
            }

            Item { Layout.fillWidth: true }

            // Stop-all button
            VCButton {
                text:    "Stop All"
                variant: "danger"
                height:  26
                visible: root._anyLive
                onClicked: bridge.stopAllRtmpStreams()
            }

            // Save config button
            VCButton {
                text:    "Save"
                variant: "default"
                height:  26
                onClicked: bridge.saveRtmpConfig()
            }
        }
    }

    // ── Runtime stats ──────────────────────────────────────────────
    property int  _liveCount: 0
    property bool _anyLive:   false

    function _updateStats() {
        var count = 0
        for (var i = 0; i < bridge.rtmpStreams.length; ++i) {
            if (bridge.rtmpStreams[i].status === "live") count++
        }
        root._liveCount = count
        root._anyLive   = count > 0
    }

    Connections {
        target: bridge
        function onRtmpStreamsChanged() { root._updateStats() }
        function onRtmpStreamStatusChanged(id, status, message) { root._updateStats() }
    }
    Component.onCompleted: root._updateStats()

    // ── Stream list ────────────────────────────────────────────────
    ListView {
        id: streamList
        anchors {
            top:    hdr.bottom
            bottom: addBar.top
            left:   parent.left
            right:  parent.right
            topMargin: 6
        }
        spacing:   6
        clip:      true
        model:     bridge.rtmpStreams
        leftMargin:  6
        rightMargin: 6

        delegate: StreamRow {
            width: streamList.width - 12
        }
    }

    // ── Add-platform bar ──────────────────────────────────────────
    Rectangle {
        id: addBar
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        height: 48
        color:  "#1C2128"

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: "#30363D"
        }

        RowLayout {
            anchors { fill: parent; leftMargin: 8; rightMargin: 8 }
            spacing: 6

            Text {
                text:  "Add:"
                color: "#8B949E"
                font.pixelSize: 11
                font.family:    "Segoe UI, Inter, Helvetica Neue, Arial"
            }

            // Quick-add platform buttons
            Repeater {
                model: [
                    { label: "YouTube",  platform: "youtube",  url: "rtmp://a.rtmp.youtube.com/live2" },
                    { label: "Facebook", platform: "facebook", url: "rtmp://live-api-s.facebook.com:80/rtmp" },
                    { label: "Twitch",   platform: "twitch",   url: "rtmp://live.twitch.tv/app" }
                ]
                delegate: VCButton {
                    text:    modelData.label
                    variant: "default"
                    height:  28
                    onClicked: bridge.addRtmpStream(
                        modelData.label, modelData.platform, modelData.url, "")
                }
            }

            // Custom destination
            VCButton {
                text:    "+ Custom"
                variant: "primary"
                height:  28
                onClicked: addDialog.open()
            }

            Item { Layout.fillWidth: true }
        }
    }

    // ── Add custom stream dialog ───────────────────────────────────
    Dialog {
        id:    addDialog
        title: "Add Custom RTMP Stream"
        modal: true
        anchors.centerIn: parent
        width:  440
        height: 300

        background: Rectangle {
            color:  "#161B22"
            radius: 10
            border { color: "#30363D"; width: 1 }
        }

        header: Rectangle {
            height: 48
            color:  "#1C2128"
            radius: 10
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1; color: "#30363D"
            }
            Text {
                text:  "Add Custom RTMP Stream"
                color: "#E6EDF3"
                font.pixelSize: 13
                font.weight:    Font.Bold
                anchors.left: parent.left; anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        contentItem: Column {
            anchors { fill: parent; margins: 16 }
            spacing: 12

            // Name
            Column {
                width: parent.width
                spacing: 4
                Text { text: "Name"; color: "#8B949E"; font.pixelSize: 11; font.family: "Segoe UI, Inter" }
                Rectangle {
                    width: parent.width; height: 34; radius: 6
                    color: "#21262D"; border { color: "#30363D"; width: 1 }
                    TextInput {
                        id: nameInput
                        anchors { fill: parent; leftMargin: 10; rightMargin: 10 }
                        verticalAlignment: TextInput.AlignVCenter
                        color: "#E6EDF3"; font.pixelSize: 12; font.family: "Segoe UI, Inter"
                        placeholderText: "e.g. Production Stream"
                        clip: true
                    }
                }
            }

            // RTMP URL
            Column {
                width: parent.width
                spacing: 4
                Text { text: "RTMP URL"; color: "#8B949E"; font.pixelSize: 11; font.family: "Segoe UI, Inter" }
                Rectangle {
                    width: parent.width; height: 34; radius: 6
                    color: "#21262D"; border { color: "#30363D"; width: 1 }
                    TextInput {
                        id: urlInput
                        anchors { fill: parent; leftMargin: 10; rightMargin: 10 }
                        verticalAlignment: TextInput.AlignVCenter
                        color: "#E6EDF3"; font.pixelSize: 12; font.family: "Segoe UI, Inter"
                        placeholderText: "rtmp://live.example.com/app"
                        clip: true
                    }
                }
            }

            // Stream key
            Column {
                width: parent.width
                spacing: 4
                Text { text: "Stream Key"; color: "#8B949E"; font.pixelSize: 11; font.family: "Segoe UI, Inter" }
                Rectangle {
                    width: parent.width; height: 34; radius: 6
                    color: "#21262D"; border { color: "#30363D"; width: 1 }
                    TextInput {
                        id: keyInput
                        anchors { fill: parent; leftMargin: 10; rightMargin: 10 }
                        verticalAlignment: TextInput.AlignVCenter
                        color: "#E6EDF3"; font.pixelSize: 12; font.family: "Segoe UI, Inter"
                        placeholderText: "xxxx-xxxx-xxxx-xxxx"
                        echoMode: TextInput.Password
                        clip: true
                    }
                }
            }
        }

        footer: Rectangle {
            height: 52; color: "transparent"
            RowLayout {
                anchors { fill: parent; rightMargin: 16 }
                Item { Layout.fillWidth: true }
                VCButton {
                    text: "Cancel"; variant: "default"; width: 80; height: 32
                    onClicked: addDialog.close()
                }
                VCButton {
                    text: "Add"; variant: "primary"; width: 80; height: 32
                    enabled: nameInput.text.trim().length > 0 && urlInput.text.trim().length > 0
                    onClicked: {
                        bridge.addRtmpStream(nameInput.text.trim(), "custom",
                                             urlInput.text.trim(), keyInput.text.trim())
                        nameInput.text = ""
                        urlInput.text  = ""
                        keyInput.text  = ""
                        addDialog.close()
                    }
                }
            }
        }
    }

    // ── StreamRow component ────────────────────────────────────────
    component StreamRow: Rectangle {
        id: row
        height: expanded ? 180 : 72
        radius: 8
        clip: true

        property bool expanded: false
        property bool _isLive:        modelData.status === "live"
        property bool _isConnecting:  modelData.status === "connecting"
        property bool _isError:       modelData.status === "error"

        color: _isLive       ? "#3FB95012"
             : _isConnecting ? "#D2992212"
             : _isError      ? "#F8514912"
             : "#1C2128"
        border {
            color: _isLive       ? "#3FB95055"
                 : _isConnecting ? "#D2992255"
                 : _isError      ? "#F8514955"
                 : "#30363D"
            width: 1
        }

        Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        Behavior on color  { ColorAnimation  { duration: 200 } }

        // ── Top row ────────────────────────────────────────────────
        RowLayout {
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 10 }
            height: 52
            spacing: 8

            // Status dot
            Rectangle {
                width: 10; height: 10; radius: 5
                color: row._isLive       ? "#3FB950"
                     : row._isConnecting ? "#D29922"
                     : row._isError      ? "#F85149"
                     : "#484F58"
                Behavior on color { ColorAnimation { duration: 300 } }

                SequentialAnimation on opacity {
                    running:  row._isConnecting
                    loops:    Animation.Infinite
                    NumberAnimation { to: 0.2; duration: 600 }
                    NumberAnimation { to: 1.0; duration: 600 }
                }
            }

            // Platform badge
            Rectangle {
                width:  platformLbl.implicitWidth + 12
                height: 22; radius: 4
                color: row._platformColor + "22"
                border { color: row._platformColor; width: 1 }
                Text {
                    id: platformLbl
                    text:  modelData.platform.toUpperCase()
                    color: row._platformColor
                    font.pixelSize: 10; font.weight: Font.Bold
                    anchors.centerIn: parent
                }
            }

            // Name
            Text {
                text:  modelData.name
                color: "#E6EDF3"
                font.pixelSize: 12; font.weight: Font.Medium
                font.family: "Segoe UI, Inter, Helvetica Neue, Arial"
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            // Status label
            Text {
                text:  row._statusLabel
                color: row._isLive       ? "#3FB950"
                     : row._isConnecting ? "#D29922"
                     : row._isError      ? "#F85149"
                     : "#8B949E"
                font.pixelSize: 10; font.weight: Font.Bold
                font.family: "Segoe UI, Inter, Helvetica Neue, Arial"
            }

            // Start / Stop button
            VCButton {
                text:    row._isLive || row._isConnecting ? "Stop" : "Start"
                variant: row._isLive || row._isConnecting ? "danger" : "primary"
                height:  28; width: 60
                onClicked: {
                    if (row._isLive || row._isConnecting)
                        bridge.stopRtmpStream(modelData.id)
                    else
                        bridge.startRtmpStream(modelData.id)
                }
            }

            // Expand / collapse log
            VCButton {
                text:    row.expanded ? "▲" : "▼"
                variant: "default"
                height:  28; width: 32
                onClicked: row.expanded = !row.expanded
            }

            // Remove button
            VCButton {
                text:    "✕"
                variant: "danger"
                height:  28; width: 32
                enabled: !row._isLive && !row._isConnecting
                onClicked: bridge.removeRtmpStream(modelData.id)
            }
        }

        // ── URL / key row ──────────────────────────────────────────
        RowLayout {
            anchors {
                top: parent.top; topMargin: 54
                left: parent.left; leftMargin: 30
                right: parent.right; rightMargin: 10
            }
            height: 18
            spacing: 6
            visible: true

            Text {
                text:  modelData.serverUrl.length > 0
                       ? modelData.serverUrl : "No URL configured"
                color: "#484F58"
                font.pixelSize: 10; font.family: "Segoe UI, Inter"
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
            Rectangle {
                width:  keyBadge.implicitWidth + 10
                height: 16; radius: 4
                color: modelData.streamKey.length > 0 ? "#1F6FEB22" : "#21262D"
                border { color: modelData.streamKey.length > 0 ? "#1F6FEB55" : "#30363D"; width: 1 }
                Text {
                    id: keyBadge
                    text:  modelData.streamKey.length > 0 ? "🔑 Key set" : "No key"
                    color: modelData.streamKey.length > 0 ? "#58A6FF" : "#484F58"
                    font.pixelSize: 9
                    anchors.centerIn: parent
                }
            }
        }

        // ── Expanded log area ──────────────────────────────────────
        Rectangle {
            anchors {
                top:    parent.top; topMargin: 76
                bottom: parent.bottom; bottomMargin: 6
                left:   parent.left; leftMargin: 10
                right:  parent.right; rightMargin: 10
            }
            visible:  row.expanded
            color:    "#0D1117"
            radius:   6
            clip:     true
            border { color: "#21262D"; width: 1 }

            Flickable {
                anchors { fill: parent; margins: 6 }
                contentHeight: logText.implicitHeight
                clip: true

                Text {
                    id: logText
                    width: parent.width
                    text:  modelData.log.length > 0 ? modelData.log : "[No log output yet]"
                    color: "#8B949E"
                    font.pixelSize: 10
                    font.family:    "Consolas, Courier New, monospace"
                    wrapMode: Text.WrapAnywhere
                }
            }
        }

        // ── Helpers ────────────────────────────────────────────────
        readonly property color _platformColor:
            modelData.platform === "youtube"  ? "#F85149"
          : modelData.platform === "facebook" ? "#58A6FF"
          : modelData.platform === "twitch"   ? "#A855F7"
          : "#3FB950"

        readonly property string _statusLabel:
            _isLive       ? "● LIVE"
          : _isConnecting ? "⟳ CONNECTING"
          : _isError      ? "✕ ERROR"
          : "○ IDLE"
    }
}
