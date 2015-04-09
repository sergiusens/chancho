/*
 * Copyright (c) 2015 Manuel de la Peña <mandel@themacaque.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 0.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItems

import "models"

Popover {
    id: popover
    property var shapeWidth
    property var shapeHeight
    property var color

    onColorChanged: {
        colorTextInput.text = color;
    }

    Column {
        id: containerLayout
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: units.gu(1)

        height: childrenRect.height + units.gu(4)

        spacing: units.gu(1)

        Flow {
            id: colorGrid
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: units.gu(1)
            anchors.horizontalCenter: parent.center

            spacing: units.gu(1)

            Repeater {
                id: colorRepeater

                model: DefaultColors {}

                UbuntuShape {
                    color: option
                    width: shapeWidth
                    height: shapeHeight

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            colorTextInput.text = parent.color;
                        }
                    }
                }
            }
        }

        ListItems.ThinDivider {
            width: colorGrid.width
            anchors.leftMargin: units.gu(1)
            anchors.rightMargin: units.gu(1)
        }

        Row {
            id: sampleRow
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: units.gu(1)

            spacing: units.gu(1)

            UbuntuShape {
                id: colorSample
                color: popover.color
                width: shapeWidth
                height: shapeHeight
            }

            TextField {
                id: colorTextInput
                width: colorGrid.childrenRect.width - colorSample.width - units.gu(1)

                text: colorSample.color
                font.bold: true

                onTextChanged: {
                    colorSample.color = text;
                }
            }
        }

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: units.gu(1)
            anchors.rightMargin: units.gu(1)

            Button {
                id: okButton
                Layout.fillWidth: true
                text: i18n.tr("ok")
                color: UbuntuColors.orange
                onClicked: {
                    popover.color = colorSample.color;
                    mainView.anchorToKeyboard = true;
                    PopupUtils.close(popover);
                }
            }

            Button {
                id: cancelButton
                Layout.fillWidth: true
                text: i18n.tr("cancel")
                onClicked: {
                    mainView.anchorToKeyboard = true;
                    PopupUtils.close(popover)
                }
            }
        }

    }
}
