import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: mainPage
    allowedOrientations: bothOrientation

    SilicaListView {
        id: mainList
        enabled: true
        visible: true
        clip: true
        anchors.fill: parent

        contentWidth: width
        header: PageHeader {
            title: "Daedalus"
        }
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
                    requestAlbums();
                    pageStack.push(Qt.resolvedUrl("AlbumsList.qml"));
                } else if ( ident=== "artists" ) {
                    requestArtists();
                    pageStack.push(Qt.resolvedUrl("ArtistsList.qml"));
                } else if ( ident=== "settings" ) {
                    pageStack.push(Qt.resolvedUrl("settings/SettingsPage.qml"));
                } else if ( ident === "playlist" ) {
//                    pageStack.push(Qt.resolvedUrl("CurrentPlaylistPage.qml"));
                    pageStack.navigateForward();
                }  else if ( ident === "alltracks" ) {
                    requestAllTracks();
                    pageStack.push(Qt.resolvedUrl("AlbumTracks.qml"), {
                                       artistname: "",
                                       albumname: "",
                                       allTracksPage: true
                                   })
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
                                 name: qsTr("all tracks"),
                                 ident: "alltracks"
                             })
        /*mainMenuModel.append({
                                 name: qsTr("files"),
                                 ident: "files"
                             })*/
//        mainMenuModel.append({
//                                 name: qsTr("search"),
//                                 ident: "search"
//                             })
        mainMenuModel.append({
                                 name: qsTr("settings"),
                                 ident: "settings"
                             })
    }

    Timer {
            id: showCurrentSongTimer
            interval: 15000
            repeat: false
            onTriggered: {
                    pageStack.navigateForward();
            }
        }

    onStatusChanged: {
            if (status === PageStatus.Active) {
                if ( mPlaylistPage == undefined) {
                    var playlistComponent = Qt.createComponent(Qt.resolvedUrl("CurrentPlaylistPage.qml"));
                    var playlistPage = playlistComponent.createObject(mainWindow);
                    mPlaylistPage = playlistPage;
                }

                pageStack.pushAttached(mPlaylistPage);
                showCurrentSongTimer.start()
            } else if (status === PageStatus.Deactivating) {
                if (showCurrentSongTimer.running) {
                    showCurrentSongTimer.stop()
                }
            }
            else if ( status === PageStatus.Activating ) {
            }
        }


}
