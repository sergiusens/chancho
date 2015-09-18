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
import Ubuntu.Components.ListItems 1.0 as ListItems

import jbQuick.Charts 1.0

import com.chancho 1.0

Page {
    id: page
    property var recurrentTransaction
    property int minSize

    title: i18n.tr("Edit entry")

    Component.onCompleted: {
        minSize = childrenRect.height;
        flickable.contentHeight = childrenRect.height;
    }

    onHeightChanged : {
        if (height > minSize)
            flickable.contentHeight = childrenRect.height;
    }

    head.actions: [
        Action {
            iconName: "edit"
            text: i18n.tr("Edit")
            onTriggered: {
                Qt.inputMethod.commit()
                var editTransactionsCb = function(updateAll) {
                    var accountModel = accountSelector.model.get(accountSelector.selectedIndex);
                    var categoryModel = categorySelector.model.get(categorySelector.selectedIndex);
                    var date = datePicker.date;
                    var contents = contentsField.text;
                    var memo = memoTextArea.text;
                    var amount = amountField.text
                    amount = amount.replace(",", ".");
                    console.log("New amount is " + amount);

                    Book.updateRecurrentTransaction(recurrentTransaction, accountModel, categoryModel, date, contents,
                        memo, amount, updateAll);
                    transactionsPageStack.pop();
                };
                var properties = {
                    "title": i18n.tr("Edit entry"),
                    "text": i18n.tr("Do you want to update this entry?"),
                    "okCallback": editTransactionsCb
                };
                PopupUtils.open(Qt.resolvedUrl("UpdateConfirmationDialog.qml"), page, properties);
            }
        },
        Action {
            iconName: "delete"
            text: i18n.tr("Delete")
            onTriggered: {
                var deleteTransactionsCb = function(removePast) {
                    Book.removeRecurrentTransaction(recurrentTransaction, removePast);
                    transactionsPageStack.pop();
                };
                var properties = {
                    "title": i18n.tr("Delete transaction"),
                    "text": i18n.tr("Do you want to remove this transaction?"),
                    "okCallback": deleteTransactionsCb
                };
                PopupUtils.open(Qt.resolvedUrl("RemoveConfirmationDialog.qml"), page, properties);
            }
       }
    ]

   onRecurrentTransactionChanged: {
       amountField.text = recurrentTransaction.amount;
       accountSelector.selectedIndex = accountSelector.model.getIndex(recurrentTransaction.accountModel)
       categorySelector.selectedIndex = categorySelector.model.getIndex(recurrentTransaction.categoryModel)

       if (recurrentTransaction.type == Book.INCOME) {
           typeSelector.selectedIndex = 1;
       } else {
           typeSelector.selectedIndex = 0;
       }

       if (recurrentTransaction.contents !== "")
           contentsField.text = recurrentTransaction.contents
       if (recurrentTransaction.memo !== "")
           memoTextArea.text = recurrentTransaction.memo;
       generatedList.model.recurrentTransaction = recurrentTransaction;
       datePicker.date = recurrentTransaction.startDate
   }

   ColumnLayout {
       anchors.fill: parent
       anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
       spacing: units.gu(2)
       UbuntuShape {
           id: formShape
           Layout.fillHeight: true
           anchors.left: parent.left
           anchors.right: parent.right

           Flickable {
               id: flickable
               anchors {
                   left: parent.left
                   right: parent.right
                   top: parent.top
                   bottom: parent.bottom
               }

               contentHeight: parent.height;
               clip: true


               ColumnLayout {
                   id: mainColumn
                   spacing: units.gu(1)
                   anchors {
                       left: parent.left
                       right: parent.right
                       top: parent.top
                       bottom: parent.bottom
                       margins: units.gu(1)
                   }

                   Component {
                       id: typeDelegate
                       OptionSelectorDelegate {
                           text: name;
                       }
                   }

                   Item{
                       ListModel {
                           id: typeModel
                           dynamicRoles: true
                       }
                       Component.onCompleted: {
                           typeModel.append({
                               "name": i18n.tr("Expense"),
                               "enumType": Book.EXPENSE
                           });
                           typeModel.append({
                               "name": i18n.tr("Income"),
                               "enumType": Book.INCOME
                           });
                       }
                   }

                   OptionSelector {
                       id: typeSelector
                       Layout.fillWidth : true

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

                       containerHeight: itemHeight * 4
                       model: Book.categoriesModelForType(Book.EXPENSE)
                       delegate: categoriesDelegate
                   }

                   TextField {
                       id: datePicker
                       Layout.fillWidth: true

                       property date date: new Date()

                       placeholderText: i18n.tr("Date")
                       onDateChanged: {
                           datePicker.text = Qt.formatDateTime(date, "dd/MM/yyyy");
                       }

                       MouseArea {
                           anchors.fill: parent

                           onClicked: {
                               var popup = PickerPanel.openDatePicker(datePicker, "date", "Days|Years|Months");
                               popup.picker.minimum = new Date(1900, 1, 1);
                           }
                       }
                   }

                   TextField {
                        id: amountField

                        anchors.left: parent.left
                        anchors.right: parent.right
                        inputMethodHints: Qt.ImhFormattedNumbersOnly

                        validator: DoubleValidator {
                            id: doubleValidator
                            locale: "en"
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
                       Layout.fillHeight: true
                       anchors.left: parent.left
                       anchors.right: parent.right

                       width: parent.width
                       autoSize: true

                       placeholderText: i18n.tr("Memo")
                   }

                   UbuntuShape {
                       id: transactionsShape
                       color: "white"
                       Layout.fillHeight: true
                       Layout.minimumHeight: page.height / 4
                       Layout.maximumHeight: parent.height/4
                       anchors.left: parent.left
                       anchors.right: parent.right

                       UbuntuListView {
                           id: generatedList
                           anchors.fill: parent
                           anchors.margins: units.gu(1)
                           spacing: units.gu(2)
                           clip: true

                           model: Book.generatedTransactions(recurrentTransaction)

                           delegate: GeneratedTransaction {
                               property var transaction: model.display
                               property var repeaterIndex: index

                           }
                       }
                   } // Sahpe
               }

           } // flickable
       }
   } // Column
}
