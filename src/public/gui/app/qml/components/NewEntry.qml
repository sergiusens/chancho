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

UbuntuShape {

    Grid {
        id: formGrid

        columns: 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: units.gu(1)

        rowSpacing: units.gu(1)
        columnSpacing: units.gu(1)

        Label {
            text: "Date:"
            height: dateTextField.height
            width: units.gu(15)

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }

        TextField {
            id: dateTextField
            placeholderText: "Date:"

            width: parent.width - units.gu(16)
        }

        Label {
            text: "Account:"
            height: accountSelector.height
            width: units.gu(15)

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }

        OptionSelector {
            id: accountSelector
            width: parent.width - units.gu(16)

            model: ["Value 1", "Value 2", "Value 3", "Value 4"]
        }

        Label {
            text: "Category:"
            height: categorySelector.height
            width: units.gu(15)

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }

        OptionSelector {
            id: categorySelector
            width: parent.width - units.gu(16)

            model: ["Value 1", "Value 2", "Value 3", "Value 4"]
        }

        Label {
            text: "Amount:"
            height: amountTextField.height
            width: units.gu(15)

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }

        TextField {
            id: amountTextField
            width: parent.width - units.gu(16)

            placeholderText: "Amount"
        }

        Label {
            text: "Contents:"
            height: contentsTextField.height
            width: units.gu(15)

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }

        TextField {
            id: contentsTextField
            width: parent.width - units.gu(16)

            placeholderText: "Contents"
        }

        width: parent.width
        height: dateTextField.height + accountSelector.height +  categorySelector.height + amountTextField.height + contentsTextField.height + units.gu(5)
    }

    TextArea {
        anchors.top: formGrid.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: units.gu(1)
        anchors.right: parent.right
        anchors.rightMargin: units.gu(1)

        width: parent.width
        autoSize: true

        placeholderText: "Memo"
    }
}
