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

Page {
    id: page

    property alias operationText: progressText.text
    property alias progressUnknown: startUpProgress.indeterminate
    property alias progressValue: startUpProgress.value

    UbuntuShape {
        id: mainShape
        color: "white"

        anchors {
            fill: parent
            margins: units.gu(2)
        }

        ColumnLayout {
            id: mainColumn

            anchors {
                centerIn: parent
                margins: units.gu(1)
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: units.gu(20)
                Layout.preferredHeight: units.gu(10)

                cache: false
                fillMode: Image.PreserveAspectFit
                source: Qt.resolvedUrl("images/chancho_logo.png");
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: units.gu(40)
                Layout.preferredHeight: units.gu(10)

                cache: false
                fillMode: Image.PreserveAspectFit
                source: Qt.resolvedUrl("images/chancho_wordmark.png");
            }


            ProgressBar {
                id: startUpProgress
                Layout.alignment: Qt.AlignHCenter
                anchors.leftMargin: units.gu(2)
                anchors.rightMargin: units.gu(2)

                indeterminate: true

                maximumValue: 1
                minimumValue: 0
                value: 0
            }

            Label {
                id: progressText
                Layout.alignment: Qt.AlignHCenter
            }
        }

    }
}
