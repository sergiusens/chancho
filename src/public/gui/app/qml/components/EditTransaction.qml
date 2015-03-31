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
    property var transaction

    onTransactionChanged: {
        if (transaction) {
            // do set the type of entry, this updates the other models accordingly
            if (transaction.type == Book.EXPENSE) {
                form.selectedTypeIndex = 0;
            } else {
                form.selectedTypeIndex = 1;
            }

            form.date = transaction.date
            form.contents = transaction.contents
            form.memo = transaction.memo
            form.amount = transaction.amount

            form.accountIndex = form.accountModel.getIndex(transaction.accountModel)
            form.categoryIndex = form.categoryModel.getIndex(transaction.categoryModel)
        }
    }

    title: "Edit entry"

    head.actions: [
        Action {
            iconName: "edit"
            text: i18n.tr("Edit")
            onTriggered: {
                PopupUtils.open(editDialog, page);
            }
        },
        Action {
            iconName: "delete"
            text: i18n.tr("Delete")
            onTriggered: {
                PopupUtils.open(deleteDialog, page);
            }
        }
    ]

    Component {
         id: deleteDialog

         Dialog {
             id: dialogue
             title: i18n.tr("Delete entry")
             text: i18n.tr("Do you want to remove this entry?")
             Button {
                 text: i18n.tr("ok")
                 color: UbuntuColors.orange
                 onClicked: {
                    Book.removeTransaction(transaction);
                    PopupUtils.close(dialogue);
                    mainPageStack.pop();
                 }
             }
             Button {
                 text: i18n.tr("cancel")
                 onClicked: {
                    PopupUtils.close(dialogue);
                 }
             }
         }
    }

    Component {
         id: editDialog

         Dialog {
             id: dialogue
             title: i18n.tr("Edit entry")
             text: i18n.tr("Do you want to update this entry?")
             Button {
                 text: i18n.tr("ok")
                 color: UbuntuColors.orange
                 onClicked: {
                    var accountModel = form.accountModel.get(form.accountIndex);
                    var categoryModel = form.categoryModel.get(form.categoryIndex);
                    var date = form.date;
                    var contents = form.contents;
                    var memo = form.memo;
                    var amount = form.amount;
                    amount = amount.replace(",", ".");

                    Book.updateTransaction(transaction, accountModel, categoryModel, date, contents, memo, amount);
                    PopupUtils.close(dialogue);
                    mainPageStack.pop();
                 }
             }
             Button {
                 text: i18n.tr("cancel")
                 onClicked: {
                    PopupUtils.close(dialogue);
                 }
             }
         }
    }

    TransactionForm {
        id: form
        anchors.fill: parent
        anchors.margins: units.gu(1)
    }
}
