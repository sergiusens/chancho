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

import "js/accounts_wizard.js" as AccountsWizardJs

Dialog {
    id: dialogue
    property alias color: colorChooser.color
    property var accountsModel

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
    TextField {
        id: initialAmountField

        inputMethodHints: Qt.ImhFormattedNumbersOnly
        validator: DoubleValidator {}
        placeholderText: i18n.tr("Initial amount")
    }

    TextField {
       id: memoField
       placeholderText: i18n.tr("Memo")
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
            id: okButton
            enabled: false
            Layout.fillWidth: true
            text: "ok"
            color: UbuntuColors.orange
            onClicked: {
                Qt.inputMethod.commit()
                AccountsWizardJs.onAddAccountOkClicked(nameField, initialAmountField,
                    memoField, colorChooser, accountsModel);
                PopupUtils.close(dialogue)
            }
        }
        Button {
            Layout.fillWidth: true
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(dialogue)
        }
    }
}
