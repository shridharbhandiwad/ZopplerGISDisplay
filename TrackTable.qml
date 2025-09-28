import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#1a1f2e"
    
    property var trackData: []
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#2a2f3e"
            border.color: "#00ffff"
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                
                Text {
                    text: "Track Table"
                    color: "#00ffff"
                    font.pixelSize: 14
                    font.bold: true
                    Layout.fillWidth: true
                }
                
                Button {
                    text: "Add Track"
                    font.pixelSize: 10
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 24
                    
                    background: Rectangle {
                        color: parent.hovered ? "#004d4d" : "#003333"
                        border.color: "#00ffff"
                        border.width: 1
                        radius: 3
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "#00ffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 10
                    }
                    
                    onClicked: addNewTrack()
                }
                
                Button {
                    text: "Clear"
                    font.pixelSize: 10
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 24
                    
                    background: Rectangle {
                        color: parent.hovered ? "#664444" : "#443333"
                        border.color: "#ff6666"
                        border.width: 1
                        radius: 3
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "#ff6666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 10
                    }
                    
                    onClicked: clearAllTracks()
                }
            }
        }
        
        // Table headers
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "#0f1419"
            border.color: "#00ffff"
            border.width: 1
            
            Row {
                anchors.fill: parent
                
                TableHeaderItem {
                    width: parent.width * 0.15
                    text: "ID"
                }
                TableHeaderItem {
                    width: parent.width * 0.2
                    text: "Type"
                }
                TableHeaderItem {
                    width: parent.width * 0.15
                    text: "Status"
                }
                TableHeaderItem {
                    width: parent.width * 0.2
                    text: "Position"
                }
                TableHeaderItem {
                    width: parent.width * 0.15
                    text: "Speed"
                }
                TableHeaderItem {
                    width: parent.width * 0.15
                    text: "Actions"
                }
            }
        }
        
        // Table content
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ListView {
                id: trackListView
                model: trackData
                delegate: trackDelegate
                
                Component {
                    id: trackDelegate
                    
                    Rectangle {
                        width: trackListView.width
                        height: 35
                        color: index % 2 === 0 ? "#0f1419" : "#1a1f2e"
                        border.color: "#004d4d"
                        border.width: 1
                        
                        Row {
                            anchors.fill: parent
                            
                            TableCellItem {
                                width: parent.width * 0.15
                                text: modelData.id
                            }
                            TableCellItem {
                                width: parent.width * 0.2
                                text: modelData.type
                                color: getTypeColor(modelData.type)
                            }
                            TableCellItem {
                                width: parent.width * 0.15
                                text: modelData.status
                                color: getStatusColor(modelData.status)
                            }
                            TableCellItem {
                                width: parent.width * 0.2
                                text: modelData.position
                            }
                            TableCellItem {
                                width: parent.width * 0.15
                                text: modelData.speed + " kts"
                            }
                            
                            // Actions
                            Item {
                                width: parent.width * 0.15
                                height: parent.height
                                
                                Row {
                                    anchors.centerIn: parent
                                    spacing: 2
                                    
                                    Button {
                                        width: 20
                                        height: 20
                                        text: "👁"
                                        font.pixelSize: 8
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? "#004d4d" : "transparent"
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
                                        
                                        onClicked: viewTrack(index)
                                    }
                                    
                                    Button {
                                        width: 20
                                        height: 20
                                        text: "×"
                                        font.pixelSize: 10
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? "#ff4444" : "transparent"
                                            border.color: "#ff6666"
                                            border.width: 1
                                            radius: 2
                                        }
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#ff6666"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            font.pixelSize: 10
                                        }
                                        
                                        onClicked: removeTrack(index)
                                    }
                                }
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: selectTrack(index)
                            onDoubleClicked: editTrack(index)
                        }
                    }
                }
            }
        }
        
        // Status bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 25
            color: "#2a2f3e"
            border.color: "#00ffff"
            border.width: 1
            
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 8
                text: "Total Tracks: " + trackData.length
                color: "#00ffff"
                font.pixelSize: 10
            }
            
            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 8
                text: getCurrentTime()
                color: "#00ffff"
                font.pixelSize: 10
            }
        }
    }
    
    // Table header component
    component TableHeaderItem: Rectangle {
        property string text: ""
        height: parent.height
        color: "#2a2f3e"
        border.color: "#00ffff"
        border.width: 1
        
        Text {
            anchors.centerIn: parent
            text: parent.text
            color: "#00ffff"
            font.pixelSize: 11
            font.bold: true
        }
    }
    
    // Table cell component
    component TableCellItem: Rectangle {
        property string text: ""
        property color color: "#00ffff"
        height: parent.height
        color: "transparent"
        
        Text {
            anchors.centerIn: parent
            text: parent.text
            color: parent.color
            font.pixelSize: 10
            elide: Text.ElideRight
        }
    }
    
    // Timer for real-time updates
    Timer {
        id: updateTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered: updateTrackData()
    }
    
    // Functions
    function addNewTrack() {
        var newTrack = {
            id: "T" + (trackData.length + 1).toString().padStart(3, '0'),
            type: getRandomType(),
            status: "Active",
            position: generateRandomPosition(),
            speed: Math.floor(Math.random() * 50) + 10
        }
        
        trackData.push(newTrack)
        trackData = trackData // Trigger update
    }
    
    function clearAllTracks() {
        trackData = []
    }
    
    function removeTrack(index) {
        trackData.splice(index, 1)
        trackData = trackData // Trigger update
    }
    
    function selectTrack(index) {
        console.log("Selected track:", trackData[index].id)
    }
    
    function editTrack(index) {
        console.log("Edit track:", trackData[index].id)
    }
    
    function viewTrack(index) {
        console.log("View track:", trackData[index].id)
    }
    
    function updateTrackData() {
        for (var i = 0; i < trackData.length; i++) {
            // Randomly update speed and position
            if (Math.random() < 0.3) {
                trackData[i].speed = Math.floor(Math.random() * 50) + 10
                trackData[i].position = generateRandomPosition()
            }
            
            // Randomly change status
            if (Math.random() < 0.1) {
                var statuses = ["Active", "Inactive", "Warning", "Lost"]
                trackData[i].status = statuses[Math.floor(Math.random() * statuses.length)]
            }
        }
        trackData = trackData // Trigger update
    }
    
    function getRandomType() {
        var types = ["Aircraft", "Ship", "Vehicle", "Unknown", "Missile", "Drone"]
        return types[Math.floor(Math.random() * types.length)]
    }
    
    function generateRandomPosition() {
        var lat = (Math.random() * 180 - 90).toFixed(3)
        var lon = (Math.random() * 360 - 180).toFixed(3)
        return lat + "°, " + lon + "°"
    }
    
    function getTypeColor(type) {
        switch(type) {
            case "Aircraft": return "#00ff00"
            case "Ship": return "#0099ff"
            case "Vehicle": return "#ffaa00"
            case "Unknown": return "#ff6666"
            case "Missile": return "#ff0000"
            case "Drone": return "#aa00ff"
            default: return "#00ffff"
        }
    }
    
    function getStatusColor(status) {
        switch(status) {
            case "Active": return "#00ff00"
            case "Inactive": return "#666666"
            case "Warning": return "#ffaa00"
            case "Lost": return "#ff0000"
            default: return "#00ffff"
        }
    }
    
    function getCurrentTime() {
        var now = new Date()
        return now.toLocaleTimeString()
    }
    
    Component.onCompleted: {
        // Initialize with some sample data
        for (var i = 0; i < 5; i++) {
            addNewTrack()
        }
    }
}