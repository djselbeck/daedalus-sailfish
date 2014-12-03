#include "albumtracksmodel.h"

#include <QTime>

AlbumTracksModel::AlbumTracksModel(QObject *parent, QSparqlConnection *connection,QThread *fetchthread) :
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

void AlbumTracksModel::requestAlbumTracks(QString albumurn)
{
    albumurn = albumurn.replace('\'',"\\\'");
    qDebug() << "Album tracks requested: " + albumurn;
    mAlbumTracksQueryString = "SELECT ?title ?artistname ?albumname ?length ?tracknr ?discnr ?fileurl ?piece WHERE { ?piece nmm:musicAlbum '" + albumurn + "' ; nie:url ?fileurl ; nie:title ?title ; nfo:duration ?length ; nmm:trackNumber ?tracknr ; nmm:musicAlbumDisc ?disc ; nmm:performer ?artist ; nmm:musicAlbum ?album . ?album nmm:albumTitle ?albumname . ?artist nmm:artistName ?artistname . ?disc nmm:setNumber ?discnr } ORDER BY ?discnr ?tracknr";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
}

void AlbumTracksModel::requestAlbumTracksReverseFromTrack(QString urn)
{
    urn = urn.replace('\'',"\\\'");
    urn = urn.replace('<','\<');
    urn = urn.replace('>','\>');
    qDebug() << "Album tracks requested: " + urn;
    mAlbumTracksQueryString = "SELECT ?title ?artistname ?albumname ?length ?tracknr ?discnr ?fileurl ?piece WHERE { <"+ urn +">  nmm:musicAlbum ?album . ?piece nmm:musicAlbum ?album ; nie:url ?fileurl ; nie:title ?title ; nfo:duration ?length ; nmm:trackNumber ?tracknr ; nmm:musicAlbumDisc ?disc ; nmm:performer ?artist ; nmm:musicAlbum ?album . ?album nmm:albumTitle ?albumname . ?artist nmm:artistName ?artistname . ?disc nmm:setNumber ?discnr } ORDER BY ?discnr ?tracknr";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
}

void AlbumTracksModel::requestArtistTracks(QString artisturn)
{
    artisturn = artisturn.replace('\'',"\\\'");
    artisturn = artisturn.replace('<','\<');
    artisturn = artisturn.replace('>','\>');
    qDebug() << "Album tracks requested: " + artisturn;
    mAlbumTracksQueryString = "SELECT ?title ?artistname ?albumname ?length ?tracknr ?discnr ?fileurl ?piece WHERE { ?album nmm:albumArtist '"+artisturn+"' . ?piece nmm:musicAlbum ?album ; nie:url ?fileurl ; nie:title ?title ; nfo:duration ?length ; nmm:trackNumber ?tracknr ; nmm:musicAlbumDisc ?disc ; nmm:performer ?artist ; nmm:musicAlbum ?album . ?album nmm:albumTitle ?albumname . ?artist nmm:artistName ?artistname . ?disc nmm:setNumber ?discnr } ORDER BY ?albumname ?discnr ?tracknr";
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
    roles[FileURLRole] = "fileurl";
    roles[TrackURNRole] = "trackurn";
    roles[DurationFormattedRole] = "lengthformatted";
    return roles;
}

void AlbumTracksModel::sparqlModelfinished()
{
    beginResetModel();
    qDebug() << "underlaying model finished result fetching";
    emit modelReady();
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
        return mSparqlModel->data(index,role);
        break;
    case ArtistRole:
        return mSparqlModel->data(index,role);
        break;
    case AlbumRole:
        return mSparqlModel->data(index,role);
        break;
    case DurationRole:
        return mSparqlModel->data(index,role);
        break;
    case TrackNumberRole:
        return mSparqlModel->data(index,role);
        break;
    case DiscNumberRole:
        return mSparqlModel->data(index,role);
        break;
    case FileURLRole:
        return mSparqlModel->data(index,role);
        break;
    case TrackURNRole:
        return mSparqlModel->data(index,role);
    default:
        break;
    }
    if ( role == DurationFormattedRole ) {
        int length = mSparqlModel->data(index,DurationRole).toInt();
        return getLengthFormatted(length);
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

inline QString AlbumTracksModel::getLengthFormatted(int length) const
{
    QString temp;
    int hours=0,min=0,sec=0;
    hours = length/3600;
    if(hours>0)
    {
        min=(length-(3600*hours))/60;
    }
    else{
        min=length/60;
    }
    sec = length-hours*3600-min*60;
    if(hours==0)
    {
        temp=(min<10?"0":"")+QString::number(min)+":"+(sec<10?"0":"")+QString::number(sec);
    }
    else
    {
        temp=(hours<10?"0":"")+QString::number(hours)+":"+(min<10?"0":"")+QString::number(min)+":"+(sec<10?"0":"")+QString::number(sec);
    }
    return temp;
}

