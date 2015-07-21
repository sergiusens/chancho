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

        Text {
            id: categoriesIntroLabel
            Layout.fillWidth: true

            wrapMode: Text.WordWrap
            text: i18n.tr("Categories allow you to indentify the different transactions. There are two different types: income and expense categories. The following are some of the predefined ones but you can add as many as you want.")
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

                delegate: Component {
                    Loader {
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: units.gu(1)
                        }

                        property string modelName: model.name
                        property string modelColor: model.color
                        property int numberOfCategories: incomeModel.count

                        property var onClickCallback: function (rootItem) {
                            rootItem.selected = !rootItem.selected;
                            incomeModel.setProperty(index, "selected", rootItem.selected);
                        }

                        source: "./categories/Category.qml"
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

                delegate: Component {
                    Loader {
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: units.gu(1)
                        }

                        property string modelName: model.name
                        property string modelColor: model.color
                        property int numberOfCategories: expenseModel.count

                        property var onClickCallback: function (rootItem) {
                            rootItem.selected = !rootItem.selected;
                            expenseModel.setProperty(index, "selected", rootItem.selected);
                        }

                        source: "./categories/Category.qml"
                    }
                }
            } // List View
        } // Ubuntu shape list view

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: i18n.tr("Remove")
                onClicked: {
                    var incomeAfterRemoval = incomeModel.count - CategoriesWizardJs.numberOfSelectedItems(incomeModel);
                    var expenseAfterRemoval = expenseModel.count - CategoriesWizardJs.numberOfSelectedItems(expenseModel);

                    if (incomeAfterRemoval <= 0 || expenseAfterRemoval <= 0) {
                        console.log("Trying to delete the last category.");
                        var properties = {
                            "title": i18n.tr("Removal Error"),
                            "text": i18n.tr("You cannot delete all categories in the system. You must have at least one of each type")
                        };
                        PopupUtils.open(Qt.resolvedUrl("dialogs/ErrorDialog.qml"), page, properties);
                        return;
                    }
                    var showConfirmationDialog = CategoriesWizardJs.areCategoriesSelected(incomeModel, expenseModel);
                    console.log("Should we show the removal dialog " + showConfirmationDialog);
                    if (showConfirmationDialog) {
                        console.log("Categories selected for removal.");
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
        }
    } // ColumnLayout
}
