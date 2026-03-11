// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.
//
// TalentPanel.qml — Talent database browser with add/remove and detail view.

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Rectangle {
    id: root
    color: "#161B22"
    property string filterText: ""
    property string selectedId: ""
    property string formId: ""
    property string formName: ""
    property string formRole: ""
    property string formOrg: ""
    property string formPhoto: ""

    signal showToast(string msg, string lvl)

    // Header (search)
    Column {
        id:      hdr
        anchors.top:   parent.top
        anchors.left:  parent.left
        anchors.right: parent.right
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
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                visible: bridge.currentTalent.length > 0
                text:    "\u25CF " + bridge.currentTalent
                color:   "#3FB950"
                font.pixelSize: 10
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width:   120
                elide: Text.ElideRight
            }
        }

        // Search
        Item {
            width:  parent.width
            height: 48

            VCSearchField {
                placeholder: "Search talent\u2026"
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                onSearchTextChanged: function(t) { root.filterText = t }
            }
        }
    }

    // List + Details
    RowLayout {
        anchors {
            top:    hdr.bottom
            bottom: parent.bottom
            left:   parent.left
            right:  parent.right
            margins: 8
        }
        spacing: 8

        // Talent list
        ListView {
            id: talentList
            Layout.preferredWidth: 210
            Layout.fillHeight: true
            clip: true
            spacing: 6
            model: bridge.talents

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            delegate: VCTalentCard {
                width: talentList.width
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
                    root.formId     = modelData.id
                    root.formName   = modelData.name
                    root.formRole   = modelData.role
                    root.formOrg    = modelData.organisation !== undefined ? modelData.organisation : ""
                    root.formPhoto  = modelData.photo !== undefined ? modelData.photo : ""
                    bridge.selectTalent(modelData.id)
                }
            }
        }

        // Artist form/détail
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#21262D"
            radius: 8
            border.color: "#30363D"
            border.width: 1
            anchors.margins: 6

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Text {
                    text: root.selectedId === "" ? "Add new talent" : "Talent details"
                    color: "#93a1b1"
                    font.weight: Font.Bold
                    font.pixelSize: 15
                }

                // Form
                GridLayout {
                    id: form
                    columns: 2
                    rowSpacing: 6
                    columnSpacing: 18
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Text { text: "Name"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: root.formName; onTextChanged: root.formName = text }

                    Text { text: "Role"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: root.formRole; onTextChanged: root.formRole = text }

                    Text { text: "Organisation"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: root.formOrg; onTextChanged: root.formOrg = text }

                    Text { text: "Photo URL"; color: "#8B949E"; font.pixelSize: 12; Layout.alignment: Qt.AlignRight }
                    TextField { text: root.formPhoto; onTextChanged: root.formPhoto = text }
                }

                RowLayout {
                    spacing: 12
                    Button {
                        text: root.selectedId === "" ? "Add" : "Update"
                        enabled: root.formName.length > 0 && root.formRole.length > 0
                        onClicked: {
                            if (root.selectedId === "") {
                                bridge.addTalent(root.formName, root.formRole, root.formOrg, root.formPhoto)
                                root.showToast("Talent added!", "success")
                            } else {
                                bridge.updateTalent(root.formId, root.formName, root.formRole, root.formOrg, root.formPhoto)
                                root.showToast("Talent updated!", "success")
                            }
                            root.clearForm()
                        }
                    }
                    Button {
                        text: "Remove"
                        visible: root.selectedId !== ""
                        onClicked: {
                            bridge.removeTalent(root.formId)
                            root.showToast("Talent removed!", "warning")
                            root.clearForm()
                        }
                    }
                    Button {
                        text: "Clear"
                        onClicked: root.clearForm()
                    }
                }
            }

            function clearForm() {
                root.selectedId = ""
                root.formId = ""
                root.formName = ""
                root.formRole = ""
                root.formOrg = ""
                root.formPhoto = ""
            }
        }
    }

    // Notifications depuis ce composant
    onShowToast: function(msg, lvl) {
        root.Window ? root.Window._toastMsg = msg : console.log(msg)
    }
}