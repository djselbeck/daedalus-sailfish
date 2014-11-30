function artistClicked(artist, urn) {
    mainWindow.artistname = artist
    requestArtistAlbums(urn)
    pageStack.push(Qt.resolvedUrl("../pages/AlbumsList.qml"), {
                       artistname: artist
                   })
}

function albumClicked(artist, albumstring, albumurn) {
    requestAlbum(albumurn)
    mainWindow.artistname = artist
    mainWindow.albumname = albumstring
    pageStack.push(Qt.resolvedUrl("../pages/AlbumTracks.qml"), {
                       artistname: artist,
                       albumname: albumstring
                   })
}

function albumTrackClicked(title, album, artist, lengthformatted, uri, year, tracknr, discnr, index) {
    pageStack.push(Qt.resolvedUrl("../pages/SongDialog.qml"), {
                       title: title,
                       album: album,
                       artist: artist,
                       filename: uri,
                       lengthtext: lengthformatted,
                       date: "FIXME",
                       nr: tracknr,
                       discnr: discnr,
                       index: index
                   })
}
