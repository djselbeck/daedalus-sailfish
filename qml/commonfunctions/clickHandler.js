
function artistClicked(artist, urn)
{
    mainWindow.artistname = artist;
    requestArtistAlbums(urn);
    pageStack.push(Qt.resolvedUrl("../pages/AlbumsList.qml"),{artistname:artist});
}

function albumClicked(artist,albumstring,albumurn)
 {
     requestAlbum(albumurn);
     mainWindow.artistname = artist;
     mainWindow.albumname = albumstring;
     pageStack.push(Qt.resolvedUrl("../pages/AlbumTracks.qml"),{artistname:artist,albumname:albumstring});
 }
