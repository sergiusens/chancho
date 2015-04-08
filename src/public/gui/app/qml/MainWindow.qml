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

import Ubuntu.PerformanceMetrics 0.1
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

PerformanceOverlay {
    active: true
    z: 300
}
    applicationName: "chancho.mandel"

    //automaticOrientation: true
    useDeprecatedToolbar: false

    anchorToKeyboard: true

    width: units.gu(100)
    height: units.gu(75)

    PageStack {
        id: pagestack
    }

    Component {
        id: tabsComponent
        Tabs {
            id: tabs
            Tab {
                id: mainTab
                title: "Bills"
                page: Loader {
                    parent: mainTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === mainTab) ? Qt.resolvedUrl("components/MainPage.qml") : ""
                }
            } // tab

            Tab {
                id: statsTab
                title: "Stats"
                page: Loader {
                    parent: statsTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === statsTab) ? Qt.resolvedUrl("components/CategoryStatsPage.qml") : ""
                }
            }
            Tab {
                id: accountsTab
                title: "Accounts"
                page: Loader {
                    parent: accountsTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === accountsTab) ? Qt.resolvedUrl("components/AccountsPage.qml") : ""
                }
            }

            Tab {
                id: categoriesTab
                title: "Categories"
                page: Loader {
                    parent: categoriesTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === categoriesTab) ? Qt.resolvedUrl("components/CategoriesPage.qml") : ""
                }
            }
        }// tabs
    }

    Component.onCompleted: {
        var accounts = Book.accounts()
        if (accounts.length == 0) {
            pagestack.push(tabsComponent);
            pagestack.push(Qt.resolvedUrl("components/WelcomeWizard.qml"));
        } else {
            pagestack.push(tabsComponent)
        }
    }

}

