#include "playlist.h"

Playlist::Playlist(QObject *parent) :
    QAbstractListModel(parent)
{
    mQPlaylist = new QMediaPlaylist(this);
    mTrackList = new QList<TrackObject*>();
    mPlayer = new QMediaPlayer(this);
    mPlayer->setPlaylist(mQPlaylist);
    connect(mQPlaylist,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
    mStatusTimer = 0;
    mStatusInterval = 1000;

    connect(mPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(updateStatus()));
    connect(mPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updateStatus()));
}


void Playlist::addFile(TrackObject *track)
{
    bool retVal = mQPlaylist->addMedia(track->getURL());
    if ( !retVal ) {
        qDebug() << "Couldn't add " << track->getURL();
        return;
    }
    int position = mTrackList->size();
    beginInsertRows(QModelIndex(),position,position);
    mTrackList->append(track);
    endInsertRows();
    qDebug() << "File: " << track->getURL() << " added at " << mTrackList->size()-1;
}

void Playlist::insertAt(TrackObject *track, int pos)
{
    int insPos;
    if ( pos >= mTrackList->size() ) {
        insPos = mTrackList->size();
    }
    bool retVal = mQPlaylist->insertMedia(insPos,track->getURL());
    if ( !retVal ) {
        qDebug() << "Couldn't add " << track->getURL();
        return;
    }
    beginInsertRows(QModelIndex(),insPos,insPos);
    mTrackList->insert(insPos,track);
    endInsertRows();
    qDebug() << "File: " << track->getURL() << " added at " <<insPos;
}

void Playlist::playSong(TrackObject *track)
{
    int pos = rowCount();
    insertAt(track,pos);
    playPosition(pos);
}

void Playlist::removePosition(int position)
{
    if (mQPlaylist->currentIndex() == position) {
        // currently playing try to advance
        next();
    }
    bool retVal = mQPlaylist->removeMedia(position);
    if ( !retVal ) {
        qDebug() << "error removing song:" << position;
        return;
    }
    beginRemoveRows(QModelIndex(),position,position);
    delete(mTrackList->at(position));
    mTrackList->removeAt(position);
    endRemoveRows();

}

void Playlist::playPosition(int position)
{
    int oldpos = mQPlaylist->currentIndex();
    qDebug() << "jumping to position: " << position;
    mQPlaylist->setCurrentIndex(position);
    mPlayer->play();
    indexChanged(mQPlaylist->currentIndex());
    updateStatus();
}

void Playlist::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{

}


QHash<int, QByteArray> Playlist::roleNames() const {
    QHash<int,QByteArray> roles;
    roles[TitleRole] = "title";
    roles[ArtistRole] = "artist";
    roles[AlbumRole] = "album";
    roles[DurationRole] = "length";
    roles[TrackNumberRole] = "tracknr";
    roles[DiscNumberRole] = "discnr";
    roles[FileURLRole] = "fileurl";
    roles[DurationFormattedRole] = "lengthformatted";
    roles[PlayingRole] = "playing";
    roles[SectionRole] = "section";
    return roles;
}


int Playlist::rowCount(const QModelIndex &parent) const
{
    return mTrackList->size();
}

QVariant Playlist::data(const QModelIndex &index, int role) const
{
    switch ( role ) {
    case TitleRole:
        return mTrackList->at(index.row())->getTitle();
    case ArtistRole:
        return mTrackList->at(index.row())->getArtist();
    case AlbumRole:
        return mTrackList->at(index.row())->getAlbum();
    case DurationRole:
        return mTrackList->at(index.row())->getLength();
    case TrackNumberRole:
        return mTrackList->at(index.row())->getTrackNr();
    case DiscNumberRole:
        return mTrackList->at(index.row())->getDiscNr();
    case FileURLRole:
        return mTrackList->at(index.row())->getURL();
    case DurationFormattedRole:
        return mTrackList->at(index.row())->getLengthFormatted();
    case PlayingRole:
        if ( (mPlayer->state() == QMediaPlayer::PlayingState || mPlayer->state() == QMediaPlayer::PlayingState) &&
             mQPlaylist->currentIndex() == index.row() ) {
            return true;
        }
        return false;
    case SectionRole:
        return mTrackList->at(index.row())->getArtist() + '|' + mTrackList->at(index.row())->getAlbum();
    }


    return "";
}

