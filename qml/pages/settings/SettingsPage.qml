import QtQuick 2.0
import Sailfish.Silica 1.0
import "../../components"

Page
{
    id: settingsPage;
    allowedOrientations: bothOrientation
    SilicaListView {
        id: settingsListView
        anchors.fill: parent
//        anchors.bottomMargin: quickControlPanel.visibleSize
        clip: true
        contentWidth: width
        header: PageHeader {
            title: qsTr("settings");
        }
        model: settingsMenuModel
        delegate: BackgroundItem {
            Label {
                clip: true
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                    leftMargin: listPadding
                    rightMargin: listPadding
                }
                text: name
            }
            onClicked: {
                parseClickedSettings(ident);
            }
        }
    }


    Component.onCompleted: {
        settingsMenuModel.append({"name":qsTr("artwork settings"), "ident":"database"})
        settingsMenuModel.append({"name":qsTr("gui settings"), "ident":"guisettings"})
        settingsMenuModel.append({"name":qsTr("scrobbling"), "ident":"scrobbling"})
        settingsMenuModel.append({"name":qsTr("about"), "ident":"about"})
        // Debug-only
        if(mDebugEnabled) {
            settingsMenuModel.append({"name":qsTr("garbage collection"), "ident":"gc"})
        }
    }

    ListModel {
        id: settingsMenuModel
    }

    function parseClickedSettings(ident)
    {
        switch (ident) {
        case "about" :
            pageStack.push(Qt.resolvedUrl("AboutPage.qml"));
            break;
        case "database" :
            pageStack.push(Qt.resolvedUrl("DatabaseSettings.qml"))
            break;
        case "guisettings" :
            pageStack.push(Qt.resolvedUrl("GUISettings.qml"))
            break;
        case "scrobbling" :
            pageStack.push(Qt.resolvedUrl("ScrobbleSettings.qml"))
            break;
        case "gc" :
            console.debug("Calling garbage collection")
            gc();
            console.debug("Called garbage collection")
            break;
        }
    }
}
