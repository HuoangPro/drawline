import QtQuick 2.14
import QtQuick.Window 2.14

Window {

    property int squareWitdh: 20
    property int dimension: UI.dimension
    property var gridData: UI.grid
    width: Math.max(100, squareWitdh + dimension * squareWitdh)
    height: width
    visible: true
    title: "RobotDraw"

    Item {
        anchors.fill: parent
        visible: squareWitdh >= 20
        Row {
            x: 20
            Repeater {
                model: dimension
                Text {
                    text: index
                    color: "black"
                    width: squareWitdh
                    height: 20
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Column {
            y: 20
            Repeater {
                model: dimension
                Text {
                    text: index
                    color: "black"
                    width: 20
                    height: squareWitdh
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    Item {
        x: 20
        y: 20
        width: dimension * squareWitdh
        height: width

        Grid {
            anchors.centerIn: parent
            columns: dimension

            Repeater {
                model: dimension * dimension
                Rectangle {
                    width: squareWitdh
                    height: squareWitdh
                    color: "transparent"
                    border.color: "black"
                    border.width: 1
                    property bool isShow: gridData[Math.floor(index / dimension)][index % dimension]
                    Rectangle {
                        anchors.centerIn: parent
                        width: squareWitdh/3
                        height: width
                        radius: width/2
                        color: "red"
                        visible: isShow
                    }
                }
            }
        }

        Repeater {
            anchors.fill: parent
            model: UI.lines.length
            Canvas {
                id: canvas
                anchors.fill: parent
                property int startX: UI.lines[index][1] * squareWitdh + squareWitdh/2
                property int startY: UI.lines[index][0] * squareWitdh + squareWitdh/2
                property int endX: UI.lines[index][3] * squareWitdh + squareWitdh/2
                property int endY: UI.lines[index][2] * squareWitdh + squareWitdh/2

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    ctx.beginPath();
                    ctx.moveTo(startX, startY);
                    ctx.lineTo(endX, endY);
                    ctx.lineWidth = 2;
                    ctx.strokeStyle = "blue";
                    ctx.stroke();
                }
            }
        }
    }

}
