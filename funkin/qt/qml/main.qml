import QtQuick
import QtQuick.Controls
import Funkin.Render 1.0

Window {
    width: 1280
    height: 720
    visible: true
    title: "FunkinC++ Editor"
	
    RenderViewport {
        id: gameViewport
        anchors.fill: parent
    }
	
    Text {
        text: "FPS: 60"
        color: "white"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
    }
}