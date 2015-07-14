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
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.Pickers 0.1

import com.chancho 1.0

Dialog {
     id: dialogue
     property var okCallback

     title: i18n.tr("Recurrency")
     text: i18n.tr("Choose the recurrency of the transaction")

     Component {
         id: repeatDelegate
         OptionSelectorDelegate {
             text: name;
         }
     }

     Item{
         ListModel {
             id: repeatModel
             dynamicRoles: true
         }
         Component.onCompleted: {
             repeatModel.append({
                 "name": i18n.tr("Daily"),
                 "enumType": Book.DAILY
             });
             repeatModel.append({
                 "name": i18n.tr("Weekly"),
                 "enumType": Book.WEEKLY
             });
             repeatModel.append({
                 "name": i18n.tr("Monthly"),
                 "enumType": Book.MONTHLY
             });
             repeatModel.append({
                 "name": i18n.tr("Other"),
                 "enumType": Book.OTHER
             });
         }
     }

     Label {
         Layout.fillWidth : true
         text: i18n.tr("Repeat mode:")
     }

     OptionSelector {
         id: repeatSelector
         Layout.fillWidth : true

         containerHeight: itemHeight * 2

         model: repeatModel
         delegate: repeatDelegate

         onSelectedIndexChanged : {
             if (selectedIndex == 3) {
                 console.log("User wants a special repeat");
                 daysField.enabled= true;
             } else {
                 daysField.enabled = false;
             }
         }
     }

     TextField {
         id: daysField
         enabled: false

         anchors.left: parent.left
         anchors.right: parent.right
         inputMethodHints: Qt.ImhDigitsOnly

         placeholderText: i18n.tr("Number of days")
     }

     Component {
         id: endDateDelegate
         OptionSelectorDelegate {
             text: name;
         }
     }

     Item{
         ListModel {
             id: endDateModel
             dynamicRoles: true
         }
         Component.onCompleted: {
             endDateModel.append({
                 "name": i18n.tr("Never"),
             });
             endDateModel.append({
                 "name": i18n.tr("Date"),
             });
             endDateModel.append({
                 "name": i18n.tr("Occurrences"),
             });
         }
     }

     Label {
         Layout.fillWidth : true
         text: i18n.tr("Termination:")
     }

     OptionSelector {
         id: endDateSelector
         Layout.fillWidth : true

         containerHeight: itemHeight * 2

         model: endDateModel
         delegate: endDateDelegate

         onSelectedIndexChanged : {
             if (selectedIndex == 1) {
                 endField.text = "";
                 endField.placeholderText = i18n.tr("Date");
                 endField.inputMethodHints = Qt.ImhDate;
                 endFieldMouseArea.enabled = true;
                 endField.enabled = true;
             } else if (selectedIndex == 2) {
                 endField.text = "";
                 endField.placeholderText = i18n.tr("Occurrences");
                 endField.inputMethodHints = Qt.ImhDigitsOnly;
                 endFieldMouseArea.enabled = false;
                 endField.enabled = true;
             } else {
                 endField.text = "";
                 endField.enabled = false;
             }
         }
     }

     TextField {
         id: endField
         enabled: false

         property date date: new Date()

         anchors.left: parent.left
         anchors.right: parent.right

         onDateChanged: {
             endField.text = Qt.formatDateTime(date, "dd/MM/yyyy");
         }

         MouseArea {
             id: endFieldMouseArea
             anchors.fill: parent
             enabled: false

             onClicked: {
                 if (endDateSelector.selectedIndex == 1) {
                     var popup = PickerPanel.openDatePicker(endField, "date", "Days|Years|Months");
                     popup.picker.minimum = new Date(1900, 1, 1);
                 }
             }
         }
     }

     RowLayout {
         Layout.fillWidth: true
         spacing: units.gu(1)

         Button {
             text: i18n.tr("Cancel")
             onClicked: PopupUtils.close(dialogue)
             Layout.fillWidth: true
         }

         Button {
             text: i18n.tr("Ok")
             color: UbuntuColors.orange
             onClicked: {
                if (okCallback) {
                    // create the object to be passed to the cb
                    var recurrence = {
                        "type": repeatModel.get(repeatSelector.selectedIndex).enumType
                    };
                    if (repeatSelector.selectedIndex == 3) {
                        recurrence["days"] = parseInt(daysField.text);
                    }

                    if (endDateSelector.selectedIndex == 1) {
                        recurrence["end"] = {
                            "date" : endField.date
                        };
                    }

                    if (endDateSelector.selectedIndex == 2) {
                        recurrence["end"] = {
                            "occurrences" : parseInt(endField.text)
                        };
                    }

                    okCallback(recurrence);
                }
                PopupUtils.close(dialogue)
             }
             Layout.fillWidth: true
         }
     }
}

