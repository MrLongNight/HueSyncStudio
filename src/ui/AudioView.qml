import QtQuick
import QtQuick.Controls

Pane {
    id: root

    // Simple Mode Implementation
    Row {
        anchors.fill: parent
        spacing: 5
        padding: 5

        Rectangle {
            id: lowBandIndicator
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Normalize the energy value to a 0.0 - 1.0 range for opacity
            // The magic number '0.1' is a scaling factor, needs tuning later.
            opacity: Math.min(audioDataModel.lowBandEnergy * 0.1, 1.0)

            Behavior on opacity {
                NumberAnimation { duration: 100 }
            }
        }

        Rectangle {
            id: midBandIndicator
            color: "green"
            Layout.fillWidth: true
            Layout.fillHeight: true
            opacity: Math.min(audioDataModel.midBandEnergy * 0.1, 1.0)

            Behavior on opacity {
                NumberAnimation { duration: 100 }
            }
        }

        Rectangle {
            id: highBandIndicator
            color: "red"
            Layout.fillWidth: true
            Layout.fillHeight: true
            opacity: Math.min(audioDataModel.highBandEnergy * 0.1, 1.0)

            Behavior on opacity {
                NumberAnimation { duration: 100 }
            }
        }
    }
}
