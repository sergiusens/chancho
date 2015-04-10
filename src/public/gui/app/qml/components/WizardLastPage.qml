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
import "js/last_page_wizard.js" as WizardJs

Component {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
        spacing: units.gu(2)

        Text {
            id: accountsIntroLabel
            Layout.fillWidth: true

            wrapMode: Text.WordWrap
            text: i18n.tr("You are nearly done! Bellow you can find the accounts and categories you will have. Are you happy with them?")
        }

        Label {
            id: accountsLabel
            anchors.left: parent.left
            anchors.right: parent.right
            text: i18n.tr("Accounts:")
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
                clip: true
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
                }
            } // List View
        } // UbuntuShape for list

        Label {
            id: incomeLabel
            anchors.left: parent.left
            anchors.right: parent.right
            text: i18n.tr("Income categories:")
        }

        UbuntuShape {
            id: income
            color: "white"
            Layout.fillHeight: true
            anchors.left: parent.left
            anchors.right: parent.right

            UbuntuListView {
                id: incomeList
                anchors.fill: parent
                anchors.topMargin: units.gu(1)
                anchors.bottomMargin: units.gu(1)
                clip: true
                spacing: units.gu(1)
                model: incomeModel
                delegate: CategoryComponent {
                    name: model.name
                    color: model.color
                    numberOfCategories: incomeModel.count
                }
            } // List View
        } // UbuntuShape for list

        Label {
            id: expenseLabel
            anchors.left: parent.left
            anchors.right: parent.right
            text: i18n.tr("Expense categories:")
        }

        UbuntuShape {
            id: expense
            color: "white"
            Layout.fillHeight: true
            anchors.left: parent.left
            anchors.right: parent.right

            UbuntuListView {
                id: expenseList
                anchors.fill: parent
                anchors.topMargin: units.gu(1)
                anchors.bottomMargin: units.gu(1)
                clip: true
                spacing: units.gu(1)
                model: expenseModel
                delegate: CategoryComponent {
                    name: model.name
                    color: model.color
                    numberOfCategories: expenseModel.count
                }
            } // List View
        } // Ubuntu shape list view

        Button {
            id: readyButton
            Layout.alignment: Qt.AlignHCenter
            color: UbuntuColors.orange
            text: i18n.tr("I'm ready")
            onClicked: {
                console.log("We are done!!!");
                var canBeInit = WizardJs.canBeInit(accountsModel, incomeModel, expenseModel);
                if (canBeInit) {
                    WizardJs.initDatabase(Book, accountsModel, incomeModel, expenseModel);
                    pagestack.pop()
                } else {
                    // show an error!
                    var properties = {
                        "title": i18n.tr("Init Error"),
                        "text": i18n.tr("Make sure that you have added at least one account and a category of each type.")
                    };
                    PopupUtils.open(Qt.resolvedUrl("dialogs/ErrorDialog.qml"), page, properties);
                }
            }
        }

    } // ColumnLayout
}
