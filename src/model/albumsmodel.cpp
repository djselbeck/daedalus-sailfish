#include "albumsmodel.h"


// REmove me
#include <QSparqlResult>

AlbumsModel::AlbumsModel(QObject *parent, QSparqlConnection *connection) :
    QSparqlQueryModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
        mAlbumsQueryString = "SELECT ?album COUNT(?piece) as ?trackcount SUM(?duration) as ?totalduration ?artistname WHERE { ?piece a nmm:MusicPiece; nfo:duration ?duration; nmm:musicAlbum ?albumObj . ?albumObj nmm:albumTitle ?album . ?albumObj nmm:albumArtist ?albumartistObj . ?albumartistObj nmm:artistName ?albumartist } GROUP by ?album";
    }
}


void AlbumsModel::requestAlbums()
{
    // Initialize the query
    setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
}
