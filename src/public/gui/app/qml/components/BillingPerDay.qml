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
import com.chancho 1.0

UbuntuShape {
    property alias day: perDayHeader.day
    property alias dayName: perDayHeader.dayName
    property alias month: perDayHeader.month
    property alias year: perDayHeader.year

    BillingPerDayHeader {
        id: perDayHeader
        anchors.top: parent.top

        income: "0"//transactionModel.income
        outcome: "0"//transactionModel.outcome
    }

    Component {
        id: transactionDelegate
        Column {
            width: parent.width
            spacing: units.gu(1)
            TransactionComponent {
                width: parent.width

                anchors.left: parent.left
                anchors.leftMargin: units.gu(1)
                anchors.right: parent.right
                anchors.rightMargin: units.gu(1)

                category: transactionCategory
                contents: transactionContents
                amount: transactionAmount
            }

            Rectangle {
                width: parent.width
                height: units.dp(1)

                anchors.left: parent.left
                anchors.leftMargin: units.gu(1)
                anchors.right: parent.right
                anchors.rightMargin: units.gu(1)

                color: "Light grey"
                visible: (index < transactionsList.model.count - 1) ? true: false
            }
        }
    }

    ListView {
        id: transactionsList
        anchors.top: perDayHeader.bottom
        anchors.topMargin: units.gu(1)
        anchors.bottom: parent.bottom

        spacing: units.gu(1)

        model: Book.dayModel(day, month, year)
        delegate: transactionDelegate

        focus: true
        width: parent.width
    }

    width: parent.width
    height: transactionsList.model.count * 50

}

