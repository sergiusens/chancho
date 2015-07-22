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

    title: i18n.tr("Add new entry")

    head.actions: [
        Action {
            iconName: "add"
            text: i18n.tr("Add")
            onTriggered: {
                Qt.inputMethod.commit()
                var amount = form.amount;
                if (amount === "") {
                    // we must have the amount
                    var title = i18n.tr("Error: amount is missing");
                    var text = i18n.tr("Transactions must have an amount.");
                    PopupUtils.open(Qt.resolvedUrl("../dialogs/ErrorDialog.qml"), page, {"title": title, "text": text});
                } else {
                    var account = form.accountModel.get(form.accountIndex);
                    amount = amount.replace(",", ".");
                    var category = form.categoryModel.get(form.categoryIndex);
                    var date = form.date;
                    var contents = form.contents;
                    var memo = form.memo;
                    var recurrence = form.recurrence
                    if (recurrence && contents == "") {
                        // we want to make sure that we have a memo so that we can easily identify the reucurrent
                        // transaction
                        var title = i18n.tr("Error: content is missing");
                        var text = i18n.tr("Recurrent transactions must have a contents field so that they can be later identified.");
                        PopupUtils.open(Qt.resolvedUrl("../dialogs/ErrorDialog.qml"), page, {"title": title, "text": text});
                    } else {
                        var success = Book.storeTransaction(account, category, date, parseFloat(amount), contents, memo,
                                                            recurrence);
                        if (success) {
                            mainPageStack.pop();
                        } else {
                            var title = i18n.tr("Internal Error");
                            var text = i18n.tr("The transaction could not be stored.");
                            PopupUtils.open(Qt.resolvedUrl("../dialogs/ErrorDialog.qml"), page, {"title": title, "text": text});
                        }
                    }
                }
            }
        }
    ]

    Form {
        id: form
        showRecurrence: true
        anchors.fill: parent
        anchors.margins: units.gu(1)
    }
}
