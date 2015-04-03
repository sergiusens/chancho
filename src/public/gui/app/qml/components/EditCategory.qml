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

Page {
    id: page
    property var category

    onCategoryChanged: {
        form.name = category.name;
        if (category.type == Book.EXPENSE) {
            form.categoryTypeSelectedIndex = 0;
        } else {
            form.categoryTypeSelectedIndex = 1;
        }
        form.color = category.color;
    }

    title: "Edit account"

    head.actions: [
        Action {
            iconName: "edit"
            text: i18n.tr("Edit")
            onTriggered: {
                PopupUtils.open(editDialog, page);
            }
        },
        Action {
            iconName: "delete"
            text: i18n.tr("Delete")
            onTriggered: {
                PopupUtils.open(deleteDialog, page);
            }
        }
    ]

    Component {
         id: deleteDialog

         Dialog {
             id: dialogue
             title: i18n.tr("Delete entry")
             text: i18n.tr("Do you want to remove this entry?")
             Button {
                 text: i18n.tr("ok")
                 color: UbuntuColors.orange
                 onClicked: {
                    Book.removeCategory(category);
                    PopupUtils.close(dialogue);
                    categoriesPageStack.pop();
                 }
             }
             Button {
                 text: i18n.tr("cancel")
                 onClicked: {
                    PopupUtils.close(dialogue);
                 }
             }
         }
    }

    Component {
         id: editDialog

         Dialog {
             id: dialogue
             title: i18n.tr("Edit entry")
             text: i18n.tr("Do you want to update this entry?")
             Button {
                 text: i18n.tr("ok")
                 color: UbuntuColors.orange
                 onClicked: {
                    var type = form.categoryTypeModel.get(form.categoryTypeSelectedIndex);
                    type = type.enumType
                    Book.updateCategory(category, form.name, form.color, type);
                    categoriesPageStack.pop();
                    PopupUtils.close(dialogue);
                 }
             }
             Button {
                 text: i18n.tr("cancel")
                 onClicked: {
                    PopupUtils.close(dialogue);
                 }
             }
         }
    }

    CategoryForm {
        id: form
        anchors.fill: parent
        anchors.margins: units.gu(1)
    }
}
