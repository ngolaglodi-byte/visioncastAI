// VisionCast-AI — DesignPanel.qml : Thèmes, couleurs, templates.

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#161B22"

    property string selectedTheme: "Dark"
    property string selectedTemplate: "Default"
    property color selectedAccent: "#1F6FEB"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 18

        Text {
            text: "Design & Thèmes"
            font.pixelSize: 17
            font.weight: Font.Bold
            color: "#E6EDF3"
        }

        // Thème (clair/foncé, custom)
        RowLayout {
            spacing: 12
            Text { text: "Thème"; color: "#8B949E"; font.pixelSize: 12 }
            ComboBox {
                id: themeCombo
                model: ["Dark", "Light", "Ocean", "Prestige"]
                currentIndex: themeCombo.model.indexOf(root.selectedTheme)
                onCurrentIndexChanged: root.selectedTheme = themeCombo.model[currentIndex]
            }
            Button {
                text: "Appliquer"
                onClicked: bridge.setTheme(root.selectedTheme)
            }
        }

        // Palette couleur  
        RowLayout {
            spacing: 10
            Text { text: "Accent"; color: "#8B949E"; font.pixelSize: 12 }
            Repeater {
                model: ["#1F6FEB", "#A855F7", "#3FB950", "#FFD33D", "#F85149"]
                delegate: Rectangle {
                    width: 24; height: 24; radius: 12
                    color: modelData
                    border.color: root.selectedAccent === modelData ? "#E6EDF3" : "transparent"
                    border.width: 2
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.selectedAccent = modelData
                    }
                }
            }
            Button {
                text: "Appliquer"
                onClicked: bridge.setAccentColor(root.selectedAccent)
            }
        }

        // Choix de template graphique
        RowLayout {
            spacing: 12
            Text { text: "Template"; color: "#8B949E"; font.pixelSize: 12 }
            ComboBox {
                id: templateCombo
                model: ["Default", "Barred", "Minimal", "Sports", "Split"]
                currentIndex: templateCombo.model.indexOf(root.selectedTemplate)
                onCurrentIndexChanged: root.selectedTemplate = templateCombo.model[currentIndex]
            }
            Button {
                text: "Activer"
                onClicked: bridge.setGraphicsTemplate(root.selectedTemplate)
            }
        }

        // Sauvegarder config design
        RowLayout {
            spacing: 10
            Button {
                text: "Sauvegarder configuration"
                onClicked: bridge.exportDesignSettings()
            }
            Button {
                text: "Restaurer config"
                onClicked: bridge.importDesignSettings()
            }
        }
    }
}