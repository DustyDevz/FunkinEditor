import QtQuick
import QtQuick.Controls
import Funkin.Render 1.0

Window {
    width: 1280
    height: 720
    visible: true

    Render {
        id: viewport
        objectName: "viewport"
        anchors.fill: parent
    }

    Column {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        spacing: 5

        Text {
            text: "FPS: " + Debug.fps
            color: "white"
        }

        Text {
            text: "Memory: " + Debug.memoryUsage.toFixed(2) + " MB"
            color: "white"
        }
    }
}