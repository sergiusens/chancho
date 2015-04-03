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
import Ubuntu.Components.ListItems 0.1 as ListItems

import jbQuick.Charts 1.0

import com.chancho 1.0

UbuntuShape {
    id: topShape

    property alias color: colorChooser.color
    property alias name: nameField.text
    property alias categoryTypeModel: typeModel
    property alias categoryTypeSelectedIndex: typeSelector.selectedIndex

    onColorChanged: {
        console.log("Color is " + color);
        colorChooser.lightColor = Qt.lighter(color, 1.6);
        colorChooser.darkColor = Qt.darker(color, 1.2);

        chart.chartData.datasets[0]["fillColor"] = colorChooser.lightColor;
        chart.chartData.datasets[0]["pointColor"] = colorChooser.darkColor;
    }

    Component {
        id: popoverComponent
        ColorChooser {
            onColorChanged: {
                colorChooser.color = color;
                colorChooser.lightColor = Qt.lighter(color, 1.6);
                colorChooser.darkColor = Qt.darker(color, 1.2);

                // update the chart colors
                chart.chartData.datasets[0]["fillColor"] = colorChooser.lightColor;
                chart.chartData.datasets[0]["pointColor"] = colorChooser.darkColor;
            }
        }
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.gu(1)

        Item {
            id: nameAndColor
            anchors.margins: units.gu(1)
            anchors.left: parent.left
            anchors.right: parent.right
            height: childrenRect.height

            UbuntuShape {
                id: colorChooser
                anchors.top: parent.top
                anchors.left: parent.left

                color: "orange"
                property var lightColor: Qt.lighter(color, 1.6)
                property var darkColor: Qt.darker(color, 1.2)

                height: nameField.height
                width: nameField.height

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        PopupUtils.open(popoverComponent, colorChooser, {"color": colorChooser.color})
                    }
                }

                onColorChanged: {
                    chart.repaint();
                }
            }

            TextField {
                id: nameField
                anchors.top: parent.top
                anchors.left: colorChooser.right
                anchors.right: parent.right
                anchors.leftMargin: units.gu(1)

                placeholderText: i18n.tr("Name")

                onTextChanged: {
                    chart.chartData.datasets[0]["label"] = text;
                }
            }
        }

        Component {
            id: typeDelegate
            OptionSelectorDelegate {
                text: name;
            }
        }

        ListModel {
            id: typeModel
            ListElement {
                name: "Expense";
                enumType: Book.EXPENSE;
            }
            ListElement {
                name: "Income";
                enumType: Book.INCOME;
            }
        }

        OptionSelector {
            id: typeSelector

            anchors.margins: units.gu(1)
            anchors.left: parent.left
            anchors.right: parent.right

            model: typeModel
            delegate: typeDelegate
        }

        UbuntuShape {
            id: chartShape
            Layout.fillHeight: true
            Layout.fillWidth: true

            anchors.margins: units.gu(1)
            anchors.left: parent.left
            anchors.right: parent.right
            height: childrenRect.height
            color: "white"

            Chart {
                id: chart
                anchors.fill: parent

                chartAnimated: true;
                chartAnimationEasing: Easing.InOutElastic;
                chartAnimationDuration: 2000;
                chartType: Charts.ChartType.LINE;

                Component.onCompleted: {
                    chartData = {
                        labels: [
                            i18n.tr("January"),
                            i18n.tr("February"),
                            i18n.tr("March"),
                            i18n.tr("April"),
                            i18n.tr("May"),
                            i18n.tr("June"),
                            i18n.tr("July"),
                            i18n.tr("August"),
                            i18n.tr("September"),
                            i18n.tr("October"),
                            i18n.tr("November"),
                            i18n.tr("December")],
                        datasets: [{
                            label: nameField.text,
                            fillColor: colorChooser.lightColor,
                            strokeColor: colorChooser.color,
                            pointColor: colorChooser.darkColor,
                            pointStrokeColor: "#fff",
                            pointHighlightFill: "#fff",
                            pointHighlightStroke: "rgba(220,220,220,1)",
                            data: [65, -59, 80.5, 81, -156, 55, 140, 90, 45, 12, 98, 25]
                        }]
                    }
                }
            }
        }  // chart u shape
    }
}
