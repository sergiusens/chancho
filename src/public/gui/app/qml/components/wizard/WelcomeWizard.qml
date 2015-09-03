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

import "../../components"
import "../models"

Walkthrough {
    id: walkthrough

    appName: "Chancho"
	completeColor: UbuntuColors.orange

    ListModel {
        id: accountsModel
    }

    Item{
        ListModel {
            id: incomeModel
            dynamicRoles: true
        }
        Component.onCompleted: {
            incomeModel.append({
                "name": i18n.tr("Allowance"),
                "color": "purple"
            });

            incomeModel.append({
                "name": i18n.tr("Salary"),
                "color": "green"
            });

            incomeModel.append({
                "name": i18n.tr("Bonus"),
                "color": "yellow"
            });

            incomeModel.append({
                "name": i18n.tr("Other"),
                "color": "red"
            });
        }
    }

    Item{
        ListModel {
            id: expenseModel
            dynamicRoles: true
        }
        Component.onCompleted: {
            expenseModel.append({
                "name": i18n.tr("Food"),
                "color": "purple"
            });

            expenseModel.append({
                "name": i18n.tr("Social Life"),
                "color": "fuchsia"
            });

            expenseModel.append({
                "name": i18n.tr("Self-development"),
                "color": "navy"
            });

            expenseModel.append({
                "name": i18n.tr("Transportation"),
                "color": "blue"
            });

            expenseModel.append({
                "name": i18n.tr("Culture"),
                "color": "teal"
            });

            expenseModel.append({
                "name": i18n.tr("Household"),
                "color": "aqua"
            });

            expenseModel.append({
                "name": i18n.tr("Apparel"),
                "color": "green"
            });

            expenseModel.append({
                "name": i18n.tr("Beauty"),
                "color": "lime"
            });

            expenseModel.append({
                "name": i18n.tr("Health"),
                "color": "yellow"
            });

            expenseModel.append({
                "name": i18n.tr("Education"),
                "color": "maroon"
            });

            expenseModel.append({
                "name": i18n.tr("Gift"),
                "color": "red"
            });

            expenseModel.append({
                "name": i18n.tr("Other"),
                "color": "gold"
            });
        }
    }

    titles: [i18n.tr("Accounts"), i18n.tr("Categories"), i18n.tr("Confirmation")]

    onFinished: {
        console.log("Welcome Wizard Complete!")
        // Here perhaps save isFirstRun variable to the disk
        pagestack.pop()
        pagestack.push(tabsComponent)
    }

    model: [
        AccountsPage {},
        CategoriesPage {},
        LastPage {}
    ]
}
