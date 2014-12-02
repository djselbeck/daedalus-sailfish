#include "albumsmodel.h"

#include <QUrl>

// REmove me
#include <QSparqlResult>

AlbumsModel::AlbumsModel(QObject *parent, QSparqlConnection *connection, QThread *fetchthread) :
    QAbstractListModel(parent)
{

    if ( connection != NULL ) {
        mConnection = connection;
    }
    mSparqlModel = new QSparqlQueryModel(0);
    if ( fetchthread != 0  ) {
        mThread = fetchthread;
        mSparqlModel->moveToThread(mThread);
    }
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
}


void AlbumsModel::requestAlbums()
{
    // Initialize the query
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mAlbumsQueryString = "SELECT ?albumname COUNT(distinct ?piece) as ?trackcount SUM( distinct ?tracklength) as ?totalduration ?artistname ?album WHERE { ?piece nmm:musicAlbum ?album . ?piece nfo:duration ?tracklength . ?album nmm:albumTitle ?albumname ; nmm:albumArtist ?artistIns . ?artistIns nmm:artistName ?artistname } GROUP BY ?album ORDER BY ?album";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
    endResetModel();
}

void AlbumsModel::requestAlbums(QString artist)
{
    qDebug() << "getting albums of: " << artist;
    // Initialize the query
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    mAlbumsQueryString = "SELECT ?albumname COUNT(?piece) as ?trackcount SUM(?tracklength) as ?totalduration ?artistname ?album WHERE { ?piece nmm:performer '" + artist + "'  . ?piece nmm:musicAlbum ?album . ?album nmm:albumTitle ?albumname . ?piece nfo:duration ?tracklength . ?piece nmm:performer ?performer . ?performer nmm:artistName ?artistname  } GROUP BY ?album ORDER BY ?album";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
}

void AlbumsModel::requestArtistAlbumsReverseFromTrack(QString urn)
{
    urn = urn.replace('<','\<');
    urn = urn.replace('>','\>');
    qDebug() << "getting albums of: " << urn;
    // Initialize the query
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    mAlbumsQueryString = "SELECT ?albumname COUNT(?piece) as ?trackcount SUM(?tracklength) as ?totalduration ?artistname ?album WHERE { <" + urn + "> nmm:performer ?artist . ?piece nmm:performer ?artist . ?piece nmm:musicAlbum ?album . ?album nmm:albumTitle ?albumname . ?piece nfo:duration ?tracklength . ?piece nmm:performer ?performer . ?performer nmm:artistName ?artistname  } GROUP BY ?album ORDER BY ?album";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);

}

void AlbumsModel::sparqlModelfinished()
{
    beginResetModel();
    qDebug() << "underlaying model finished result fetching";
    endResetModel();
}


QHash<int, QByteArray> AlbumsModel::roleNames() const {
    QHash<int,QByteArray> roles;

    roles[AlbumRole] = "title";
    roles[TrackCountRole] = "trackcount";
    roles[DurationRole] = "duration";
    roles[ArtistRole] = "artist";
    roles[AlbumURNRole] = "albumurn";
    roles[SectionRole] = "sectionprop";
    roles[AlbumCleandRole] = "titleClean";
    roles[AlbumImageRole] = "coverURL";
    return roles;
}

int AlbumsModel::rowCount(const QModelIndex &parent) const {
    return mSparqlModel->rowCount(parent);
}

QVariant AlbumsModel::data(const QModelIndex &index, int role) const {
    QString albumTitle;
    QUrl urn;
    switch ( role ) {
    case AlbumRole:
        return mSparqlModel->data(index,role);
        break;
    case AlbumURNRole:
        urn = mSparqlModel->data(index,role).toUrl();
        return urn.toEncoded();
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
