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
import Ubuntu.Components.ListItems 1.0 as ListItems

Item {
    property alias name: nameLabel.text
    property alias memo: memoLabel.text
    property alias amount: amountLabel.text
    property alias color: accountColor.color
    property bool selected: false
    property var numberOfAccounts

    anchors.left: parent.left
    anchors.right: parent.right
    height: childrenRect.height

    onSelectedChanged: {
        if (selected) {
            background.color = "Light grey";
        } else {
            background.color = "white";
        }
    }

    Rectangle {
        id: background

        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        ColumnLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: units.gu(1)
            spacing: units.gu(1)

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height

                UbuntuShape {
                    id: accountColor
                    anchors.top: parent.top
                    anchors.left: parent.left

                    color: "orange"
                    height: units.gu(2)
                    width: units.gu(2)
                }

                Label {
                    id: nameLabel
                    anchors.top: parent.top
                    anchors.left: accountColor.right
                    anchors.leftMargin: units.gu(1)
                    width: units.gu(20)

                    elide: Text.ElideRight
                    font.bold: true
                    horizontalAlignment: Text.AlignLeft
                }

                Label {
                    id: memoLabel
                    anchors.top: parent.top
                    anchors.left: nameLabel.right
                    width: parent.width - nameLabel.width - amountLabel.width - accountColor.width

                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                }

                Label {
                    id: amountLabel
                    anchors.top: parent.top
                    anchors.right: parent.right
                    width: units.gu(10)

                    color: (model.display.amount > 0)? "green" : "red"
                    horizontalAlignment: Text.AlignRight
                }
            }
            ListItems.ThinDivider {
                width: parent.width
                visible: (index + 1 != numberOfAccounts)?true:false;
            }
        }
    }
}
