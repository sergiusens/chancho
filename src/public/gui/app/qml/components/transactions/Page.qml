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
import "../../components"
import "../models"

PageStack {
    id: mainPageStack
    Component.onCompleted: push(mainPage)

    Edit {
        id: editTransaction

        visible: false
    }

    PageWithBottomEdge {
       id: mainPage
       title: i18n.tr("Bills")

       ColumnLayout {
           id: dateTitle
           property date date
           property var monthModel: Book.monthModel(date)

           Component.onCompleted: {
               dateTitle.date = new Date();
           }

           Connections {
                target: dateTitle.monthModel
                onDaysCountChanged: {
                    var count = dateTitle.monthModel.daysCount
                    if (count > 0) {
                        daysList.visible = true;
                        noResultLabel.visible = false;
                    } else {
                        daysList.visible = false;
                        noResultLabel.visible = true;
                    }
                }
           }

           spacing: units.gu(2)

           anchors.fill: parent
           anchors.margins: units.gu(1)

           onDateChanged: {
               monthLabel.text = Qt.formatDateTime(date, "MMMM yyyy");
               monthModel.date = date;
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

               UbuntuListView {
                   id: daysList
                   anchors.fill: parent
                   anchors.margins: units.gu(1)
                   spacing: units.gu(2)
                   clip: true

                   visible: dateTitle.monthModel.daysCount > 0

                   model: dateTitle.monthModel
                   delegate: Loader {
                       anchors {
                           left: parent.left
                           right: parent.right
                       }
                       sourceComponent: BillingPerDay {
                           z: -1
                           dayModel: Book.dayModel(model.display.day, model.display.month, model.display.year)
                       }
                   }
               }

               Label {
                    id: noResultLabel
                    anchors.centerIn: parent
                    anchors.margins: units.gu(1)
                    width: parent.width

                    text: i18n.tr("No entries were found!")
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter

                    fontSize: "x-large"
                    visible: dateTitle.monthModel.daysCount <= 0
               }

           }

       }

       bottomEdgePageComponent: New {}
       bottomEdgeTitle: i18n.tr("Add new entry")
    }
} // page stack
