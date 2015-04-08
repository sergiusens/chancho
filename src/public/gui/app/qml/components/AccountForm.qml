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

    property var graphData
    property alias showGraph: chartShape.visible
    property alias color: colorChooser.color
    property alias name: nameField.text
    property alias memo: memoTextArea.text
    property alias showInitialAmount: initialAmount.visible
    property alias initialAmount: initialAmount.text

    onColorChanged: {
        if (graphData) {
            colorChooser.lightColor = Qt.lighter(color, 1.6);
            colorChooser.lightColor.a = 0.5;
            colorChooser.darkColor = Qt.darker(color, 1.2);

            if (showGraph) {
                chart.chartData.datasets[0]["fillColor"] = colorChooser.lightColor;
                chart.chartData.datasets[0]["strokeColor"] = colorChooser.color;
                chart.chartData.datasets[0]["pointColor"] = colorChooser.darkColor;
            }
        }
    }

    Component.onCompleted: {
        // grab the accounts and set the diff data
        if (!topShape.graphData) {
            var chartData = {
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
                datasets: []
            }
        }
        topShape.graphData = chartData;
    }

    onGraphDataChanged: {
        chart.chartData =  topShape.graphData;
        chart.repaint();
    }

    Component {
        id: popoverComponent
        ColorChooser {
            onColorChanged: {
                colorChooser.color = color;
                colorChooser.lightColor = Qt.lighter(color, 1.6);
                colorChooser.lightColor.a = 0.5
                colorChooser.darkColor = Qt.darker(color, 1.2);

                if (showGraph) {
                    // update the chart colors
                    chart.chartData.datasets[0]["fillColor"] = colorChooser.lightColor;
                    chart.chartData.datasets[0]["pointColor"] = colorChooser.darkColor;
                }
            }
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

        Component.onCompleted: {
            console.log("Height is " + mainColumn.height);
            flickable.contentHeight = mainColumn.height;
        }
        ColumnLayout {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: units.gu(1)
            }

            Item {
                id: nameAndColor

                anchors {
                    left: parent.left
                    right: parent.right
                    margins: units.gu(1)
                }

                height: childrenRect.height

                UbuntuShape {
                    id: colorChooser

                    property var lightColor: Qt.lighter(color, 1.6)
                    property var darkColor: Qt.darker(color, 1.2)

                    anchors {
                        top: parent.top
                        left: parent.left
                    }

                    color: "orange"

                    height: nameField.height
                    width: nameField.height

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            var properties = {
                                "shapeWidth": colorChooser.width,
                                "shapeHeight": colorChooser.height,
                                "color": colorChooser.color
                            };
                            PopupUtils.open(popoverComponent, colorChooser, properties);
                        }
                    }

                    onColorChanged: {
                        chart.repaint();
                    }
                }

                TextField {
                    id: nameField

                    anchors {
                        top: parent.top
                        left: colorChooser.right
                        right: parent.right
                        leftMargin: units.gu(1)
                    }

                    placeholderText: i18n.tr("Name")

                    onTextChanged: {
                        if (showGraph)
                            chart.chartData.datasets[0]["label"] = text;
                    }
                }
            }

            TextField {
                id: initialAmount

                anchors {
                    margins: units.gu(1)
                    left: parent.left
                    right: parent.right
                }

                inputMethodHints: Qt.ImhFormattedNumbersOnly

                validator: DoubleValidator {}

                placeholderText: i18n.tr("Initial amount")
            }

            TextField {
                id: memoTextArea

                anchors {
                    margins: units.gu(1)
                    left: parent.left
                    right: parent.right
                }

                placeholderText: i18n.tr("Memo")
            }

            UbuntuShape {
                id: chartShape

                Layout.fillHeight: true
                Layout.fillWidth: true

                anchors {
                    margins: units.gu(1)
                    left: parent.left
                    right: parent.right
                }

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
                        // grab the accounts and set the diff data
                        chart.chartData = topShape.graphData;
                    }
                }
            }  // chart u shape
        }
    }
}
