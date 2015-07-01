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
    id: transactionsPageStack

    Component.onCompleted: {
        push(mainPage);
    }

    Page {
       id: mainPage
       title: i18n.tr("Recurrent Transactions")

       ColumnLayout {
           anchors.fill: parent
           anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
           spacing: units.gu(2)
           UbuntuShape {
               id: transactionsShape
               color: "white"
               Layout.fillHeight: true
               anchors.left: parent.left
               anchors.right: parent.right

               UbuntuListView {
                   id: incomeList
                   anchors.fill: parent
                   anchors.margins: units.gu(1)
                   spacing: units.gu(2)
                   clip: true

                   model: Book.recurrentTransactionsModel()
                   delegate: RecurrentTransactionComponent {
                       account: model.display.account
                       amount: model.display.amount
                       category: model.display.category
                       contents: model.display.contents

                       MouseArea {
                           anchors.fill: parent
                           onClicked: {
                               console.log("Recurrent transactions!");
                           }
                       }
                   }
               }
           } // Sahpe
       } // Column
    }
} // page stack
