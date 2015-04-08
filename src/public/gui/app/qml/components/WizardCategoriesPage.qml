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

import "js/categories_wizard.js" as CategoriesWizardJs

Component {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.gu(2) /* two unit so that we have the same as the main page. */
        spacing: units.gu(2)

        Label {
            id: titleLabel
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.WordWrap

            text: i18n.tr("Categories");
            fontSize: "x-large"
        }

        Text {
            id: categoriesIntroLabel
            Layout.fillWidth: true

            wrapMode: Text.WordWrap
            text: i18n.tr("Categories allow you to indentify the different transactions. There are two different "
                + "types income an expense categories. The following are some of the predefined ones but you can "
                + "add as many as you want.")
        }

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
                clip: true

                spacing: units.gu(1)
                model: incomeModel
                delegate: CategoryComponent {
                    name: model.name
                    color: model.color
                    numberOfCategories: incomeModel.count

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            parent.selected = !parent.selected;
                            incomeModel.setProperty(index, "selected", parent.selected);
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
                clip: true
                spacing: units.gu(1)
                model: expenseModel
                delegate: CategoryComponent {
                    name: model.name
                    color: model.color
                    numberOfCategories: expenseModel.count

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            parent.selected = !parent.selected;
                            expenseModel.setProperty(index, "selected", parent.selected);
                        }
                    }
                }
            } // List View
        } // Ubuntu shape list view

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: i18n.tr("Add")
                color: UbuntuColors.orange
                onClicked: {
                    var properties = {
                        "incomeModel": incomeModel,
                        "expenseModel": expenseModel
                    };
                    PopupUtils.open(Qt.resolvedUrl("dialogs/WizardNewCategoryDialog.qml"), page, properties);
                }
            }
            Button {
                text: i18n.tr("Remove")
                onClicked: {
                    var showConfirmationDialog = CategoriesWizardJs.areCategoriesSelected(incomeModel, expenseModel);
                    if (showConfirmationDialog) {
                        var deleteCategoriesCb = function() {
                            CategoriesWizardJs.deleteCategoriesCallback(incomeModel, expenseModel);
                        };
                        var properties = {
                            "title": i18n.tr("Delete categories"),
                            "text": i18n.tr("Do you want to delete the selected categories?"),
                            "okCallback": deleteCategoriesCb
                        };
                        PopupUtils.open(Qt.resolvedUrl("dialogs/ConfirmationDialog.qml"), page, properties);
                    }
                }
            }
        }
    } // ColumnLayout
}
