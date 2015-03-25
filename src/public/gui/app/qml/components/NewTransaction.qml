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
import Ubuntu.Components.Pickers 0.1
import Ubuntu.Components.Popups 1.0

import com.chancho 1.0

Page {
    id: page

    title: "Add new entry"

    head.actions: [
        Action {
            iconName: "add"
            text: i18n.tr("Add")
            onTriggered: {
                var amount = amountField.text;
                if (amount === "") {
                    // we must have the amount
                    var title = i18n.tr("Error: amount is missing");
                    var text = i18n.tr("Transactions must have an amount.");
                    PopupUtils.open(dialog, page, {"title": title, "text": text});
                } else {
                    var account = accountSelector.model.get(accountSelector.selectedIndex);
                    var category = categorySelector.model.get(categorySelector.selectedIndex);
                    var date = datePicker.date;
                    var contents = contentsField.text;
                    var memo = memoTextArea.text;
                    var success = Book.storeTransaction(account, category, date, amount, contents, memo);
                    if (success) {
                        stack.pop();
                    } else {
                        var title = i18n.tr("Internal Error");
                        var text = i18n.tr("The transaction could not be stored.");
                        PopupUtils.open(dialog, page, {"title": title, "text": text});
                    }
                }
            }
        }
    ]

    Component {
         id: dialog

         Dialog {
             id: dialogue
             Button {
                 text: "ok"
                 color: UbuntuColors.orange
                 onClicked: PopupUtils.close(dialogue)
             }
         }
    }

    UbuntuShape {

        anchors.fill: parent
        anchors.margins: units.gu(1)

        ColumnLayout {
            spacing: units.gu(1)
            anchors.fill: parent
            anchors.margins: units.gu(1)

            Component {
                id: typeDelegate
                OptionSelectorDelegate {
                    text: name;
                }
            }

            ListModel {
                id: typeModel
                ListElement {
                    name: "Expense";
                    enumType: Book.EXPENSE;
                }
                ListElement {
                    name: "Income";
                    enumType: Book.INCOME;
                }
            }

            OptionSelector {
                id: typeSelector

                anchors.left: parent.left
                anchors.right: parent.right
                width: parent.width

                model: typeModel
                delegate: typeDelegate

                onSelectedIndexChanged : {
                    categorySelector.model.categoryType = model.get(selectedIndex).enumType;
                }
            }

            Component {
                id: accountsDelegate
                OptionSelectorDelegate {
                    text: model.display.name;
                }
            }

            OptionSelector {
                id: accountSelector
                anchors.left: parent.left
                anchors.right: parent.right

                model: Book.accountsModel()
                delegate: accountsDelegate
            }

            Component {
                id: categoriesDelegate
                OptionSelectorDelegate {
                    text: model.display.name;
                }
            }

            OptionSelector {
                id: categorySelector
                anchors.left: parent.left
                anchors.right: parent.right

                model: Book.categoriesModeForType(Book.EXPENSE)
                delegate: categoriesDelegate
            }

            TextField {
                id: datePicker
                property date date: new Date()

                anchors.left: parent.left
                anchors.right: parent.right

                placeholderText: i18n.tr("Date")
                onDateChanged: {
                    console.log("Date changed.");
                    datePicker.text = Qt.formatDateTime(date, "dd/MM/yyyy");
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        PickerPanel.openDatePicker(datePicker, "date");
                    }
                }
            }

            TextField {
                id: amountField

                anchors.left: parent.left
                anchors.right: parent.right

                validator: DoubleValidator {
                    notation: DoubleValidator.StandardNotation;
                }

                placeholderText: i18n.tr("Amount")
            }

            TextField {
                id: contentsField

                anchors.left: parent.left
                anchors.right: parent.right

                placeholderText: i18n.tr("Contents")
            }

            TextArea {
                id: memoTextArea
                anchors.left: parent.left
                anchors.right: parent.right

                Layout.fillHeight: true

                width: parent.width
                autoSize: true

                placeholderText: i18n.tr("Memo")
            }
        }
    }
}
