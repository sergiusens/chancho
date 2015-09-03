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

import com.chancho 1.0

Item {
    property alias account: accountLabel.text
    property alias amount: amountLabel.text
    property alias memo: memoLabel.text
    property alias contents: contentsLabel.text
    property alias color: background.color
    property bool selected: false
    property var numberOfTransactions

    height: childrenRect.height
    anchors.left: parent.left
    anchors.right: parent.right

    Component.onCompleted: {
        var text = "";
        switch(model.display.recurrence) {
            case Book.DAILY:
                text = i18n.tr("Daily");
                break;
            case Book.WEEKLY:
                text = i18n.tr("Weekly");
                break;
            case Book.MONTHLY:
                text = i18n.tr("Monthly");
                break;
            default:
                text = i18n.tr("Every ") + model.display.numberOfDays  + i18n.tr(" days");
        }
        text += " (" + Qt.formatDate(model.display.startDate, "dd/MM/yy");
        if (model.display.endDate.isValid()) {
            text += "-" + Qt.formatDate(model.display.endDate, "dd/MM/yy");
        }
        text += ")";
        recurrenceLabel.text = text;
    }

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
                    id: categoryInfo
                    anchors.left: parent.left
                    anchors.top: parent.top

                    Label {
                        id: contentsLabel
                        Layout.maximumWidth: units.gu(10)
                        Layout.minimumWidth: units.gu(10)
                        text: model.display.contents
                        elide: Text.ElideRight
                        font.bold: true
                        fontSize: "small"
                        horizontalAlignment: Text.AlignLeft
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
                }  // Column

                Label {
                    id: memoLabel
                    anchors.left: categoryInfo.right
                    anchors.top: parent.top
                    width: parent.width - categoryInfo.width - amountLabel.width

                    text: model.display.memo
                    fontSize: "small"
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                }
                ColumnLayout {
                    anchors.right: parent.right
                    anchors.top: parent.top

                    Label {
                        id: amountLabel
                        Layout.fillWidth: true

                        text: model.display.amount
                        fontSize: "small"
                        color: (model.display.amount > 0)? "green" : "red"
                        horizontalAlignment: Text.AlignRight
                    }
                    Label {
                        id: recurrenceLabel
                        fontSize: "small"
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
            ListItems.ThinDivider {
                visible: (index + 1 != numberOfTransactions)?true:false;
            }
        }
    }
}
