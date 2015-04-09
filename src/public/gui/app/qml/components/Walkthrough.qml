import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Page {
    id: walkthrough

    // Property to set the app name used in the walkthrough
    property string appName

    // Property to check if this is the first run or not
    property bool isFirstRun: true

    // Property to store the slides shown in the walkthrough (Each slide is a component defined in a separate file for simplicity)
    property list<Component> model

    // Property to set the color of bottom cirle to indicate the user's progress
    property color completeColor: "green"

    // Property to set the color of the bottom circle to indicate the slide still left to cover
    property color inCompleteColor: "grey"

    // Property to set the color of the skip welcome wizard text
    property color skipTextColor: "grey"

    property var titles: []

    // Property to signal walkthrough completion
    signal finished

    title:"Foo"

    head.backAction: Action {
        iconName: "navigation-menu"
        enabled: false
    }

    // ListView to show the slides
    ListView {
        id: listView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: separator.top
        }

        model: walkthrough.model
        snapMode: ListView.SnapOneItem
        orientation: Qt.Horizontal
        highlightMoveDuration: UbuntuAnimation.FastDuration
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true

        delegate: Item {
            width: listView.width
            height: listView.height
            clip: true

            Loader {
                anchors {
                    fill: parent
                    margins: units.gu(2)
                }

                sourceComponent: modelData
            }
        }

        onCurrentIndexChanged: {
            console.log("Change the title");
            if (title.length > currentIndex) {
                walkthrough.title = titles[currentIndex];
            }
        }
    }

    // Separator between walkthrough slides and slide indicator
    ListItem.ThinDivider {
        id: separator
        anchors.bottom: slideIndicator.top
    }

    // Indicator element to represent the current slide of the walkthrough
    Row {
        id: slideIndicator
        height: units.gu(6)
        spacing: units.gu(2)
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        Repeater {
            model: walkthrough.model.length
            delegate: Rectangle {
                height: width
                radius: width/2
                width: units.gu(2)
                antialiasing: true
                anchors.verticalCenter: parent.verticalCenter
                color: listView.currentIndex >= index ? completeColor : inCompleteColor
                Behavior on color {
                    ColorAnimation {
                        duration: UbuntuAnimation.FastDuration
                    }
                }
            }
        }
    }
}
