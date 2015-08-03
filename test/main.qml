import QtQuick 2.4
import QtQuick.Window 2.2

Window {
    visible: true
    width: 800
    height: 480

    MultiPointTouchArea {
        anchors.fill: parent
        touchPoints: [
            TouchPoint { id: point1 },
            TouchPoint { id: point2 },
            TouchPoint { id: point3 },
            TouchPoint { id: point4 }
        ]
    }

    Rectangle {
        visible: point1.pressed
        width: 16; height: 16; radius: 8
        x: point1.x; y: point1.y
        color: "gray"
        Text { anchors.centerIn: parent; text: "1"; color: "white" }
    }

    Rectangle {
        visible: point2.pressed
        width: 16; height: 16; radius: 8
        x: point2.x; y: point2.y
        color: "gray"
        Text { anchors.centerIn: parent; text: "2"; color: "white" }
    }

    Rectangle {
        visible: point3.pressed
        width: 16; height: 16; radius: 8
        x: point3.x; y: point3.y
        color: "gray"
        Text { anchors.centerIn: parent; text: "3"; color: "white" }
    }

    Rectangle {
        visible: point4.pressed
        width: 16; height: 16; radius: 8
        x: point4.x; y: point4.y
        color: "gray"
        Text { anchors.centerIn: parent; text: "4"; color: "white" }
    }
}
