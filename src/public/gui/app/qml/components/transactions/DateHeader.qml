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

Item {
    property alias dayName: dayNameLabel.text
    property alias day: dayLabel.text
    property alias expenses: expenses.text
    property alias income: income.text
    property var month
    property var year

    anchors.left: parent.left
    anchors.right: parent.right

    height: totalLayout.height + units.gu(1)

    Item {
        anchors.left: parent.left
        anchors.right: parent.right

        height: totalLayout.height

        Row {
            id: dateRow
            spacing: units.gu(1)
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.rightMargin: units.gu(1)
            width: parent.width - units.gu(20)
            height: totalLayout.height

            UbuntuShape {
                id: dayNameRectangle

                color: "Orange"
                width: dayNameLabel.width
                height: dayLabel.height - units.dp(2)

                Label {
                    id: dayNameLabel

                    anchors.top: parent.top;
                    anchors.bottom: parent.bottom;
                    rotation: -90

                    fontSize: "x-small"
                    font.bold: true
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Label {
                id: dayLabel

                fontSize: "x-large"
                font.bold: true
                horizontalAlignment: Text.AlignCenter
                verticalAlignment: Text.AlignBottom
            }

            Label {
                id: monthYear
                height: dayLabel.height - units.dp(2)

                text: month + "." + year

                fontSize: "small"
                horizontalAlignment: Text.AlignCenter
                verticalAlignment: Text.AlignBottom
            }
        } // Row

        ColumnLayout {
            id: totalLayout
            anchors.top: parent.top
            anchors.left: dateRow.left
            anchors.right: parent.right
            anchors.topMargin: units.gu(1)

        Label {
            id: expenses
            Layout.fillWidth: true

            horizontalAlignment: Text.AlignRight
            fontSize: "small"

            text: "0.0"
            color: "Red"
        }

        Label {
            id: income
            Layout.fillWidth: true

            horizontalAlignment: Text.AlignRight
            fontSize: "small"

            text: "0.0"
            color: "Green"
        }
        }
    }// Item
}
