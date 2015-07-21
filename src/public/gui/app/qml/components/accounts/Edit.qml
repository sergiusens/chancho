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
import "js/accounts.js" as AccountsJs

Page {
    id: page
    property var account

    onAccountChanged: {
        var date = new Date();
        var graphData = AccountsJs.calculateGraphData(Book, [account], date);
        form.graphData = graphData;
        form.name = account.name;
        form.memo = account.memo;
        form.color = account.color;
    }

    title: "Edit account"

    head.actions: [
        Action {
            iconName: "edit"
            text: i18n.tr("Edit")
            onTriggered: {
                Qt.inputMethod.commit()
                var updateEntryCb = function() {
                    Book.updateAccount(account, form.name, form.memo, form.color);
                    accountsPageStack.pop();
                };
                var properties = {
                    "title": i18n.tr("Edit entry"),
                    "text": i18n.tr("Do you want to update this entry?"),
                    "okCallback": updateEntryCb
                };

                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmationDialog.qml"), page, properties);
            }
        },
        Action {
            iconName: "delete"
            text: i18n.tr("Delete")
            onTriggered: {
                var allAccounts = Book.accounts()
                if (allAccounts.length > 1) {
                    var deleteEntryCb = function() {
                        Book.removeAccount(account);
                        accountsPageStack.pop();
                    };
                    var properties = {
                        "title": i18n.tr("Delete entry"),
                        "text": i18n.tr("Do you want to remove this account?"),
                        "okCallback": deleteEntryCb
                    };

                    PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmationDialog.qml"), page, properties);
                } else {
                    var title = i18n.tr("Error");
                    var text = i18n.tr("You must have at least one account in the system.");
                    PopupUtils.open(Qt.resolvedUrl("../dialogs/ErrorDialog.qml"), page, {"title": title, "text": text});
                }
            }
        }
    ]

    Form {
        id: form
        anchors.fill: parent
        anchors.margins: units.gu(1)

        showInitialAmount: false
    }
}
