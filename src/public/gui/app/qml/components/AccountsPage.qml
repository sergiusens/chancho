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

PageStack {
    id: accountsPageStack
    Component.onCompleted: push(mainPage)

    EditAccount {
        id: editAccount

        visible: false
    }

    PageWithBottomEdge {
       id: mainPage
       title: i18n.tr("Accounts")

       property var accountsModel: Book.accountsModel()

       ColumnLayout {
           anchors.fill: parent
           anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
           spacing: units.gu(2)
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
                   spacing: units.gu(1)
                   model: mainPage.accountsModel
                   property var numberOfAccounts: mainPage.accountsModel.numberOfAccounts()
                   delegate: AccountComponent {
                       anchors.left: parent.left
                       anchors.right: parent.right
                       anchors.margins: units.gu(1)
                       color: model.display.color
                       name: model.display.name
                       memo: model.display.memo
                       amount: model.display.amount
                       numberOfAccounts: parent.numberOfAccounts
                       MouseArea {
                           anchors.fill: parent
                           onClicked: {
                               parent.selected = !parent.selected;
                               accountsPageStack.push(editAccount, {"account": model.display});
                               parent.selected = !parent.selected;
                           }
                       }
                   }
               } // List View
           } // UbuntuShape for list

           UbuntuShape {
               id: chartShape
               Layout.fillHeight: true
               Layout.fillWidth: true

               anchors.left: parent.left
               anchors.right: parent.right
               height: childrenRect.height
               color: "white"

               Chart {
                   id: chart
                   anchors.fill: parent

                   chartAnimated: true;
                   chartAnimationEasing: Easing.InOutElastic;
                   chartAnimationDuration: 2000;
                   chartType: Charts.ChartType.LINE;

                   Component.onCompleted: {
                       chartData = {
                           labels: [
                               i18n.tr("January"),
                               i18n.tr("February"),
                               i18n.tr("March"),
                               i18n.tr("April"),
                               i18n.tr("May"),
                               i18n.tr("June"),
                               i18n.tr("July"),
                               i18n.tr("August"),
                               i18n.tr("September"),
                               i18n.tr("October"),
                               i18n.tr("November"),
                               i18n.tr("December")],
                           datasets: [{
                               label: "My First dataset",
                               fillColor: "orange",
                               strokeColor: "orange",
                               pointColor: "orange",
                               pointStrokeColor: "#fff",
                               pointHighlightFill: "#fff",
                               pointHighlightStroke: "rgba(220,220,220,1)",
                               data: [65, -59, 80.5, 81, -156, 55, 140, 90, 45, 12, 98, 25]
                           }, {

                               label: "My First dataset",
                               fillColor: "blue",
                               strokeColor: "blue",
                               pointColor: "blue",
                               pointStrokeColor: "#fff",
                               pointHighlightFill: "#fff",
                               pointHighlightStroke: "rgba(220,220,220,1)",
                               data: [5, -19, 9.5, 81, -15, 85, 121, 220, 48, 14, 102, 35]
                           }]
                       }
                   }
               }
           }  // chart u shape
       } // ColumnLayout


       bottomEdgePageComponent: NewAccount {}
       bottomEdgeTitle: i18n.tr("Add new account")
    }
} // page stack

