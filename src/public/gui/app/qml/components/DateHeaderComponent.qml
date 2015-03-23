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

Row {
    property int day
    property int month
    property int year
    property string dayName

    spacing: 2

    UbuntuShape {
        id: dayNameRectangle

        color: "Orange"
        width: dayLabel.width + units.gu(1)
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

            text: dayName
        }
    }

    Label {
        id: dayLabel

        text: day

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
}

