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
import "js/categories.js" as CategoriesJs

PageStack {
    id: mainPageStack
    Component.onCompleted: {
        var updateGraphOnStoreDelete = function(transactionsDate) {
            // TODO: be smarter
            CategoriesJs.redrawGraph(Book, chart, legendModel, dateTitle.date);
        };

        Book.transactionStored.connect(updateGraphOnStoreDelete);
        Book.transactionRemoved.connect(updateGraphOnStoreDelete);
        Book.transactionUpdated.connect(updateGraphOnStoreDelete);

        push(mainPage);
    }

    EditTransaction {
        id: editTransaction

        visible: false
    }

    Page {
       id: mainPage
       title: i18n.tr("Bills")

       head.actions: [
           Action {
               iconName: "search"
               text: i18n.tr("Search")
           },
           Action {
               iconName: "settings"
               text: i18n.tr("Settings")
           }
       ]

       ColumnLayout {
           id: dateTitle
           property date date: new Date()
           property var monthModel: Book.monthModel(date)

           spacing: units.gu(2)

           anchors.fill: parent
           anchors.margins: units.gu(1)

           onDateChanged: {
               monthLabel.text = Qt.formatDateTime(date, "MMMM yyyy");
               monthModel.date = date;

               CategoriesJs.redrawGraph(Book, chart, legendModel, date);
           }

           Label {
               id: monthLabel

               anchors.left: parent.left
               anchors.right: parent.right

               text: Qt.formatDateTime(date, "MMMM yyyy");
               fontSize: "x-large"
               horizontalAlignment: Text.AlignHCenter

               MouseArea {
                   anchors.fill: parent

                   onClicked: {
                       var popup = PickerPanel.openDatePicker(dateTitle, "date", "Years|Months");
                       popup.picker.minimum = new Date(1900, 1, 1);
                   }
               }
           }

           UbuntuShape {
               id: days
               anchors.left: parent.left
               anchors.right: parent.right
               anchors.margins: units.gu(1)
               color: "white"

               Layout.fillHeight: true

               Chart {
                   id: chart;
                   anchors.fill: parent

                   chartAnimated: true;
                   chartType: Charts.ChartType.PIE;
                   chartAnimationEasing: Easing.Linear;
                   chartAnimationDuration: 1000;
                   chartOptions: {"segmentStrokeColor": "#ECECEC"};

                   Component.onCompleted: {
                        var accounts = Book.accounts();
                        var date = new Date();
                        var percentages = CategoriesJs.calculateGraphData(Book, date);
                        chart.chartData = percentages.data;
                        var categories = percentages.legend

                        // we need to update the legend too
                        legendModel.clear();
                        for(var index=0; index < categories.length; index++) {
                            var category = categories[index];
                            legendModel.append({"name":category.name, "color":category.color});
                        }
                    }
               }
           }

           ListModel {
               id: legendModel
           }

           UbuntuShape {
               id: legend

               anchors.left: parent.left
               anchors.right: parent.right
               anchors.leftMargin: units.gu(1);
               anchors.rightMargin: units.gu(1);
               color: "white"

               Layout.minimumHeight: parent.height/4

               UbuntuListView {
                   id: daysList
                   anchors.fill: parent
                   anchors.topMargin: units.gu(1)
                   anchors.bottomMargin: units.gu(1)
                   spacing: units.gu(1)
                   model: legendModel
                   delegate: CategoryComponent{
                       name: model.name
                       color: model.color
                       numberOfCategories: model.count
                   }
               } // List View

           }

       }
    }
} // page stack
