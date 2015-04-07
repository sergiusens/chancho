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

PageStack {
    id: categoriesPageStack
    Component.onCompleted: push(mainPage)

    EditCategory {
        id: editCategory

        visible: false
    }

    PageWithBottomEdge {
       id: mainPage
       title: i18n.tr("Categories")

       property var incomeModel: Book.categoriesModelForType(Book.INCOME)
       property var expenseModel: Book.categoriesModelForType(Book.EXPENSE)

       ColumnLayout {
           anchors.fill: parent
           anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
           spacing: units.gu(2)

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
                   spacing: units.gu(1)
                   model: mainPage.incomeModel

                   property var numberOfCategories: Book.numberOfCategories(Book.INCOME)

                   delegate: CategoryComponent {
                       name: model.display.name
                       color: model.display.color
                       numberOfCategories: parent.numberOfCategories

                       MouseArea {
                           anchors.fill: parent
                           onClicked: {
                               parent.selected = !parent.selected;
                               categoriesPageStack.push(editCategory, {"category": model.display});
                               parent.selected = !parent.selected;
                           }
                       }
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
                   spacing: units.gu(1)
                   model: mainPage.expenseModel

                   property var numberOfCategories: Book.numberOfCategories(Book.EXPENSE)

                   delegate: CategoryComponent {
                       name: model.display.name
                       color: model.display.color
                       numberOfCategories: parent.numberOfCategories

                       MouseArea {
                           anchors.fill: parent
                           onClicked: {
                               parent.selected = !parent.selected;
                               categoriesPageStack.push(editCategory, {"category": model.display});
                               parent.selected = !parent.selected;
                           }
                       }
                   }
               } // List View
           } // Ubuntu shape list view
       } // ColumnLayout


       bottomEdgePageComponent: NewCategory {}
       bottomEdgeTitle: i18n.tr("Add new category")
    }
} // page stack
