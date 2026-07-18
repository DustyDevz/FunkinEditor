import QtQuick

Window {
    color: "transparent"
    visible: true

    Column {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        spacing: 5

        Text { text: "FPS: " + Debug.fps; color: "white" }
        Text { text: "Memory: " + Debug.memoryUsage.toFixed(2) + " MB"; color: "white" }
    }
}