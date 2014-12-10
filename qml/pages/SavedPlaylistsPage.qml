import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page
{
    id: savedPlaylistPage
    allowedOrientations: bothOrientation
    property string artistname;
    SilicaListView {
            id : savedPlaylistsListView
            SpeedScroller {
                listview: savedPlaylistsListView
            }
            ScrollDecorator {}
            quickScrollEnabled: jollaQuickscroll
            anchors.fill: parent
//            anchors.bottomMargin: quickControlPanel.visibleSize
            contentWidth: width
            header: PageHeader {
                title: qsTr("playlists")
            }
            model: savedPlaylistsModel
            delegate: ListItem {
                menu: contextMenu
                Column{
                    id: mainColumn
                    anchors {
                        right: parent.right
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: listPadding
                        rightMargin: listPadding
                    }
                         Label{
                             text: name === undefined ? playlisturl : name
                        }
                    }
                OpacityRampEffect {
                    sourceItem: mainColumn
                    slope: 3
                    offset: 0.65
                }
                onClicked: {
                    console.debug("Clicked PL: " + playlisturl);
                    requestSavedPlaylist(playlisturl);
                    pageStack.push(Qt.resolvedUrl("PlaylistTracksPage.qml"),{playlistname:name === undefined ? playlisturl : name})
                }
                function playListRemorse() {
                    remorseAction(qsTr("playing list"), function() { playPlaylist(modelData); },3000)
                }
                function addListRemorse() {
                    remorseAction(qsTr("adding list"), function() { addPlaylist(modelData); },3000)
                }
                Component {

                            id: contextMenu
                            ContextMenu {
                                MenuItem {
                                    text: qsTr("play playlist");
                                    onClicked: {
                                        playListRemorse();
                                    }
                                }

                                MenuItem {
                                    text: qsTr("add list")
                                    onClicked: {
                                        addListRemorse();
                                    }
                                }
                                MenuItem {
                                    text: qsTr("delete list")
                                    onClicked: {
                                        pageStack.push(deleteSavedPlaylistQuestion,{playlisturl:playlisturl});
                                        //deletePlaylist(playlisturl);
                                    }
                                }
                            }
                }
            }
    }
    Component.onDestruction: {
        clearSavedPlaylists();
        console.debug("Clearing playlists");
    }

    Dialog
    {
        id: deleteSavedPlaylistQuestion
        property string playlisturl;
        property string playlistname;
        anchors.fill: parent
        Column {
            width: parent.width
            spacing: 10
            anchors.margins: Theme.paddingMedium
            DialogHeader {
                acceptText: qsTr("delete playlist");
            }
            Label {
                width: parent.width
                wrapMode: Text.WordWrap
                height: implicitHeight
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("Do you really want to delete this playlist? This step is irreversible!")
             }
        }
        onDone: {
            if ( result == DialogResult.Accepted)
            {
                deletePlaylistFile(playlisturl);
                pageStack.pop(mPlaylistPage);
            }
        }
    }
}
