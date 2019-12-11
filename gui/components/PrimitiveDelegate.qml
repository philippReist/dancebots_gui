import QtQuick 2.13
import dancebots.backend 1.0
import "../GuiStyle"

Rectangle{
  id: root
  height: appWindow.width * Style.primitives.height * Style.timerBar.height
  radius: Style.primitives.radius * height
  border.color: Style.palette.prim_border
  border.width: Style.primitives.borderWidth * height

  property var idleParent: null
  property bool isFromBar: false
  property var primitive: null
  property var dragTarget: null
  property bool showData: false
  property bool dragActive: dragArea.dragActive

  property real frameToPixels: appWindow.frameToPixels

  onFrameToPixelsChanged: updatePrimitive()

  onPrimitiveChanged: updatePrimitive()

  onEnabledChanged: {
    if(enabled){
      color = primitiveColors[primitive.type]
    }else{
      color = Style.palette.prim_disabled
    }
  }

	function updatePrimitive(){
    // only update if there is a primitive
    if(primitive){
      textID.text=primitiveTextIDs[primitive.type]
      color= enabled ? primitiveColors[primitive.type]
                     : Style.palette.prim_disabled
      x= beats[primitive.positionBeat] * appWindow.frameToPixels
      var endBeat = primitive.positionBeat + primitive.lengthBeat
      endBeat = endBeat < beats.length ? endBeat : beats.length - 1
      width= (beats[endBeat]
              - beats[primitive.positionBeat]) * appWindow.frameToPixels
    }
	} // update primitive

	Text
	{
		id: textID
    text: "D"
    color: Style.palette.prim_text
    x: Style.primitives.textPosX * root.height
    y: Style.primitives.textPosY * root.height
    font.pixelSize: Style.primitives.textSize * root.height
    font.bold: Style.primitives.textBold
	} // text

  MouseArea{
    id: dragArea
    anchors.fill: parent
    drag.threshold: 2

    property bool controlPressed: false
    property bool shiftPressed: false
    property bool dragActive: drag.active

    property var resizeMargin: Style.primitives.resizeMarginRight * root.height
    property bool doResize: false
    hoverEnabled: parent.isFromBar

    onWidthChanged: {
      if(resizeMargin > width / 2){
        resizeMargin = width / 2
      }
    }

    onDragActiveChanged: {
      if(dragActive){
        // check if primitive was already selected
        // and clean if neither control or shift were pressed
        if(parent.state !== "onDrag"){
          if(!shiftPressed){
            dragTarget.clean(root)
          }
          parent.state = "onDrag"
        }
        dragTarget.startDrag(controlPressed)
      }else{
        dragTarget.endDrag()
      }
    }

    onPositionChanged:{
      // figure out in what part of the primitive the cursor is
      // and then change the mouse pointer accordingly
      if(!dragActive && isFromBar && mouseX > width - resizeMargin){
        cursorShape = Qt.SizeHorCursor
      }else{
        cursorShape = Qt.ArrowCursor
      }

      if(doResize && pressed){
        // do resize
        var currentFrame = (parent.x + mouseX) / appWindow.frameToPixels;
        if(currentFrame > backend.getAudioLengthInFrames()){
          currentFrame = backend.getAudioLengthInFrames() - 1
        }
        var beatLoc = backend.getBeatAtFrame(currentFrame) + 1
        var newLength = beatLoc - parent.primitive.positionBeat
        if(newLength < 1){
          newLength = 1
        }
        if(newLength < parent.primitive.lengthBeat){
          // decrease size:
          idleParent.parent.freeOccupied(parent.primitive)
          parent.primitive.lengthBeat = newLength
          idleParent.parent.setOccupied(parent.primitive)
          parent.updatePrimitive()
        }else if(newLength > parent.primitive.lengthBeat){
          // check if there is space:
          var start = parent.primitive.positionBeat + parent.primitive.lengthBeat
          var end = parent.primitive.positionBeat + newLength
          if(end > idleParent.parent.occupied.length - 1){
            end = idleParent.parent.occupied.length - 1
          }
          var notFree = false
          for(var i = start; i < end; ++i){
            notFree |= idleParent.parent.occupied[i]
          }

          if(!notFree){
            // space available, resize
            parent.primitive.lengthBeat = end - parent.primitive.positionBeat
            idleParent.parent.setOccupied(parent.primitive)
            parent.updatePrimitive()
          }
        }
      }
    }

    onPressed:{
      appWindow.grabFocus()
      controlPressed = (mouse.modifiers & Qt.ControlModifier)
      shiftPressed = (mouse.modifiers & Qt.ShiftModifier)
      doResize = isFromBar && mouseX > width - resizeMargin
      if(doResize){
        timerBarFlickable.interactive = false
        drag.target= null
      }else{
        drag.target= dragTarget
      }
      mouse.accepted = true
    }

    onReleased: {
      // unless a drag is active, handle de- selection
      if (!drag.active && !doResize) {
        // if shift was pressed, we keep selecting and do not
        // deselect
        if(shiftPressed){
          // select:
          parent.state = "onDrag"
        }else if(controlPressed){
          // with control pressed, we toggle:
          if(parent.state === "onDrag"){
            // deselect
            parent.state = "idle"
          }else{
            parent.state = "onDrag"
          }
        }else{
          // with no modifiers, toggle while deselecting others
          if(parent.state == "onDrag"){
            dragTarget.clean()
          }else{
            parent.state = "onDrag"
            dragTarget.clean(root)
          }
        }
      }
      doResize = false
      timerBarFlickable.interactive = true
    }

    onEntered: showTimer.start()
    onExited: {showTimer.stop(); showData=false}

    Timer {
      id: showTimer
      interval: 250
      onTriggered: showData = true
    }

  } // mouse area


  states: [
    State {
        name: "idle"
        ParentChange { target: root; parent: idleParent }
        PropertyChanges{target: selectionHighlight; visible: false}
    },
    State {
        name: "onDrag"
        ParentChange { target: root; parent: dragTarget }
        PropertyChanges{target: selectionHighlight; visible: true && isFromBar}
    }
  ]

  function deselect(){
    state="idle"
  }

  Rectangle{
    id: selectionHighlight
    visible: false
    color: Style.palette.prim_highlight
    anchors.fill: parent
  }
}
