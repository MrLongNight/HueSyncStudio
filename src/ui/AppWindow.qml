import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: "HueSyncStudio"

    header: MenuBar {
        Menu {
            title: "View"
            Action {
                text: "Effect/Control Panel"
                checkable: true
                checked: true
                onTriggered: effectPanelItem.visible = !effectPanelItem.visible
            }
            Action {
                text: "Live FFT Viewer"
                checkable: true
                checked: true
                onTriggered: audioViewItem.visible = !audioViewItem.visible
            }
            Action {
                text: "2D/3D Map & Control"
                checkable: true
                checked: true
                onTriggered: lampMapItem.visible = !lampMapItem.visible
            }
        }
    }

    SplitView {
        id: mainSplitView
        anchors.fill: parent

        EffectPanel {
            id: effectPanelItem
            Layout.minimumWidth: 250
            Layout.preferredWidth: 350
        }

        SplitView {
            id: rightSplitView
            orientation: Qt.Vertical
            Layout.fillWidth: true

            AudioView {
                id: audioViewItem
                Layout.minimumHeight: 200
                Layout.preferredHeight: 300
            }

            LampMap {
                id: lampMapItem
                Layout.fillHeight: true
            }
        }
    }
}
