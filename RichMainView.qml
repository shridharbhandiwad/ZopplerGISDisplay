import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtCharts 2.15

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1400
    height: 900
    title: "Rich UI Control Center"
    
    property bool isMaximized: false
    property bool isMinimized: false
    property real normalWidth: 1400
    property real normalHeight: 900
    property real normalX: 100
    property real normalY: 100
    
    // Custom window flags for frameless window
    flags: Qt.FramelessWindowHint | Qt.Window
    
    color: "#0a0d1a"
    
    // Window drag area
    MouseArea {
        id: titleBarDragArea
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        
        property point lastMousePos: Qt.point(0, 0)
        
        onPressed: {
            lastMousePos = Qt.point(mouseX, mouseY)
        }
        
        onMouseXChanged: {
            if (pressed && !mainWindow.isMaximized) {
                mainWindow.x += (mouseX - lastMousePos.x)
            }
        }
        
        onMouseYChanged: {
            if (pressed && !mainWindow.isMaximized) {
                mainWindow.y += (mouseY - lastMousePos.y)
            }
        }
        
        onDoubleClicked: {
            toggleMaximize()
        }
    }
    
    // Title bar
    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#1a1f2e"
        border.color: "#00ffff"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            
            Text {
                text: mainWindow.title
                color: "#00ffff"
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
            }
            
            // Window control buttons
            Row {
                spacing: 4
                
                Button {
                    id: minimizeBtn
                    width: 30
                    height: 24
                    text: "−"
                    font.pixelSize: 16
                    font.bold: true
                    background: Rectangle {
                        color: parent.hovered ? "#444" : "#2a2f3e"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 3
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: minimizeWindow()
                }
                
                Button {
                    id: maximizeBtn
                    width: 30
                    height: 24
                    text: mainWindow.isMaximized ? "⧉" : "⬜"
                    font.pixelSize: 12
                    background: Rectangle {
                        color: parent.hovered ? "#444" : "#2a2f3e"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 3
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: toggleMaximize()
                }
                
                Button {
                    id: closeBtn
                    width: 30
                    height: 24
                    text: "×"
                    font.pixelSize: 16
                    font.bold: true
                    background: Rectangle {
                        color: parent.hovered ? "#ff4444" : "#2a2f3e"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 3
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.parent.hovered ? "#ffffff" : "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: Qt.quit()
                }
            }
        }
    }
    
    // Main content area
    Rectangle {
        id: contentArea
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#0a0d1a"
        
        // Control panel
        Rectangle {
            id: controlPanel
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 60
            color: "#1a1f2e"
            border.color: "#00ffff"
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                
                Button {
                    text: "Create New Control"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 150
                    background: Rectangle {
                        color: parent.hovered ? "#004d4d" : "#003333"
                        border.color: "#00ffff"
                        border.width: 2
                        radius: 5
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: controlFactory.open()
                }
                
                Item { Layout.fillWidth: true }
                
                Text {
                    text: "Active Controls: " + controlContainer.children.length
                    color: "#00ffff"
                    font.pixelSize: 12
                }
            }
        }
        
        // Main workspace
        Rectangle {
            id: workspace
            anchors.top: controlPanel.bottom
            anchors.left: parent.left
            anchors.right: trackTablePanel.left
            anchors.bottom: parent.bottom
            color: "#0f1419"
            border.color: "#00ffff"
            border.width: 1
            
            // Container for movable controls
            Item {
                id: controlContainer
                anchors.fill: parent
                anchors.margins: 5
            }
            
            // Background grid
            Canvas {
                id: gridCanvas
                anchors.fill: parent
                opacity: 0.1
                
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.strokeStyle = "#00ffff"
                    ctx.lineWidth = 1
                    
                    // Draw grid
                    for (var x = 0; x < width; x += 50) {
                        ctx.beginPath()
                        ctx.moveTo(x, 0)
                        ctx.lineTo(x, height)
                        ctx.stroke()
                    }
                    
                    for (var y = 0; y < height; y += 50) {
                        ctx.beginPath()
                        ctx.moveTo(0, y)
                        ctx.lineTo(width, y)
                        ctx.stroke()
                    }
                }
            }
        }
        
        // Track table panel
        Rectangle {
            id: trackTablePanel
            anchors.top: controlPanel.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: 400
            color: "#1a1f2e"
            border.color: "#00ffff"
            border.width: 1
            
            TrackTable {
                id: trackTable
                anchors.fill: parent
                anchors.margins: 5
            }
        }
    }
    
    // Control Factory Dialog
    ControlFactory {
        id: controlFactory
        controlContainer: controlContainer
        workspace: workspace
    }
    
    // Window management functions
    function minimizeWindow() {
        mainWindow.showMinimized()
        isMinimized = true
    }
    
    function toggleMaximize() {
        if (isMaximized) {
            mainWindow.width = normalWidth
            mainWindow.height = normalHeight
            mainWindow.x = normalX
            mainWindow.y = normalY
            isMaximized = false
        } else {
            normalWidth = mainWindow.width
            normalHeight = mainWindow.height
            normalX = mainWindow.x
            normalY = mainWindow.y
            mainWindow.showMaximized()
            isMaximized = true
        }
    }
    
    // Create new control function
    function createNewControl() {
        var controlTypes = ["Gauge", "Chart", "Slider", "Display", "Button Panel"]
        var randomType = controlTypes[Math.floor(Math.random() * controlTypes.length)]
        
        var component = Qt.createComponent("MovableControl.qml")
        if (component.status === Component.Ready) {
            var control = component.createObject(controlContainer, {
                "controlType": randomType,
                "x": Math.random() * (workspace.width - 200),
                "y": Math.random() * (workspace.height - 150)
            })
        }
    }
    
    Component.onCompleted: {
        // Create some initial controls
        for (var i = 0; i < 3; i++) {
            createNewControl()
        }
    }
}