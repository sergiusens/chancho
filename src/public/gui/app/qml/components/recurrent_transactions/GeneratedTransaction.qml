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
    property alias category: dateLabel.text
    property alias contents: contentsLabel.text
    property alias amount: amountLabel.text
    property alias color: background.color
    property bool selected: false
    property var numberOfTransactions

    height: childrenRect.height
    anchors.left: parent.left
    anchors.right: parent.right

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

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height

                ColumnLayout {
                    id: dateInfo
                    anchors.left: parent.left
                    anchors.top: parent.top

                    Row {
                        spacing: units.gu(1)

                        Label {
                            id: dateLabel
                            Layout.maximumWidth: units.gu(10)
                            Layout.minimumWidth: units.gu(10)
                            text: Qt.formatDate(model.display.date, "d MMMM yyyy")
                            elide: Text.ElideRight
                            font.bold: true
                            fontSize: "small"
                            horizontalAlignment: Text.AlignLeft
                        }
                    }

                    Label {
                        id: accountLabel
                        Layout.maximumWidth: units.gu(10)
                        Layout.minimumWidth: units.gu(10)
                        text: model.display.account
                        fontSize: "x-small"
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignLeft
                    }
                }

                Label {
                    id: contentsLabel
                    anchors.left: dateInfo.right
                    anchors.leftMargin: units.gu(1)
                    anchors.top: parent.top
                    width: parent.width - dateInfo.width - amountLabel.width

                    text: model.display.contents
                    fontSize: "small"
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                }

                Label {
                    id: amountLabel
                    width: units.gu(10)

                    anchors.right: parent.right
                    anchors.top: parent.top

                    text: model.display.amount
                    fontSize: "small"
                    color: (model.display.amount > 0)? "green" : "red"
                    horizontalAlignment: Text.AlignRight
                }
            }
            ListItems.ThinDivider {
                visible: (index + 1 != numberOfTransactions)?true:false;
            }
        }
    }
}
