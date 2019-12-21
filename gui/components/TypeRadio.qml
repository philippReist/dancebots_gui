/*
*  Dancebots GUI - Create choreographies for Dancebots
*  https://github.com/philippReist/dancebots_gui
*
*  Copyright 2019 - mint & pepper
*
*  This program is free software : you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*  See the GNU General Public License for more details, available in the
*  LICENSE file included in the repository.
*/

import QtQuick 2.6
import QtQuick.Controls 2.12
import "../GuiStyle"

RadioButton {
  id: root
  leftPadding: 0
  rightPadding: 0
  bottomPadding: 0
  topPadding: 0
  property color mainColor: "green"

  indicator: Rectangle{
    id: indicatorBg
    height: root.height
    width: root.width
    radius: height * Style.primitiveControl.typeRadioRadius
    anchors.verticalCenter: root.verticalCenter
    color: "transparent"
    border.color: root.mainColor
    border.width: height * Style.primitiveControl.typeRadioBorderWidth
    Rectangle{
      anchors.fill: parent
      radius: parent.radius
      color: root.mainColor
      visible: root.checked
    }
  }

  contentItem: Text{
    id: labelText
    text: root.text
    z: 1000
    padding: root.height * Style.primitiveControl.typeRadioTextPadding
    font.pixelSize: root.height - 2.7 * padding
    font.capitalization: Font.AllUppercase
    font.bold: true;
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    color: root.checked ? Style.palette.pc_typeRadioFontActive
                        : mainColor
  }
}
