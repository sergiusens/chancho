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

import com.chancho 1.0

import "components"

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"
    id: mainView

    applicationName: "chancho.mandel"

    Component.onCompleted: {
        var accounts = Book.accounts()
        if (accounts.length == 0) {
            pagestack.push(tabsComponent);
            pagestack.push(Qt.resolvedUrl("components/wizard/WelcomeWizard.qml"));
        } else {
            splashComponent.operationText = i18n.tr("Updating database");
            pagestack.push(tabsComponent);
            pagestack.push(splashComponent.component);
            Book.generateRecurrentTransactions();
        }
    }

    Item {
        Timer {
            id: delay
            interval: 2000
            running: false
            repeat: false
            onTriggered:  {
                pagestack.pop();
            }
        }
    }

    Connections {
        target: Book
        onRecurrentTransactionsGenerated: {
            console.log("Recurrent transactions completed");
            splashComponent.progressUnknown = true;
            splashComponent.progressValue = 0.5;
            delay.start();
        }
    }


    //automaticOrientation: true
    useDeprecatedToolbar: false

    anchorToKeyboard: true

    width: units.gu(100)
    height: units.gu(75)

    PageStack {
        id: pagestack
    }

    Item {
        id: splashComponent
        property string operationText
        property bool progressUnknown: true
        property real progressValue: 0

        property var component: Component {
            SplashScreen {
                id: splashScreen
                operationText: splashComponent.operationText
                progressUnknown: splashComponent.progressUnknown
                progressValue: splashComponent.progressValue
            }
        }
    }

    Component {
        id: tabsComponent
        Tabs {
            id: tabs
            Tab {
                id: mainTab
                title: i18n.tr("Bills")
                page: Loader {
                    parent: mainTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === mainTab) ? Qt.resolvedUrl("components/transactions/Page.qml") : ""
                }
            }
            Tab {
                id: recurrentTab
                title: i18n.tr("Recurrent Transactions")
                page: Loader {
                    parent: recurrentTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === recurrentTab) ? Qt.resolvedUrl("components/RecurrentTransactionsPage.qml") : ""
                }
            }
            Tab {
                id: statsTab
                title: i18n.tr("Stats")
                page: Loader {
                    parent: statsTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === statsTab) ? Qt.resolvedUrl("components/categories/StatsPage.qml") : ""
                }
            }
            Tab {
                id: accountsTab
                title: i18n.tr("Accounts")
                page: Loader {
                    parent: accountsTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === accountsTab) ? Qt.resolvedUrl("components/accounts/Page.qml") : ""
                }
            }

            Tab {
                id: categoriesTab
                title: i18n.tr("Categories")
                page: Loader {
                    parent: categoriesTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === categoriesTab) ? Qt.resolvedUrl("components/categories/Page.qml") : ""
                }
            }
        }// tabs
    }
}

