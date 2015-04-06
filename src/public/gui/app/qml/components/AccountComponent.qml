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
    // get and set the name of the account
    property alias name: nameLabel.text

    // get and set the memo of the account
    property alias memo: memoLabel.text

    // get and set the amount present inthe account
    property alias amount: amountLabel.text

    // color used to identify the account
    property alias color: accountColor.color

    // if the component is selected
    property bool selected: false

    // the total number of accounts, used to decide if the last separator is used
    property var numberOfAccounts

    anchors {
        left: parent.left
        right: parent.right
    }

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

        anchors {
            left: parent.left
            right: parent.right
        }

        height: childrenRect.height

        ColumnLayout {
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }

            spacing: units.gu(1)

            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                height: childrenRect.height

                UbuntuShape {
                    id: accountColor

                    anchors {
                        top: parent.top
                        left: parent.left
                    }

                    height: units.gu(2)
                    width: units.gu(2)

                    color: "orange"
                }

                Label {
                    id: nameLabel

                    anchors{
                        top: parent.top
                        left: accountColor.right
                        leftMargin: units.gu(1)
                    }

                    width: units.gu(20)

                    elide: Text.ElideRight
                    font.bold: true
                    horizontalAlignment: Text.AlignLeft
                }

                Label {
                    id: memoLabel

                    anchors {
                        top: parent.top
                        left: nameLabel.right
                    }

                    width: parent.width - nameLabel.width - amountLabel.width - accountColor.width

                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                }

                Label {
                    id: amountLabel

                    anchors {
                        top: parent.top
                        right: parent.right
                    }

                    width: units.gu(10)

                    color: (parseFloat(amount) > 0)? "green" : "red"
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
