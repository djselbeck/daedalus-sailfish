import QtQuick 2.0
import Sailfish.Silica 1.0

Component {
    id: albumDelegate
    ListItem {
        menu: contextMenu
        contentHeight: ((listImageSize  === 1) || (listImageSize  === 0)  ? Theme.itemSizeSmall : (listImageSize  === 2 ? Theme.itemSizeMedium : Theme.itemSizeLarge) )
        Row {
            id: mainRow
            height: parent.height
            spacing: Theme.paddingSmall
            anchors {
                right: parent.right
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: listPadding
                rightMargin: listPadding
            }
            Rectangle {
                id: imageRectangle
                color: Theme.rgba(Theme.highlightBackgroundColor,0.2)
                width: ( listImageSize !== 0 ) ? mainRow.height : 0
                height: mainRow.height
                Image{
                    id: albumPicture
                    anchors.fill: parent
                    sourceSize.width: width
                    sourceSize.height: height
                    cache: true
                    asynchronous: true
                    //source: ( listImageSize === 0 ) ? "" : coverURL
                }
            }


            Label {
                id: albumLabel
                anchors.verticalCenter: parent.verticalCenter
                text: (album === "" ? qsTr("no album tag") : album)
            }
        }
        OpacityRampEffect {
            sourceItem: mainRow
            slope: 3
            offset: 0.65
        }

        onClicked: {
            listView.currentIndex = index;
            albumClicked(artistname, album);
            pageStack.push(Qt.resolvedUrl("../pages/database/AlbumTracksPage.qml"),{artistname:artistname,albumname:album});
        }
        function playAlbumRemorse() {
            remorseAction(qsTr("playing album"), function () {
                playAlbum([artistname, album])
            }, 3000)
        }
        function addAlbumRemorse() {
            remorseAction(qsTr("adding album"), function () {
                addAlbum([artistname, album])
            }, 3000)
        }
        Component {
            id: contextMenu
            ContextMenu {
                MenuItem {
                    text: qsTr("play album")
                    onClicked: {
                        if (album !== "") {
                            playAlbumRemorse()
                        }
                    }
                }

                MenuItem {
                    text: qsTr("add album to list")
                    onClicked: {
                        if (album !== "") {
                            addAlbumRemorse()
                        }
                    }
                }
            }
        }
    }
}
