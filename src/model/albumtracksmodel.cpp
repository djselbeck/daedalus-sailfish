#include "albumtracksmodel.h"

#include <QTime>
#include <QUrl>

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
    emit sendBusy(true);
    qDebug() << "Album tracks requested: " + albumurn;
    if ( albumurn != "") {
        albumurn = albumurn.replace('<',"%3C");
        albumurn = albumurn.replace('>',"%3E");
        mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { ?piece nmm:musicAlbum <" + albumurn + "> } ORDER BY ?discnr ?tracknr";
        mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
    } else {
        // List all tracks without album tag
        qDebug() << "Album tracks with no album tag requested: " + albumurn;
        mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { ?piece a nmm:MusicPiece . OPTIONAL { ?piece nmm:musicAlbum ?album } FILTER (!bound(?album) ) } ORDER by ?album ";
        mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
    }
}

void AlbumTracksModel::requestArtistAlbumTracks(QVariant artistalbum)
{
    // Get artist urn
    QStringList strings = artistalbum.toStringList();
    if ( strings.size() != 2 ) {
        return;
    }
    emit sendBusy(true);
    QString artisturn = strings.at(0);
    QString albumurn = strings.at(1);
    qDebug() << "Artist album tracks requested: " << artisturn << albumurn;
    if ( artisturn == "" ) {
        requestAlbumTracks(albumurn);
        return;
    }
    artisturn = artisturn.replace('<',"%3C");
    artisturn = artisturn.replace('>',"%3E");
    albumurn = albumurn.replace('<',"%3C");
    albumurn = albumurn.replace('>',"%3E");
    if ( albumurn != "" ) {
        mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { ?piece nmm:musicAlbum <" + albumurn + "> . ?piece nmm:performer <" + artisturn +"> } ORDER BY ?discnr ?tracknr";
        mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
    } else {
        mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { ?piece nmm:performer <" + artisturn +  "> . OPTIONAL { ?piece nmm:musicAlbum ?album } FILTER (!bound(?album) ) } ORDER by ?album ";
        mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
    }
}

void AlbumTracksModel::requestAlbumTracksReverseFromTrack(QString urn)
{
    emit sendBusy(true);
    urn =  urn.replace('<',"%3C");
    urn =  urn.replace('>',"%3E");
    qDebug() << "Album tracks requested: " + urn;
    mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { <"+ urn +">  nmm:musicAlbum ?album . ?piece nmm:musicAlbum ?album } ORDER BY ?discnr ?tracknr";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
}

void AlbumTracksModel::requestArtistTracks(QString artisturn)
{
    emit sendBusy(true);
    artisturn = artisturn.replace('<',"%3C");
    artisturn = artisturn.replace('>',"%3E");
    qDebug() << "Album tracks requested: " + artisturn;
    mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr  nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { ?piece nmm:performer <" + artisturn + ">  } ORDER BY ?albumname ?discnr ?tracknr";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumTracksQueryString),*mConnection);
}

void AlbumTracksModel::requestAllTracks()
{
    emit sendBusy(true);
    qDebug() << "All tracks requested: ";
    mAlbumTracksQueryString = "SELECT nie:title(?piece) as ?title nmm:artistName(nmm:performer(?piece)) as ?artistname nmm:albumTitle(nmm:musicAlbum(?piece)) as ?albumname nfo:duration(?piece) as ?length nmm:trackNumber(?piece) as ?tracknr nmm:setNumber(nmm:musicAlbumDisc(?piece)) as ?discnr nie:url(?piece) as ?fileurl ?piece WHERE { ?piece a nmm:MusicPiece }  ?artistname ?albumname ?discnr ?tracknr";
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
    emit sendBusy(false);
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
    } else if ( role == TitleRole ) {
        QString title = mSparqlModel->data(index,role).toString();
        if ( title == "" ) {
            title = mSparqlModel->data(index,FileURLRole).toUrl().fileName();
        }
        return title;
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

void AlbumTracksModel::clearData()
{
    beginResetModel();
    mSparqlModel->clear();
    endResetModel();
}
