import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import "../commonfunctions/clickHandler.js" as ClickHandler

Page
{
    id: playlistTracksPage
    allowedOrientations: bothOrientation
    property string playlistname;
    property int lastIndex;
    SilicaListView {
            id : playlistTracksListView
            model: playlistTracksModel
            // FIXME SpeedScroller
            quickScrollEnabled: jollaQuickscroll
            SpeedScroller {
                id: speedScroller
                listview: playlistTracksListView
            }
            onCountChanged:
            {
                speedScroller.reReadSections();
            }

            ScrollDecorator {}
            anchors.fill: parent
//            anchors.bottomMargin: quickControlPanel.visibleSize
            contentWidth: width
            clip:true
            header: PageHeader {
                title: playlistname;
            }
            populate: Transition {
                NumberAnimation { properties: "x"; from:playlistTracksListView.width*2 ;duration: populateDuration }
            }
            PullDownMenu {
                MenuItem {
                    text: qsTr("delete list")
                    onClicked: {
                        deleteSavedPlaylistQuestion.playlistname = playlistname;
                        pageStack.openDialog(deleteSavedPlaylistQuestion);
                    }
             }
                MenuItem {
                    text: qsTr("add list")
                    onClicked: {
                        addActivePlaylist()
                        pageStack.pop();
                    }
                }
                MenuItem {
                    text: qsTr("play list")
                    onClicked: {
                        playActivePlaylist()
                        // GO BACK
                        pageStack.pop();
                    }
                }
            }

            section {
                delegate: Loader {
                    active: sectionsInPlaylist && visible
                    height: sectionsInPlaylist ? Theme.itemSizeMedium : 0
                    width: parent.width
                    sourceComponent: PlaylistSectionDelegate{
                        width:undefined
                    }
                }
                property: "section"
            }

            delegate: ListItem {
                menu: contextMenu
                contentHeight: mainColumn.height
                Column {
                    id: mainColumn
                    clip: true
                    height: (trackRow + artistLabel
                             >= Theme.itemSizeSmall ? trackRow + artistLabel : Theme.itemSizeSmall)
                    anchors {
                        right: parent.right
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: listPadding
                        rightMargin: listPadding
                    }
                    Row {
                        id: trackRow
                        Label {
                            text: (index + 1) + ". "
                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                        }
                        Label {
                            clip: true
                            wrapMode: Text.WrapAnywhere
                            elide: Text.ElideRight
                            text: (title === "" ? filename + " " : title + " ")
                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                        }
                        Label {
                            text: (length === 0 ? "" : " (" + lengthformatted + ")")
                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                    Label {
                        id: artistLabel
                        text: (artist !== "" ? artist + " - " : "") + (album !== "" ? album : "")
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
                OpacityRampEffect {
                    sourceItem: mainColumn
                    slope: 3
                    offset: 0.65
                }
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("SongDialog.qml"), {
                                       title: title,
                                       album: album,
                                       artist: artist,
                                       filename: fileurl,
                                       lengthtext: lengthformatted,
                                       date: "FIXME",
                                       nr: tracknr,
                                       discnr: discnr,
                                       index: index,
                                       playlistTrack: true
                                   })
                }
                function playTrackRemorse() {
                    remorseAction(qsTr("playing track"), function() {
                        playSavedPlaylistTrack(index); },3000)
                }
                function addTrackRemorse() {
                    remorseAction(qsTr("adding track"), function() {
                        addSavedPlaylistTrack(index); },3000)
                }
                function addTrackAfterCurrentRemorse() {
                    remorseAction(qsTr("adding track"), function () {
                        addSavedPlaylistTrackAfterCurrent(index)
                    }, 3000)
                }
                Component {
                    id: contextMenu
                    ContextMenu {
                        MenuItem {
                            text: qsTr("play track");
                            onClicked: {
                                playTrackRemorse();
                            }
                        }

                        MenuItem {
                            text: qsTr("add track to list")
                            onClicked: {
                                addTrackRemorse();
                            }
                        }
                        MenuItem {
                            text: qsTr("play after current")
                            onClicked: {
                                addTrackAfterCurrentRemorse();
                            }
                        }
                    }
                }
            }
    }
    Dialog
    {
        id: deleteSavedPlaylistQuestion
        property string playlistname;
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
                deleteActivePlaylist();
                pageStack.pop(mPlaylistPage);
            }
        }
    }

    onStatusChanged: {
        if ( status === PageStatus.Deactivating ) {
            lastIndex = playlistTracksListView.currentIndex;
        }
        else if ( status === PageStatus.Activating ) {
            playlistTracksListView.positionViewAtIndex(lastIndex,ListView.Center);
        }
    }

    Component.onDestruction: {
        clearSavedPlaylistTracks();
    }
}
