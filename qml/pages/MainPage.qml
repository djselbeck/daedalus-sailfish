import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: mainPage

    SilicaListView {
        id: mainList
        enabled: true
        visible: true
        clip: true
        anchors.fill: parent

        contentWidth: width

        model: mainMenuModel
        delegate: ListItem {
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
                if ( ident=== "albums" ) {
                    pageStack.push(Qt.resolvedUrl("AlbumsList.qml"));
                }
            }
        }
    }

    ListModel {
        id: mainMenuModel
    }

    Component.onCompleted: {
        mainMenuModel.append({
                                 name: qsTr("playlist"),
                                 ident: "playlist"
                             })
        mainMenuModel.append({
                                 name: qsTr("artists"),
                                 ident: "artists"
                             })
        mainMenuModel.append({
                                 name: qsTr("albums"),
                                 ident: "albums"
                             })
        mainMenuModel.append({
                                 name: qsTr("files"),
                                 ident: "files"
                             })
        mainMenuModel.append({
                                 name: qsTr("search"),
                                 ident: "search"
                             })
        mainMenuModel.append({
                                 name: qsTr("connect"),
                                 ident: "connectto"
                             })
        mainMenuModel.append({
                                 name: qsTr("settings"),
                                 ident: "settings"
                             })
    }
}
