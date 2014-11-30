#include "albumsmodel.h"


// REmove me
#include <QSparqlResult>

AlbumsModel::AlbumsModel(QObject *parent, QSparqlConnection *connection) :
    QAbstractListModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
        mAlbumsQueryString = "SELECT ?title COUNT(?piece) as ?trackcount SUM(?duration) as ?totalduration ?artistname WHERE { ?piece a nmm:MusicPiece; nfo:duration ?duration; nmm:musicAlbum ?titleObj . ?titleObj nmm:albumTitle ?title . ?titleObj nmm:albumArtist ?titleartistObj . ?titleartistObj nmm:artistName ?artistname } GROUP by ?title";
    }
    mSparqlModel = new QSparqlQueryModel(this);
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
}


void AlbumsModel::requestAlbums()
{
    // Initialize the query
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
    endResetModel();
}

void AlbumsModel::sparqlModelfinished()
{
    beginResetModel();
    emit dataChanged(QModelIndex(),QModelIndex());
    qDebug() << "underlaying model finished result fetching";
    endResetModel();
}


QHash<int, QByteArray> AlbumsModel::roleNames() const {
    QHash<int,QByteArray> roles;

    roles[AlbumRole] = "title";
    roles[SectionRole] = "sectionprop";
    roles[ArtistRole] = "artist";
    roles[AlbumCleandRole] = "titleClean";
    roles[AlbumImageRole] = "coverURL";
    return roles;
}

int AlbumsModel::rowCount(const QModelIndex &parent) const {
    return mSparqlModel->rowCount(parent);
}

QVariant AlbumsModel::data(const QModelIndex &index, int role) const {
    QString albumTitle;
    switch ( role ) {
    case AlbumRole:
        return mSparqlModel->data(index,role);
        break;
    case SectionRole:
        albumTitle = mSparqlModel->data(index,AlbumRole).toString();
        return QString(albumTitle.at(0)).toUpper();
        break;
    case ArtistRole:

        break; case AlbumCleandRole:

        break;
    case AlbumImageRole:
        // FIXME
        return "";
        break;
    default:
        return "";
    }

    return "";
}

/**
 * This returns an pseudo item in an variant map for SectionScroller
 * @brief AlbumsModel::get
 * @param row
 * @return
 */
QVariantMap AlbumsModel::get(int row){
    QHash<int,QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> i(roles);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}
