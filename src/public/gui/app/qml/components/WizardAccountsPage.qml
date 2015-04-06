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

import "js/accounts_wizard.js" as AccountsWizardJs

Component {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
        spacing: units.gu(2)

        Label {
            id: titleLabel
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.WordWrap

            text: i18n.tr("Accounts");
            fontSize: "x-large"
        }

        Text {
            id: accountsIntroLabel
            Layout.fillWidth: true

            wrapMode: Text.WordWrap
            text: i18n.tr("Chancho allows you to track the expenses and incomes in several accounts. Here you "
                + "can add the accounts you want to work with. Don't worry you can add more accounts later.")
        }

        UbuntuShape {
            id: accounts
            color: "white"
            Layout.fillHeight: true
            anchors.left: parent.left
            anchors.right: parent.right
            UbuntuListView {
                id: daysList
                anchors.fill: parent
                anchors.topMargin: units.gu(1)
                anchors.bottomMargin: units.gu(1)
                model: accountsModel
                spacing: units.gu(1)
                delegate: AccountComponent {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: units.gu(1)
                    color: model.color
                    name: model.name
                    memo: model.memo
                    amount: model.amount
                    numberOfAccounts: model.count

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            parent.selected = !parent.selected;
                            accountsModel.setProperty(index, "selected", parent.selected);
                        }
                    }
                }
            } // List View
        } // UbuntuShape for list

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: i18n.tr("Add")
                color: UbuntuColors.orange
                onClicked: {
                    var title = i18n.tr("New account");
                    var addAccountText = i18n.tr("Please add the details of the new account.");
                    var properties = {
                        "title": title,
                        "text": addAccountText,
                        "accountsModel": accountsModel
                    };
                    PopupUtils.open(Qt.resolvedUrl("dialogs/WizardNewAccountDialog.qml"), page, properties);
                }
            }
            Button {
                text: i18n.tr("Remove")
                onClicked: {
                    var showConfirmationDialog = AccountsWizardJs.areAccountsSelected(accountsModel);
                    if (showConfirmationDialog) {
                        var deleteAccountsCb = function() {
                            AccountsWizardJs.deleteAccountsCallback(accountsModel);
                        };
                        var properties = {
                            "title": i18n.tr("Delete accounts"),
                            "text": i18n.tr("Do you want to delete the selected accounts?"),
                            "okCallback": deleteAccountsCb
                        };
                        PopupUtils.open(Qt.resolvedUrl("dialogs/ConfirmationDialog.qml"), page, properties);
                    }
                }
            }
        }
    } // ColumnLayout
}
