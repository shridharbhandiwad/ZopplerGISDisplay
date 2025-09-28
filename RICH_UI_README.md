# Rich QML/QtQuick UI Application

A sophisticated QML/QtQuick application featuring movable controls, real-time data visualization, and comprehensive window management capabilities.

## Features

### 🎛️ **Movable Controls**
- **Drag & Drop**: All controls can be moved around the workspace by dragging their title bars
- **Resizable**: Controls can be resized using the resize handle in the bottom-right corner
- **Multiple Types**: 
  - **Gauge**: Circular gauge with real-time animated values
  - **Chart**: Dynamic line chart with updating data series
  - **Slider**: Interactive slider control panel
  - **Display**: System status information panel
  - **Button Panel**: Grid of interactive action buttons

### 🪟 **Window Management**
- **Closeable**: Each control has a close button (×) to remove it
- **Minimizable**: Minimize controls to title bar only (−)
- **Maximizable**: Expand controls to fill workspace (⬜/⧉)
- **Main Window**: Full window management with custom title bar

### 📊 **Track Table**
- **Real-time Data**: Live updating track information
- **Interactive**: Click to select, double-click to edit tracks
- **Sortable Columns**: ID, Type, Status, Position, Speed, Actions
- **Color-coded Status**: Different colors for Active, Inactive, Warning, Lost
- **Add/Remove**: Dynamic track management with Add and Clear buttons

### 🏭 **Control Factory**
- **Create New Control**: Advanced dialog for creating custom controls
- **Configuration Options**:
  - Control type selection with descriptions
  - Custom name and dimensions
  - Feature toggles (closeable, minimizable, maximizable, resizable)
- **Real-time Preview**: Immediate creation with specified properties

## File Structure

```
/workspace/
├── RichMainView.qml          # Main application window
├── MovableControl.qml        # Draggable control component
├── TrackTable.qml           # Real-time data table
├── ControlFactory.qml       # Control creation dialog
├── DemoApp.cpp              # Demo application entry point
├── RichUIDemo.pro          # Qt project file for demo
├── myRes.qrc               # Resource file
└── RICH_UI_README.md       # This documentation
```

## Building and Running

### Prerequisites
- Qt 5.15 or later
- Qt Charts module
- Qt Quick Controls 2
- C++17 compatible compiler

### Build Instructions

#### Option 1: Demo Application
```bash
cd /workspace
qmake RichUIDemo.pro
make
./RichUIDemo
```

#### Option 2: Integration with Existing Project
Add the QML files to your existing Qt project's resource file:
```xml
<RCC>
    <qresource prefix="/qml">
        <file>RichMainView.qml</file>
        <file>MovableControl.qml</file>
        <file>TrackTable.qml</file>
        <file>ControlFactory.qml</file>
    </qresource>
</RCC>
```

Load in your C++ application:
```cpp
QQmlApplicationEngine engine;
engine.load(QUrl(QStringLiteral("qrc:/qml/RichMainView.qml")));
```

## Usage Guide

### Creating New Controls
1. Click the **"Create New Control"** button in the control panel
2. Select the desired control type from the radio buttons
3. Configure the control properties:
   - Set a custom name
   - Adjust width and height
   - Toggle features (closeable, minimizable, etc.)
4. Click **"Create"** to add the control to the workspace

### Managing Controls
- **Move**: Drag the title bar to reposition
- **Resize**: Drag the resize handle (bottom-right corner)
- **Minimize**: Click the minimize button (−)
- **Maximize**: Click the maximize button (⬜)
- **Close**: Click the close button (×)

### Track Table Operations
- **Add Track**: Click "Add Track" to create a new random track
- **Remove Track**: Click the × button in the Actions column
- **View Track**: Click the 👁 button to view details
- **Clear All**: Click "Clear" to remove all tracks

## Customization

### Adding New Control Types
To add a new control type, modify `MovableControl.qml`:

1. Add a new component in the components section:
```qml
Component {
    id: newControlComponent
    // Your custom control implementation
}
```

2. Update the switch statement in the Loader:
```qml
case "NewType": return newControlComponent
```

3. Add the new type to `ControlFactory.qml` in the model array.

### Styling
The application uses a dark theme with cyan (#00ffff) accents. Colors can be customized by modifying the color properties throughout the QML files.

### Data Integration
Replace the mock data in `TrackTable.qml` with your real data sources by:
1. Exposing C++ data models to QML
2. Connecting to real-time data feeds
3. Implementing proper data binding

## Technical Details

### Architecture
- **Component-based**: Modular QML components for reusability
- **Property Binding**: Reactive UI updates through Qt's property system
- **Dynamic Creation**: Runtime control instantiation using Qt.createComponent()
- **Resource Management**: Efficient cleanup with automatic object destruction

### Performance
- **Optimized Rendering**: Uses Qt Quick's scene graph for smooth animations
- **Efficient Updates**: Timer-based updates for real-time data
- **Memory Management**: Automatic garbage collection for destroyed controls

### Compatibility
- **Qt 5.15+**: Tested with Qt 5.15 and Qt 6.x
- **Cross-platform**: Works on Windows, Linux, and macOS
- **High DPI**: Scales properly on high-resolution displays

## Troubleshooting

### Common Issues
1. **Controls not appearing**: Check that all QML files are properly included in resources
2. **Drag not working**: Ensure MouseArea is properly configured in title bars
3. **Charts not displaying**: Verify Qt Charts module is installed and imported

### Debug Mode
Enable QML debugging by setting environment variables:
```bash
export QML_IMPORT_TRACE=1
export QT_LOGGING_RULES="qt.qml.debug=true"
```

## Future Enhancements

- [ ] Snap-to-grid functionality
- [ ] Control grouping and ungrouping
- [ ] Save/load workspace layouts
- [ ] More chart types (bar, pie, scatter)
- [ ] Keyboard shortcuts for common actions
- [ ] Undo/redo functionality
- [ ] Multi-monitor support

## License

This code is provided as-is for educational and development purposes. Integrate into your projects as needed.