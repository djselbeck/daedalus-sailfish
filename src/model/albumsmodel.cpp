#include "albumsmodel.h"


// REmove me
#include <QSparqlResult>

AlbumsModel::AlbumsModel(QObject *parent, QSparqlConnection *connection) :
    QSparqlQueryModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
        mAlbumsQueryString = "SELECT ?title COUNT(?piece) as ?trackcount SUM(?duration) as ?totalduration ?artistname WHERE { ?piece a nmm:MusicPiece; nfo:duration ?duration; nmm:musicAlbum ?titleObj . ?titleObj nmm:albumTitle ?title . ?titleObj nmm:albumArtist ?titleartistObj . ?titleartistObj nmm:artistName ?artistname } GROUP by ?title";
    }
    connect(this,SIGNAL(finished()),this,SIGNAL(albumsReady()));
}


void AlbumsModel::requestAlbums()
{
    // Initialize the query
    clear();
    qDebug() << "getting albums";
    setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
}