QVariantMap Playlist::get(int row){
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

void Playlist::indexChanged(int position)
{
    qDebug() << "new playing index: " << position << " of "  << rowCount() << " tracks";
    emit dataChanged(index(mOldIndex),index(mOldIndex));
    emit dataChanged(index(position),index(position));
    mOldIndex = position;
}

void Playlist::registerStatusObject(PlaybackStatusObject *obj)
{
    if ( obj == 0 ) {
        return;
    }
    mStatusObject = obj;
    if ( mStatusTimer != 0 ) {
        mStatusTimer->stop();
        mStatusTimer->deleteLater();
        mStatusTimer = 0;
    }
//    mStatusTimer = new QTimer();
//    mStatusTimer->setSingleShot(false);
//    mStatusTimer->setInterval(mStatusInterval);
//    connect(mStatusTimer,SIGNAL(timeout()),this,SLOT(updateStatus()));
//    mStatusTimer->start();
}

void Playlist::updateStatus()
{
    int index = mQPlaylist->currentIndex();
    int playing = mPlayer->state() == QMediaPlayer::PlayingState ? 1 : 0;
    QString title;
    QString artist;
    QString album;
    QUrl url;
    int length = 0;
    int elapsed = mPlayer->position()/1000;;
    int tracknr = 0;
    int discnr = 0;
    int playlistposition = mQPlaylist->currentIndex();
    int playlistlength = mTrackList->size();
    int random = mQPlaylist->playbackMode() == QMediaPlaylist::Random ? 1 : 0;
    int repeat = mQPlaylist->playbackMode() == QMediaPlaylist::Loop ? 1 : 0;

    if (mTrackList->size() > 0 && index >= 0) {
        title = mTrackList->at(index)->getTitle();
        artist = mTrackList->at(index)->getArtist();
        album = mTrackList->at(index)->getAlbum();
        url = mTrackList->at(index)->getURL();
        length = mTrackList->at(index)->getLength();
        tracknr = mTrackList->at(index)->getTrackNr();
        discnr = mTrackList->at(index)->getDiscNr();
    }
    if ( mStatusObject != 0 ) {
        mStatusObject->setInformation(title,artist,album,url.toEncoded(),length,tracknr,discnr,elapsed,playlistposition,playlistlength,playing,random,repeat);
    }
}

void Playlist::next()
{
    if ( mQPlaylist->currentIndex() + 1 < mTrackList->size() ) {
        mQPlaylist->next();
    } else {
        stop();
    }
}

void Playlist::previous()
{
    mQPlaylist->previous();
}

void Playlist::pause()
{
    mPlayer->pause();
    indexChanged(mQPlaylist->currentIndex());
    updateStatus();
}

void Playlist::play()
{
    mPlayer->play();
    indexChanged(mQPlaylist->currentIndex());
    updateStatus();
}

void Playlist::clear()
{
    stop();
    beginResetModel();
    bool retVal = mQPlaylist->clear();
    if ( !retVal ) {
        qDebug() << "Couldn't delete playlist";
        return;
    }
    TrackObject *track;
    foreach (track, *mTrackList) {
        delete(track);
    }
    mTrackList->clear();
    endResetModel();
}

void Playlist::togglePlayPause()
{
    if( mPlayer->state() == QMediaPlayer::PlayingState ) {
        pause();
    } else if ( mPlayer->state() == QMediaPlayer::PausedState || mPlayer->state() == QMediaPlayer::StoppedState ) {
        play();
    }
}

void Playlist::stop()
{
    mPlayer->stop();
    indexChanged(mQPlaylist->currentIndex());
    mQPlaylist->setCurrentIndex(-1);
    updateStatus();
}

void Playlist::seek(int pos)
{
    if ( mPlayer->isSeekable() ) {
        mPlayer->setPosition(pos * 1000);
    }
}

void Playlist::setRandom(bool random)
{
    if ( random ) {
        mQPlaylist->setPlaybackMode(QMediaPlaylist::Random);
    } else {
        mQPlaylist->setPlaybackMode(QMediaPlaylist::Sequential);
    }
    updateStatus();
}

void Playlist::setRepeat(bool repeat)
{
    if ( repeat ) {
        mQPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    } else {
        mQPlaylist->setPlaybackMode(QMediaPlaylist::Sequential);
    }
    updateStatus();
}
