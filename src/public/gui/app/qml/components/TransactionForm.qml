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
import "models"

UbuntuShape {

    property alias selectedTypeIndex: typeSelector.selectedIndex
    property alias accountModel: accountSelector.model
    property alias accountIndex: accountSelector.selectedIndex
    property alias categoryModel: categorySelector.model
    property alias categoryIndex: categorySelector.selectedIndex
    property alias date:  datePicker.date;
    property alias contents: contentsField.text;
    property alias memo: memoTextArea.text;
    property alias amount: amountField.text;

    onHeightChanged : {
        if (height > flickable.contentHeight) {
            flickable.contentHeight = height;
        }
    }

    Flickable {
        id: flickable
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        clip: true


        ColumnLayout {
            id: mainColumn
            spacing: units.gu(1)
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: units.gu(1)
            }

            Component {
                id: typeDelegate
                OptionSelectorDelegate {
                    text: name;
                }
            }

            Item{
                ListModel {
                    id: typeModel
                    dynamicRoles: true
                }
                Component.onCompleted: {
                    typeModel.append({
                        "name": i18n.tr("Expense"),
                        "enumType": Book.EXPENSE
                    });
                    typeModel.append({
                        "name": i18n.tr("Income"),
                        "enumType": Book.INCOME
                    });
                }
            }

            OptionSelector {
                id: typeSelector

                anchors.left: parent.left
                anchors.right: parent.right
                width: parent.width

                model: typeModel
                delegate: typeDelegate

                onSelectedIndexChanged : {
                    categorySelector.model.categoryType = model.get(selectedIndex).enumType;
                }
            }

            Component {
                id: accountsDelegate
                OptionSelectorDelegate {
                    text: model.display.name;
                }
            }

            OptionSelector {
                id: accountSelector
                anchors.left: parent.left
                anchors.right: parent.right

                model: Book.accountsModel()
                delegate: accountsDelegate
            }

            Component {
                id: categoriesDelegate
                OptionSelectorDelegate {
                    text: model.display.name;
                }
            }

            OptionSelector {
                id: categorySelector
                anchors.left: parent.left
                anchors.right: parent.right

                containerHeight: itemHeight * 4
                model: Book.categoriesModelForType(Book.EXPENSE)
                delegate: categoriesDelegate
            }

            TextField {
                id: datePicker
                property date date: new Date()

                anchors.left: parent.left
                anchors.right: parent.right

                placeholderText: i18n.tr("Date")
                onDateChanged: {
                    console.log("Date changed!");
                    datePicker.text = Qt.formatDateTime(date, "dd/MM/yyyy");
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        var popup = PickerPanel.openDatePicker(datePicker, "date", "Days|Years|Months");
                        popup.picker.minimum = new Date(1900, 1, 1);
                    }
                }
            }

            TextField {
                id: amountField

                anchors.left: parent.left
                anchors.right: parent.right
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                validator: DoubleValidator {
                    id: doubleValidator
                    locale: "en"
                }

                placeholderText: i18n.tr("Amount")
            }

            TextField {
                id: contentsField

                anchors.left: parent.left
                anchors.right: parent.right

                placeholderText: i18n.tr("Contents")
            }

            TextArea {
                id: memoTextArea
                anchors.left: parent.left
                anchors.right: parent.right

                Layout.fillHeight: true

                width: parent.width
                autoSize: true

                placeholderText: i18n.tr("Memo")
            }
        }
    }
}
