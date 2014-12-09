#include "savedplaylisttracksmodel.h"

SavedPlaylistTracksModel::SavedPlaylistTracksModel(QObject *parent, QList<QUrl> *urls, QSparqlConnection *connection, QThread *fetchthread) :
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
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(trackReady()));

    mURLs = urls;
    if ( mURLs != 0 && mURLs->size() > 0) {
        // Start adding songs with sparql
        QUrl url = mURLs->at(0);
        mURLs->removeAt(0);
        mCurrentFile = url.toEncoded();
        requestTrack(url);
    }
    mTracks = new QList<TrackObject*>();
}


void SavedPlaylistTracksModel::requestTrack(QUrl track)
{
    QString urlString = track.toEncoded();
    urlString = urlString.replace('<',"%3C");
    urlString = urlString.replace('>',"%3E");
    qDebug() << "search for: " << urlString << " in sparql";
    QString requestString = "SELECT ?title ?artistname ?albumname ?length ?tracknr ?discnr ?fileurl ?piece WHERE { ?piece nie:url <"
            + urlString + "> ; nie:url ?fileurl ; nie:title ?title ; nfo:duration ?length ; nmm:trackNumber ?tracknr ; nmm:musicAlbumDisc ?disc ; nmm:performer ?artist ; nmm:musicAlbum ?album . ?album nmm:albumTitle ?albumname . ?artist nmm:artistName ?artistname . ?disc nmm:setNumber ?discnr }";
    mSparqlModel->setQuery(QSparqlQuery(requestString),*mConnection);
}

void SavedPlaylistTracksModel::trackReady()
{
    // Parse sparql result
    qDebug() << "trackReady, rowCount: " << mSparqlModel->rowCount();
    QString title;
    QString artist;
    QString album;
    int length;
    int tracknr;
    int discnr;
    QString fileurl;
    QString trackurn;
    if ( mSparqlModel->rowCount() > 0 ) {
        title = mSparqlModel->data(mSparqlModel->index(0,0),0).toString();
        artist = mSparqlModel->data(mSparqlModel->index(0,1),0).toString();
        album = mSparqlModel->data(mSparqlModel->index(0,2),0).toString();
        length = mSparqlModel->data(mSparqlModel->index(0,3),0).toInt();
        tracknr = mSparqlModel->data(mSparqlModel->index(0,4),0).toInt();
        discnr = mSparqlModel->data(mSparqlModel->index(0,5),0).toInt();
        fileurl = mSparqlModel->data(mSparqlModel->index(0,6),0).toString();
        trackurn = mSparqlModel->data(mSparqlModel->index(0,7),0).toString();
        qDebug() << "Found title: " << title << artist << album << length << tracknr << discnr << fileurl << trackurn;
    } else {
        title = tr("Undefined");
        artist = tr("Undefined");
        album = tr("Undefined");
        length = 0;
        tracknr = 0;
        discnr = 0;
        fileurl = mCurrentFile;
        trackurn = "null";
    }

    TrackObject* tempTrack = new TrackObject(title,artist,album,fileurl,trackurn,length,tracknr,discnr,0);

//    beginInsertRows(QModelIndex(),mTracks->size(),mTracks->size());
    mTracks->append(tempTrack);
//    endInsertRows();

    // continue
    if ( mURLs != 0 && mURLs->size() > 0) {
        // Start adding songs with sparql
        QUrl url = mURLs->at(0);
        mURLs->removeAt(0);
        mCurrentFile = url.toEncoded();
        requestTrack(url);
    } else {
        // All tracks ready
        beginResetModel();
        endResetModel();
    }
}

QHash<int, QByteArray> SavedPlaylistTracksModel::roleNames() const {
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
    roles[SectionRole] = "section";
    return roles;
}

int SavedPlaylistTracksModel::rowCount(const QModelIndex &parent) const
{
    return mTracks->size();
}

QVariant SavedPlaylistTracksModel::data(const QModelIndex &index, int role) const
{
    switch ( role ) {
    case TitleRole:
        return mTracks->at(index.row())->getTitle();
    case ArtistRole:
        return mTracks->at(index.row())->getArtist();
    case AlbumRole:
        return mTracks->at(index.row())->getAlbum();
    case DurationRole:
        return mTracks->at(index.row())->getLength();
    case TrackNumberRole:
        return mTracks->at(index.row())->getTrackNr();
    case DiscNumberRole:
        return mTracks->at(index.row())->getDiscNr();
    case FileURLRole:
        return mTracks->at(index.row())->getURL();
    case TrackURNRole:
        return mTracks->at(index.row())->getURN();
    case DurationFormattedRole:
        return mTracks->at(index.row())->getLengthFormatted();
    }
    if (role == SectionRole) {
        QString album = mTracks->at(index.row())->getAlbum();
        QString artist = mTracks->at(index.row())->getArtist();

        QString sectionString;
        sectionString = artist + '|' + album;
        return sectionString;
    }

}


/**
 * This returns an pseudo item in an variant map for SectionScroller
 * @brief AlbumsModel::get
 * @param row
 * @return
 */
QVariantMap SavedPlaylistTracksModel::get(int row){
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

TrackObject* SavedPlaylistTracksModel::getTrack(int position)
{
    if ( position < mTracks->size()) {
        return mTracks->at(position);
    }
}
