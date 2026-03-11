// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// OverlayPanel.qml — Overlay template browser + editor.

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"

    property string filterText: ""
    property string selectedId: ""

    // === Header (barre + recherche) ===
    Column {
        id: hdr
        anchors.top:   parent.top
        anchors.left:  parent.left
        anchors.right: parent.right
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
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                visible: activeCount > 0
                width:   activeCountLbl.implicitWidth + 10
                height:  20
                radius:  10
                color:   "#3FB9501A"
                border.color: "#3FB950"
                border.width: 1
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter

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

        // Search
        Item {
            width:  parent.width
            height: 48

            VCSearchField {
                placeholder: "Search overlays\u2026"
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                onSearchTextChanged: function(t) { root.filterText = t }
            }
        }
    }

    // === Content: Liste + éditeur ===
    RowLayout {
        anchors {
            top:    hdr.bottom
            bottom: parent.bottom
            left:   parent.left
            right:  parent.right
            margins: 8
        }
        spacing: 8

        // == Overlay list ==
        ListView {
            id: overlayList
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            clip: true
            spacing: 6
            model: bridge.overlayTemplates

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            delegate: Rectangle {
                width: overlayList.width
                height: 54
                color: root.selectedId === modelData.id ? "#21262D" : "transparent"
                radius: 6
                border.color: root.selectedId === modelData.id ? "#1F6FEB" : "transparent"
                border.width: 1

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.selectedId = modelData.id;
                        root.formTitle     = modelData.title     || "";
                        root.formSubtitle  = modelData.subtitle  || "";
                        root.formStyle     = modelData.style     || "Default";
                        root.formColor     = modelData.color     || "#FFFFFF";
                        root.formEntryAnim = modelData.entryAnim || "none";
                        root.formExitAnim  = modelData.exitAnim  || "none";
                        root.formDuration  = modelData.duration  || 500;
                    }
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 2
                    Text {
                        text: modelData.title || modelData.name || "Overlay"
                        color: "#E6EDF3"
                        font.pixelSize: 13
                        font.weight: root.selectedId === modelData.id ? Font.Bold : Font.Normal
                    }
                    Text {
                        text: modelData.subtitle || modelData.type || ""
                        color: "#8B949E"
                        font.pixelSize: 11
                    }
                }
            }
        }

        // == Overlay editor ==
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#21262D"
            radius: 8
            border.color: "#30363D"
            border.width: 1
            anchors.margins: 6

            property string formTitle: ""
            property string formSubtitle: ""
            property string formStyle: "Default"
            property string formColor: "#FFFFFF"
            property string formEntryAnim: "none"
            property string formExitAnim: "none"
            property int    formDuration: 500

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Text {
                    text: root.selectedId === "" ? "Sélectionnez un overlay à éditer" : "Edit Overlay"
                    color: "#93a1b1"
                    font.weight: Font.Bold
                    font.pixelSize: 15
                }

                // Form
                GridLayout {
                    columns: 2
                    rowSpacing: 6
                    columnSpacing: 18
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Text { text: "Title"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: parent.formTitle; onTextChanged: parent.formTitle = text }

                    Text { text: "Subtitle"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: parent.formSubtitle; onTextChanged: parent.formSubtitle = text }

                    Text { text: "Style"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    ComboBox {
                        model: ["Default", "Minimal", "Barred", "Lower Third"]
                        currentIndex: model.indexOf(parent.formStyle)
                        onCurrentIndexChanged: parent.formStyle = model[currentIndex]
                    }

                    Text { text: "Color"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: parent.formColor; onTextChanged: parent.formColor = text }

                    Text { text: "Entry Animation"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    ComboBox {
                        model: ["none", "fade", "slide", "grow"]
                        currentIndex: model.indexOf(parent.formEntryAnim)
                        onCurrentIndexChanged: parent.formEntryAnim = model[currentIndex]
                    }

                    Text { text: "Exit Animation"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    ComboBox {
                        model: ["none", "fade", "slide", "grow"]
                        currentIndex: model.indexOf(parent.formExitAnim)
                        onCurrentIndexChanged: parent.formExitAnim = model[currentIndex]
                    }

                    Text { text: "Duration (ms)"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    SpinBox {
                        value: parent.formDuration
                        from: 100; to: 10000; stepSize: 100
                        onValueChanged: parent.formDuration = value
                    }
                }

                RowLayout {
                    spacing: 12
                    Button {
                        text: "Apply"
                        enabled: root.selectedId !== ""
                        onClicked: {
                            // Appelle la fonction C++ dans qml_bridge pour appliquer les changements
                            bridge.updateOverlay(
                                root.selectedId, parent.formTitle, parent.formSubtitle,
                                parent.formStyle, parent.formColor, parent.formEntryAnim,
                                parent.formExitAnim, parent.formDuration
                            );
                        }
                    }
                    Button {
                        text: "Clear"
                        onClicked: {
                            root.selectedId = ""
                            parent.formTitle = ""
                            parent.formSubtitle = ""
                            parent.formStyle = "Default"
                            parent.formColor = "#FFFFFF"
                            parent.formEntryAnim = "none"
                            parent.formExitAnim = "none"
                            parent.formDuration = 500
                        }
                    }
                }
            }
        }
    }
}