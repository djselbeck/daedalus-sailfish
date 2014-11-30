#include "albumtracksmodel.h"

AlbumTracksModel::AlbumTracksModel(QObject *parent, QSparqlConnection *connection) :
    QAbstractListModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
        mAlbumTracksQueryString = "SELECT  ?artist  COUNT(?album) as ?albumcount WHERE { ?album nmm:albumArtist ?artistobj . ?artistobj nmm:artistName ?artist } GROUP BY ?artist ORDER BY ?artist";
    }
    mSparqlModel = new QSparqlQueryModel(this);
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
}

void AlbumTracksModel::requestAlbumTracks()
{
    qDebug() << "Album tracks requested";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
}

QHash<int, QByteArray> AlbumTracksModel::roleNames() const {
    QHash<int,QByteArray> roles;
    roles[TitleRole] = "title";
    roles[ArtistRole] = "artist";
    roles[AlbumRole] = "album";
    roles[DurationRole] = "length";
    roles[TrackNumberRole] = "tracknr";
    roles[DiscNumberRole] = "discnr";
    roles[DurationFormattedRole] = "lengthformatted";
    return roles;
}

void AlbumTracksModel::sparqlModelfinished()
{
    beginResetModel();
    qDebug() << "underlaying model finished result fetching";
//    emit artistsReady();
    endResetModel();
}

int AlbumTracksModel::rowCount(const QModelIndex &parent) const
{
    return mSparqlModel->rowCount(parent);
}

QVariant AlbumTracksModel::data(const QModelIndex &index, int role) const
{
    switch ( role ) {
    case TitleRole:

        break;
    case ArtistRole:

        break;
    case AlbumRole:

        break;
    case DurationRole:

        break;
    case TrackNumberRole:

        break;
    case DiscNumberRole:

        break;
    case DurationFormattedRole:

        break;
    default:
        break;
    }
    return "";
}

QVariantMap AlbumTracksModel::get(int row){
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
