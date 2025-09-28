import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    title: "Create New Control"
    modal: true
    
    property var controlContainer: null
    property var workspace: null
    
    width: 400
    height: 500
    
    background: Rectangle {
        color: "#1a1f2e"
        border.color: "#00ffff"
        border.width: 2
        radius: 8
    }
    
    header: Rectangle {
        height: 40
        color: "#2a2f3e"
        radius: 6
        
        Text {
            anchors.centerIn: parent
            text: "Create New Control"
            color: "#00ffff"
            font.pixelSize: 16
            font.bold: true
        }
    }
    
    contentItem: ScrollView {
        clip: true
        
        ColumnLayout {
            width: parent.width
            spacing: 15
            
            // Control Type Selection
            GroupBox {
                Layout.fillWidth: true
                title: "Control Type"
                
                background: Rectangle {
                    color: "#0f1419"
                    border.color: "#00ffff"
                    border.width: 1
                    radius: 4
                }
                
                label: Text {
                    text: parent.title
                    color: "#00ffff"
                    font.pixelSize: 12
                    font.bold: true
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8
                    
                    ButtonGroup {
                        id: controlTypeGroup
                    }
                    
                    Repeater {
                        model: [
                            {type: "Gauge", desc: "Circular gauge with real-time values"},
                            {type: "Chart", desc: "Line chart with dynamic data"},
                            {type: "Slider", desc: "Interactive slider controls"},
                            {type: "Display", desc: "Information display panel"},
                            {type: "Button Panel", desc: "Grid of action buttons"},
                            {type: "Custom", desc: "Create a custom control type"}
                        ]
                        
                        RadioButton {
                            text: modelData.type
                            checked: index === 0
                            ButtonGroup.group: controlTypeGroup
                            
                            background: Rectangle {
                                color: parent.checked ? "#003333" : "transparent"
                                border.color: "#00ffff"
                                border.width: 1
                                radius: 3
                            }
                            
                            contentItem: Column {
                                leftPadding: 30
                                
                                Text {
                                    text: parent.text
                                    color: "#00ffff"
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                                
                                Text {
                                    text: modelData.desc
                                    color: "#aaaaaa"
                                    font.pixelSize: 10
                                    wrapMode: Text.WordWrap
                                    width: 300
                                }
                            }
                            
                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                x: parent.leftPadding
                                y: parent.height / 2 - height / 2
                                radius: 8
                                color: "transparent"
                                border.color: "#00ffff"
                                border.width: 2
                                
                                Rectangle {
                                    width: 8
                                    height: 8
                                    x: 4
                                    y: 4
                                    radius: 4
                                    color: "#00ffff"
                                    visible: parent.parent.checked
                                }
                            }
                        }
                    }
                }
            }
            
            // Control Properties
            GroupBox {
                Layout.fillWidth: true
                title: "Control Properties"
                
                background: Rectangle {
                    color: "#0f1419"
                    border.color: "#00ffff"
                    border.width: 1
                    radius: 4
                }
                
                label: Text {
                    text: parent.title
                    color: "#00ffff"
                    font.pixelSize: 12
                    font.bold: true
                }
                
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: 10
                    rowSpacing: 8
                    
                    Text {
                        text: "Name:"
                        color: "#00ffff"
                        font.pixelSize: 11
                    }
                    
                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        text: "New Control"
                        color: "#00ffff"
                        font.pixelSize: 11
                        
                        background: Rectangle {
                            color: "#0a0d1a"
                            border.color: "#00ffff"
                            border.width: 1
                            radius: 3
                        }
                    }
                    
                    Text {
                        text: "Width:"
                        color: "#00ffff"
                        font.pixelSize: 11
                    }
                    
                    SpinBox {
                        id: widthSpinBox
                        from: 100
                        to: 800
                        value: 250
                        
                        background: Rectangle {
                            color: "#0a0d1a"
                            border.color: "#00ffff"
                            border.width: 1
                            radius: 3
                        }
                        
                        contentItem: TextInput {
                            text: parent.textFromValue(parent.value, parent.locale)
                            font.pixelSize: 11
                            color: "#00ffff"
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                        }
                    }
                    
                    Text {
                        text: "Height:"
                        color: "#00ffff"
                        font.pixelSize: 11
                    }
                    
                    SpinBox {
                        id: heightSpinBox
                        from: 60
                        to: 600
                        value: 200
                        
                        background: Rectangle {
                            color: "#0a0d1a"
                            border.color: "#00ffff"
                            border.width: 1
                            radius: 3
                        }
                        
                        contentItem: TextInput {
                            text: parent.textFromValue(parent.value, parent.locale)
                            font.pixelSize: 11
                            color: "#00ffff"
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                        }
                    }
                }
            }
            
            // Control Features
            GroupBox {
                Layout.fillWidth: true
                title: "Features"
                
                background: Rectangle {
                    color: "#0f1419"
                    border.color: "#00ffff"
                    border.width: 1
                    radius: 4
                }
                
                label: Text {
                    text: parent.title
                    color: "#00ffff"
                    font.pixelSize: 12
                    font.bold: true
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8
                    
                    CheckBox {
                        id: closeableCheck
                        text: "Closeable"
                        checked: true
                        
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            x: parent.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 2
                            color: "transparent"
                            border.color: "#00ffff"
                            border.width: 2
                            
                            Text {
                                anchors.centerIn: parent
                                text: "✓"
                                color: "#00ffff"
                                font.pixelSize: 12
                                visible: parent.parent.checked
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#00ffff"
                            font.pixelSize: 11
                            leftPadding: parent.indicator.width + parent.spacing
                        }
                    }
                    
                    CheckBox {
                        id: minimizableCheck
                        text: "Minimizable"
                        checked: true
                        
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            x: parent.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 2
                            color: "transparent"
                            border.color: "#00ffff"
                            border.width: 2
                            
                            Text {
                                anchors.centerIn: parent
                                text: "✓"
                                color: "#00ffff"
                                font.pixelSize: 12
                                visible: parent.parent.checked
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#00ffff"
                            font.pixelSize: 11
                            leftPadding: parent.indicator.width + parent.spacing
                        }
                    }
                    
                    CheckBox {
                        id: maximizableCheck
                        text: "Maximizable"
                        checked: true
                        
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            x: parent.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 2
                            color: "transparent"
                            border.color: "#00ffff"
                            border.width: 2
                            
                            Text {
                                anchors.centerIn: parent
                                text: "✓"
                                color: "#00ffff"
                                font.pixelSize: 12
                                visible: parent.parent.checked
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#00ffff"
                            font.pixelSize: 11
                            leftPadding: parent.indicator.width + parent.spacing
                        }
                    }
                    
                    CheckBox {
                        id: resizableCheck
                        text: "Resizable"
                        checked: true
                        
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            x: parent.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 2
                            color: "transparent"
                            border.color: "#00ffff"
                            border.width: 2
                            
                            Text {
                                anchors.centerIn: parent
                                text: "✓"
                                color: "#00ffff"
                                font.pixelSize: 12
                                visible: parent.parent.checked
                            }
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#00ffff"
                            font.pixelSize: 11
                            leftPadding: parent.indicator.width + parent.spacing
                        }
                    }
                }
            }
        }
    }
    
    footer: DialogButtonBox {
        background: Rectangle {
            color: "#2a2f3e"
            radius: 4
        }
        
        Button {
            text: "Create"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            
            background: Rectangle {
                color: parent.hovered ? "#006666" : "#004d4d"
                border.color: "#00ffff"
                border.width: 2
                radius: 4
            }
            
            contentItem: Text {
                text: parent.text
                color: "#00ffff"
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        
        Button {
            text: "Cancel"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            
            background: Rectangle {
                color: parent.hovered ? "#664444" : "#443333"
                border.color: "#ff6666"
                border.width: 2
                radius: 4
            }
            
            contentItem: Text {
                text: parent.text
                color: "#ff6666"
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
    
    onAccepted: {
        createControl()
    }
    
    function createControl() {
        if (!controlContainer || !workspace) {
            console.log("Error: controlContainer or workspace not set")
            return
        }
        
        var selectedType = getSelectedControlType()
        var component = Qt.createComponent("MovableControl.qml")
        
        if (component.status === Component.Ready) {
            var control = component.createObject(controlContainer, {
                "controlType": selectedType,
                "width": widthSpinBox.value,
                "height": heightSpinBox.value,
                "x": Math.random() * (workspace.width - widthSpinBox.value),
                "y": Math.random() * (workspace.height - heightSpinBox.value),
                "isCloseable": closeableCheck.checked,
                "isMinimizable": minimizableCheck.checked,
                "isMaximizable": maximizableCheck.checked
            })
            
            if (control) {
                console.log("Created new control:", selectedType)
            }
        } else {
            console.log("Error creating control:", component.errorString())
        }
    }
    
    function getSelectedControlType() {
        var buttons = controlTypeGroup.buttons
        for (var i = 0; i < buttons.length; i++) {
            if (buttons[i].checked) {
                return buttons[i].text
            }
        }
        return "Gauge" // Default
    }
}