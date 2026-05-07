import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Root ApplicationWindow for the MedTech Clinician Dashboard (Stage 1).
 *
 * Binds all displayed values to the C++ UIModel object exposed via
 * setContextProperty("uiModel", ...).
 */
ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 600
    title: "MedTech Clinician Dashboard"

    background: Rectangle { color: "#1a1a2e" }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            visible: uiModel && uiModel.has_global_error
            color: "#7A0000"
            radius: 6
            border.color: "#FF6666"
            border.width: 1

            Text {
                anchors.fill: parent
                anchors.margins: 12
                verticalAlignment: Text.AlignVCenter
                text: uiModel ? uiModel.global_error : ""
                color: "#FFFFFF"
                wrapMode: Text.Wrap
                font.pixelSize: 16
                font.bold: true
            }
        }

        // ── Row 1: Connection status ────────────────────────────────────────
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: uiModel ? uiModel.status : "No Data"
            font.pixelSize: 32
            font.bold: true
            color: {
                var s = uiModel ? uiModel.status : ""
                if (s === "Connected")    return "#00AA00"
                if (s === "Connecting...") return "#FFAA00"
                if (s === "Stale Data")   return "#FFAA00"
                return "#AA0000"
            }
        }

        // ── Row 2: Vital sign cards ─────────────────────────────────────────
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 20

            VitalCard {
                vital_name:  "HR"
                vital_value: uiModel ? uiModel.hr_value : "--"
                vital_units: "bpm"
            }
            VitalCard {
                vital_name:  "BP"
                vital_value: uiModel ? uiModel.bp_value : "--"
                vital_units: "mmHg"
            }
            VitalCard {
                vital_name:  "O2"
                vital_value: uiModel ? uiModel.o2_value : "--"
                vital_units: "%"
            }
            VitalCard {
                vital_name:  "Temp"
                vital_value: uiModel ? uiModel.temp_value : "--"
                vital_units: "\u00B0C"
            }
        }

        // ── Row 3: Quality + footer ─────────────────────────────────────────
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 40

            Text {
                text: "Quality: " + (uiModel ? uiModel.quality_value : "--")
                font.pixelSize: 18
                color: "#aaaaaa"
            }
            Text {
                text: "Last update: " + (uiModel ? uiModel.last_update : "--")
                font.pixelSize: 18
                color: "#aaaaaa"
            }
        }

        // Spacer
        Item { Layout.fillHeight: true }
    }
}
