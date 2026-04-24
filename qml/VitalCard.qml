import QtQuick
import QtQuick.Controls

Rectangle {
    width: 180
    height: 140
    color: "transparent"
    border.color: "#cccccc"

    property string vital_name: "HR"
    property string vital_value: "--"
    property string vital_units: "bpm"

    Column {
        anchors.centerIn: parent
        spacing: 6

        Text { text: vital_name }
        Text { text: vital_value; font.pixelSize: 36 }
        Text { text: vital_units }
    }
}
