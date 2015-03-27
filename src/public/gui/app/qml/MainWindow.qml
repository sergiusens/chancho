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
import Ubuntu.Components.ListItems 1.0 as ListItems

import com.chancho 1.0

import "components"

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "com.ubuntu.developer.mandel.chancho"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    //automaticOrientation: true
    useDeprecatedToolbar: false

    width: units.gu(100)
    height: units.gu(75)

    Component {
        id: pageComponent

        PageWithBottomEdge {
            id: mainPage
            title: i18n.tr("Bills")

            Component {
                id: pageMenu
                ActionSelectionPopover {
                    delegate: ListItems.Standard {
                      text: action.text
                    }
                    actions: ActionList {
                      Action {
                          text: "Action one"
                          onTriggered: print(text)
                      }
                      Action {
                          text: "Action two"
                          onTriggered: print(text)
                      }
                    }
                }
            }

            head.actions: [
                Action {
                    iconName: "search"
                    text: i18n.tr("Search")
                },
                Action {
                    iconName: "settings"
                    text: i18n.tr("Settings")
                },
                Action {
                    iconName: "contextual-menu"
                    text: i18n.tr("Stats")
                },
                Action {
                    iconName: "contextual-menu"
                    text: i18n.tr("Accounts")
                }
            ]

            ColumnLayout {
                spacing: units.gu(2)

                anchors.fill: parent
                anchors.margins: units.gu(1)

                Label {
                    id: monthLabel
                    property date date: new Date()

                    anchors.left: parent.left
                    anchors.right: parent.right

                    text: Qt.formatDateTime(date, "MMMM yyyy");
                    fontSize: "x-large"
                    horizontalAlignment: Text.AlignHCenter

                    onDateChanged: {
                        text: Qt.formatDateTime(date, "MM yyyy");
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            PickerPanel.openDatePicker(monthLabel, "date");
                        }
                    }
                }


                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color : "light blue"

                    Layout.fillHeight: true

                    UbuntuListView {
                        id: daysList
                        width: parent.width
                        height:10000
                        spacing: units.gu(1)

                        model: Book.monthModel(new Date())
                        delegate: Item {
                            property var dayModel: Book.dayModel(model.display.day, model.display.month, model.display.year)
                            anchors.margins: units.gu(1)

                            width: parent.width
                            height: childrenRect.height

                            ColumnLayout {
                                width: parent.width
                                height: childrenRect.height

                                RowLayout {
                                    width: parent.width

                                    Row {
                                        spacing: units.gu(1)
                                        anchors.margins: units.gu(1)
                                        width: childrenRect.width

                                        UbuntuShape {
                                            id: dayNameRectangle

                                            color: "Orange"
                                            width: dayNameLabel.width
                                            height: dayLabel.height - units.dp(2)

                                            Label {
                                                id: dayNameLabel

                                                anchors.top: parent.top;
                                                anchors.bottom: parent.bottom;
                                                rotation: -90

                                                fontSize: "x-small"
                                                font.bold: true
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter

                                                text: "Thu"
                                            }
                                        }
                                        Label {
                                            id: dayLabel

                                            text: dayModel.day

                                            fontSize: "x-large"
                                            font.bold: true
                                            horizontalAlignment: Text.AlignCenter
                                            verticalAlignment: Text.AlignBottom
                                        }

                                        Label {
                                            id: monthYear
                                            height: dayLabel.height - units.dp(2)

                                            text: dayModel.month + "." + dayModel.year

                                            fontSize: "small"
                                            horizontalAlignment: Text.AlignCenter
                                            verticalAlignment: Text.AlignBottom
                                        }
                                    } // Row

                                    Label {
                                        Layout.alignment: Qt.AlignRight
                                        Layout.fillWidth: true
                                        id: outcomeValue
                                        height: dayLabel.height

                                        horizontalAlignment: Text.AlignRight
                                        verticalAlignment: Text.AlignBottom

                                        text: "0.0"
                                        color: "Red"
                                    }

                                    Label {
                                        Layout.alignment: Qt.AlignRight
                                        Layout.fillWidth: true
                                        id: incomeLabel
                                        height: dayLabel.height

                                        horizontalAlignment: Text.AlignRight
                                        verticalAlignment: Text.AlignBottom

                                        text: "0.0"
                                        color: "Green"
                                    }
                                }// RowLayout

                                Rectangle {
                                    id: separatorRectangle
                                    Layout.fillWidth: true
                                    width: parent.width
                                    height: units.dp(2)

                                    anchors.leftMargin: units.gu(1)
                                    anchors.rightMargin: units.gu(1)

                                    color: "Light grey"
                                }
                            } // ColumnLayout
                        }
                    }
                }

            }

            bottomEdgePageComponent: NewTransaction {}
            bottomEdgeTitle: i18n.tr("Add new entry")
        }
    }

    PageStack {
        id: stack
        Component.onCompleted: stack.push(pageComponent)
    }

}

