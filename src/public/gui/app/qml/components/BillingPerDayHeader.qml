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
import Ubuntu.Components 1.1

Item {
    property alias day: dateLabel.day
    property alias dayName: dateLabel.dayName
    property alias month: dateLabel.month
    property alias year: dateLabel.year

    property alias income: incomeLabel.text
    property alias outcome: outcomeValue.text

    Item {
        id: dataWidget

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: units.gu(1)
        anchors.leftMargin: units.gu(1)

        DateHeaderComponent {
            id: dateLabel
            width: units.gu(20)

            anchors.left : parent.left
            anchors.top: parent.top
        }

        Label {
            id: outcomeValue
            height: dateLabel.height - units.dp(2)

            anchors.right: incomeLabel.left
            anchors.top: parent.top

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignBottom

            text: "0.0"
            color: "Red"
        }

        Label {
            id: incomeLabel
            height: dateLabel.height - units.dp(2)
            width: units.gu(20)

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: units.gu(1)

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignBottom

            text: "0.0"
            color: "Green"
        }


        width: parent.width
        height: dateLabel.height
    }

    Rectangle {
        id: separatorRectangle
        height: units.dp(2)

        anchors.top: dataWidget.bottom
        anchors.topMargin: units.dp(2)
        anchors.left: parent.left
        anchors.leftMargin: units.gu(1)
        anchors.right: parent.right
        anchors.rightMargin: units.gu(1)

        color: "Light grey"
    }

    width: parent.width
    height: dataWidget.height + dataWidget.anchors.topMargin + separatorRectangle.height

}
