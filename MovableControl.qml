import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

Rectangle {
    id: root
    width: 250
    height: 200
    color: "#1a1f2e"
    border.color: isDragging ? "#ffff00" : "#00ffff"
    border.width: 2
    radius: 8
    
    property string controlType: "Gauge"
    property bool isDragging: false
    property bool isMinimized: false
    property bool isCloseable: true
    property bool isMinimizable: true
    property bool isMaximizable: true
    property real normalWidth: 250
    property real normalHeight: 200
    property bool isMaximized: false
    
    // Drag functionality
    MouseArea {
        id: dragArea
        anchors.fill: titleBar
        drag.target: root
        drag.axis: Drag.XAndYAxis
        
        onPressed: {
            root.isDragging = true
            root.z = 1000 // Bring to front
        }
        
        onReleased: {
            root.isDragging = false
            root.z = 1
        }
        
        onDoubleClicked: {
            if (isMaximizable) {
                toggleMaximize()
            }
        }
    }
    
    // Title bar
    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        color: "#2a2f3e"
        radius: 6
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            
            Text {
                text: controlType + " Control"
                color: "#00ffff"
                font.pixelSize: 11
                font.bold: true
                Layout.fillWidth: true
            }
            
            // Control buttons
            Row {
                spacing: 2
                
                Button {
                    visible: isMinimizable
                    width: 20
                    height: 20
                    text: isMinimized ? "+" : "−"
                    font.pixelSize: 10
                    background: Rectangle {
                        color: parent.hovered ? "#444" : "transparent"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 2
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 10
                    }
                    onClicked: toggleMinimize()
                }
                
                Button {
                    visible: isMaximizable
                    width: 20
                    height: 20
                    text: isMaximized ? "⧉" : "⬜"
                    font.pixelSize: 8
                    background: Rectangle {
                        color: parent.hovered ? "#444" : "transparent"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 2
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 8
                    }
                    onClicked: toggleMaximize()
                }
                
                Button {
                    visible: isCloseable
                    width: 20
                    height: 20
                    text: "×"
                    font.pixelSize: 10
                    background: Rectangle {
                        color: parent.hovered ? "#ff4444" : "transparent"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 2
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.parent.hovered ? "#ffffff" : "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 10
                    }
                    onClicked: root.destroy()
                }
            }
        }
    }
    
    // Content area
    Rectangle {
        id: contentArea
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 2
        color: "#0f1419"
        radius: 4
        visible: !isMinimized
        
        // Dynamic content based on control type
        Loader {
            id: contentLoader
            anchors.fill: parent
            anchors.margins: 5
            
            sourceComponent: {
                switch(controlType) {
                    case "Gauge": return gaugeComponent
                    case "Chart": return chartComponent
                    case "Slider": return sliderComponent
                    case "Display": return displayComponent
                    case "Button Panel": return buttonPanelComponent
                    default: return gaugeComponent
                }
            }
        }
    }
    
    // Gauge Component
    Component {
        id: gaugeComponent
        Item {
            Rectangle {
                anchors.centerIn: parent
                width: Math.min(parent.width, parent.height) - 20
                height: width
                color: "transparent"
                border.color: "#00ffff"
                border.width: 2
                radius: width / 2
                
                Canvas {
                    id: gaugeCanvas
                    anchors.fill: parent
                    property real value: Math.random() * 100
                    
                    Timer {
                        interval: 1000
                        running: true
                        repeat: true
                        onTriggered: {
                            gaugeCanvas.value = Math.random() * 100
                            gaugeCanvas.requestPaint()
                        }
                    }
                    
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.reset()
                        
                        var centerX = width / 2
                        var centerY = height / 2
                        var radius = Math.min(width, height) / 2 - 10
                        
                        // Draw arc
                        ctx.beginPath()
                        ctx.arc(centerX, centerY, radius, Math.PI, 2 * Math.PI)
                        ctx.strokeStyle = "#004d4d"
                        ctx.lineWidth = 8
                        ctx.stroke()
                        
                        // Draw value arc
                        var angle = Math.PI + (value / 100) * Math.PI
                        ctx.beginPath()
                        ctx.arc(centerX, centerY, radius, Math.PI, angle)
                        ctx.strokeStyle = "#00ffff"
                        ctx.lineWidth = 8
                        ctx.stroke()
                    }
                }
                
                Text {
                    anchors.centerIn: parent
                    text: Math.round(gaugeCanvas.value) + "%"
                    color: "#00ffff"
                    font.pixelSize: 16
                    font.bold: true
                }
            }
        }
    }
    
    // Chart Component
    Component {
        id: chartComponent
        ChartView {
            antialiasing: true
            theme: ChartView.ChartThemeDark
            backgroundColor: "#0f1419"
            
            LineSeries {
                id: lineSeries
                name: "Data"
                color: "#00ffff"
            }
            
            ValueAxis {
                id: xAxis
                min: 0
                max: 10
            }
            
            ValueAxis {
                id: yAxis
                min: 0
                max: 100
            }
            
            Component.onCompleted: {
                lineSeries.axisX = xAxis
                lineSeries.axisY = yAxis
                
                // Add some sample data
                for (var i = 0; i <= 10; i++) {
                    lineSeries.append(i, Math.random() * 100)
                }
            }
            
            Timer {
                interval: 2000
                running: true
                repeat: true
                onTriggered: {
                    lineSeries.clear()
                    for (var i = 0; i <= 10; i++) {
                        lineSeries.append(i, Math.random() * 100)
                    }
                }
            }
        }
    }
    
    // Slider Component
    Component {
        id: sliderComponent
        ColumnLayout {
            spacing: 10
            anchors.centerIn: parent
            
            Text {
                text: "Control Panel"
                color: "#00ffff"
                font.pixelSize: 14
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }
            
            Slider {
                id: slider1
                from: 0
                to: 100
                value: 50
                Layout.fillWidth: true
                
                background: Rectangle {
                    x: slider1.leftPadding
                    y: slider1.topPadding + slider1.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width: slider1.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "#004d4d"
                    
                    Rectangle {
                        width: slider1.visualPosition * parent.width
                        height: parent.height
                        color: "#00ffff"
                        radius: 2
                    }
                }
                
                handle: Rectangle {
                    x: slider1.leftPadding + slider1.visualPosition * (slider1.availableWidth - width)
                    y: slider1.topPadding + slider1.availableHeight / 2 - height / 2
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 8
                    color: "#00ffff"
                    border.color: "#ffffff"
                }
            }
            
            Text {
                text: "Value: " + Math.round(slider1.value)
                color: "#00ffff"
                font.pixelSize: 12
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
    
    // Display Component
    Component {
        id: displayComponent
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 5
            
            Text {
                text: "System Status"
                color: "#00ffff"
                font.pixelSize: 14
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#001a1a"
                border.color: "#00ffff"
                border.width: 1
                
                Column {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4
                    
                    Repeater {
                        model: ["CPU: 45%", "Memory: 67%", "Network: OK", "Status: Active"]
                        Text {
                            text: modelData
                            color: "#00ffff"
                            font.pixelSize: 11
                        }
                    }
                }
            }
        }
    }
    
    // Button Panel Component
    Component {
        id: buttonPanelComponent
        GridLayout {
            anchors.fill: parent
            anchors.margins: 10
            columns: 2
            
            Repeater {
                model: ["Start", "Stop", "Reset", "Config"]
                Button {
                    text: modelData
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    background: Rectangle {
                        color: parent.pressed ? "#006666" : (parent.hovered ? "#004d4d" : "#003333")
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 4
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 11
                    }
                }
            }
        }
    }
    
    // Window management functions
    function toggleMinimize() {
        isMinimized = !isMinimized
        if (isMinimized) {
            normalHeight = height
            height = 30
        } else {
            height = normalHeight
        }
    }
    
    function toggleMaximize() {
        if (isMaximized) {
            width = normalWidth
            height = normalHeight
            isMaximized = false
        } else {
            normalWidth = width
            normalHeight = height
            width = parent.width - 20
            height = parent.height - 20
            x = 10
            y = 10
            isMaximized = true
        }
    }
    
    // Resize handles
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 15
        height: 15
        color: "#00ffff"
        opacity: 0.3
        
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeFDiagCursor
            
            property point startPos
            property size startSize
            
            onPressed: {
                startPos = Qt.point(mouseX, mouseY)
                startSize = Qt.size(root.width, root.height)
            }
            
            onPositionChanged: {
                if (pressed) {
                    root.width = Math.max(100, startSize.width + (mouseX - startPos.x))
                    root.height = Math.max(60, startSize.height + (mouseY - startPos.y))
                }
            }
        }
    }
}