import QtQuick
import QtQuick.Controls

Pane {
    id: root

    Item {
        id: mapArea
        anchors.fill: parent
        anchors.bottom: previewButton.top

        Repeater {
            model: lampModel

            delegate: Rectangle {
                id: lampIndicator
                width: 50
                height: 20
                radius: 10

                color: model.is_preview ? "yellow" : "lightblue"
                border.color: model.is_preview ? "orange" : "steelblue"
                opacity: model.is_preview ? 1.0 : 0.7

                x: model.x_pos
                y: model.y_pos

                property int lampId: model.id

                Text {
                    text: model.name
                    anchors.centerIn: parent
                }

                DragHandler {
                    id: dragHandler
                    target: lampIndicator
                }

                DropArea {
                    anchors.fill: parent
                    onDropped: (drag) => {
                        // The DragHandler already moved the 'lampIndicator'.
                        // Its x and y properties are the correct new coordinates.
                        lampModel.updateLampPosition(lampIndicator.lampId, lampIndicator.x, lampIndicator.y);
                    }
                }
            }
        }
    }

    Button {
        id: previewButton
        text: "Toggle Preview (Lamp 1 & 3)"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            if (lampModel.previewLampIds.length > 0) {
                lampModel.previewLampIds = [];
            } else {
                lampModel.previewLampIds = [1, 3];
            }
        }
    }
}
