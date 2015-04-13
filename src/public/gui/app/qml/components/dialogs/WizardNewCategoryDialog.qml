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

import "../models"
import "js/categories_wizard.js" as CategoriesWizardJs

Dialog {
    id: dialogue
    property alias color: colorChooser.color
    property var incomeModel
    property var expenseModel

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

        model: typeModel
        delegate: typeDelegate
    }

    TextField {
        id: nameField

        onTextChanged: {
            if (text === "") {
                okButton.enabled = false;
            } else {
                okButton.enabled = true;
           }
       }

       placeholderText: i18n.tr("Name")

    }

    Row {
        spacing: units.gu(1)
        property var accountColor: Qt.rgba(Math.random(), Math.random(),Math.random(),1)

        UbuntuShape {
           id: colorChooser
           width: nameField.height
           height: nameField.height
           color: parent.accountColor

        }
        TextField {
            id: colorField
            text: parent.accountColor

            onTextChanged: {
                colorChooser.color = text;
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: units.gu(1)

        Button {
            Layout.fillWidth: true
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(dialogue)
        }

        Button {
            id: okButton
            enabled: false
            Layout.fillWidth: true
            text: i18n.tr("Ok")
            color: UbuntuColors.orange
            onClicked: {
                Qt.inputMethod.commit()
                CategoriesWizardJs.onAddCategoryOkClicked(nameField, typeSelector,
                    colorChooser, incomeModel, expenseModel);
                PopupUtils.close(dialogue)
            }
        }
    }
}
