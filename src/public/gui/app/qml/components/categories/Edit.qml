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
import "js/categories.js" as CategoriesJs

Page {
    id: page
    property var category

    Component.onCompleted: {
        // connect to the diff signals to ensure that we do redraw the graph
        Book.transactionStored.connect(redrawGraph);
        Book.transactionRemoved.connect(redrawGraph);
        Book.transactionUpdated.connect(redrawGraph);

        Book.categoryStored.connect(redrawGraph);
        Book.categoryUpdated.connect(redrawGraph);
        Book.categoryRemoved.connect(redrawGraph);

        Book.accountStored.connect(redrawGraph);
        Book.accountRemoved.connect(redrawGraph);
        Book.accountUpdated.connect(redrawGraph);
    }

    function redrawGraph() {
        console.log("Redraw graph bar graph!");
        var date = new Date();
        var graphData = CategoriesJs.calculateGraphBarData(Book, category, date);
        form.graphData = graphData;
        form.color = category.color;
    }

    onCategoryChanged: {
        form.name = category.name;
        if (category.type == Book.EXPENSE) {
            form.categoryTypeSelectedIndex = 0;
        } else {
            form.categoryTypeSelectedIndex = 1;
        }
        var date = new Date();
        var graphData = CategoriesJs.calculateGraphBarData(Book, category, date);
        form.graphData = graphData;
        form.color = category.color;
    }

    title: "Edit category"

    head.actions: [
        Action {
            iconName: "edit"
            text: i18n.tr("Edit")
            onTriggered: {
                Qt.inputMethod.commit()
                var editCategoriesCb = function() {
                    var type = form.categoryTypeModel.get(form.categoryTypeSelectedIndex);
                    type = type.enumType
                    Book.updateCategory(category, form.name, form.color, type);
                    categoriesPageStack.pop();
                };
                var properties = {
                    "title": i18n.tr("Edit entry"),
                    "text": i18n.tr("Do you want to update this category?"),
                    "okCallback": editCategoriesCb
                };
                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmationDialog.qml"), page, properties);
            }
        },
        Action {
            iconName: "delete"
            text: i18n.tr("Delete")
            onTriggered: {
                // grab the number of categories we have and if we are trying to remove the last one
                // we will stop the user
                var numberOfCats = Book.numberOfCategories(category.type);
                if (numberOfCats == 1) {
                    console.log("Trying to delete the last category.");
                    var properties = {
                        "title": i18n.tr("Removal Error"),
                        "text": i18n.tr("You cannot delete all categories in the system. You must have at least one of each type")
                    };
                    PopupUtils.open(Qt.resolvedUrl("../dialogs/ErrorDialog.qml"), page, properties);
                } else {
                    var deleteCategoriesCb = function() {
                        Book.removeCategory(category);
                        categoriesPageStack.pop();
                    };
                    var properties = {
                        "title": i18n.tr("Delete category"),
                        "text": i18n.tr("Do you want to remove this category?"),
                        "okCallback": deleteCategoriesCb
                    };
                    PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmationDialog.qml"), page, properties);
                }
            }
        }
    ]

    Form {
        id: form
        anchors.fill: parent
        anchors.margins: units.gu(1)
    }
}
