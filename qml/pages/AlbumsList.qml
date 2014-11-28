import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page
{
    id: albumsListPage
    property string artistname: "";
    SilicaListView {
                    id: listView
                    clip: true
                    model: albumsModel
                    anchors.fill: parent
                    quickScrollEnabled: jollaQuickscroll
                    //FIXME LATER
/*                    SectionScroller {
                        listview: listView
                        landscape: false
                        sectionPropertyName: "sectionprop"
                    }*/
//                    populate: Transition {
//                        NumberAnimation {
//                            properties: "x"
//                            from: listView.width * 2
//                            duration: populateDuration
//                        }
//                    }
                    ScrollDecorator {
                    }

                    header: PageHeader {
                        title: artistname !== "" ? artistname : qsTr("albums")
                        width: parent.width
                        height: Theme.itemSizeMedium
                    }
                    PullDownMenu {
                        enabled: artistname !== ""
                        MenuItem {
                            text: qsTr("add albums")
                            onClicked: {
                                addArtist(artistname)
                            }
                        }
                        MenuItem {
                            text: qsTr("play albums")
                            onClicked: {
                                playArtist(artistname)
                            }
                        }
                    }
                    delegate: AlbumListDelegate {
                    }
//                    section {
//                        property: 'sectionprop'
//                        delegate: SectionHeader {
//                            text: section
//                        }
//                    }
//                    Component.onCompleted: {
//                        console.debug("ListView created, model size: " + albumsModel.count());
//                    }
                }

}
