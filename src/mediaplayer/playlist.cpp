#include "playlist.h"
#include "../model/plswriter.h"

#include <QFile>
#include <QTextStream>
#include <QTime>

#include "../model/playbackstate.h"

Playlist::Playlist(QObject *parent) :
    QAbstractListModel(parent)
{
    mTrackList = new QList<TrackObject*>();
    mPlayer = new QMediaPlayer(this);

    connect(mPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(updateState()));
    connect(mPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updateStatus()));

    mBackgroundThread = new QThread(this);
    mBackgroundThread->start();

    mPlaybackState = new PlaybackState(0);
    mPlaybackState->moveToThread(mBackgroundThread);

    connect(this,SIGNAL(requestSavedPlaylistState()),mPlaybackState,SLOT(resumeLastPlaylist()));
    connect(this,SIGNAL(requestPlaylistStateSave(QList<TrackObject*>*)),mPlaybackState,SLOT(saveCurrentPlaylist(QList<TrackObject*>*)));
    connect(mPlaybackState,SIGNAL(lastPlaylistReady(QList<TrackObject*>*)),this,SLOT(receiveSavedPlaybackStateList(QList<TrackObject*>*)));
    connect(mPlaybackState,SIGNAL(workDone()),mBackgroundThread,SLOT(terminate()));

    mNextIndex = 0;
    mCurrentIndex = 0;
    mRandom = false;
    mRepeat = false;

    qsrand(QTime::currentTime().msec());

    emit sendBusy(false);
}

Playlist::~Playlist()
{
    stop();
    qDebug() << "Waiting for background thread";
    mBackgroundThread->wait(3 * 60 * 1000);
    qDebug() << "Waiting for background thread done";
    delete(mPlayer);
    qDeleteAll(*mTrackList);
    delete(mTrackList);
}


void Playlist::addFile(TrackObject *track)
{
    int position = mTrackList->size();
    beginInsertRows(QModelIndex(),position,position);
    mTrackList->append(track);
    endInsertRows();
    qDebug() << "File: " << track->getURL() << " added at " << mTrackList->size()-1;
    setNextTrack();
}

void Playlist::insertAt(TrackObject *track, int pos)
{
    int insPos = pos;
    if ( pos >= mTrackList->size() ) {
        insPos = mTrackList->size();
    }
    beginInsertRows(QModelIndex(),insPos,insPos);
    mTrackList->insert(insPos,track);
    endInsertRows();
    qDebug() << "File: " << track->getURL() << " added at " <<insPos;

    if ( pos < mCurrentIndex ) {
        mCurrentIndex++;
        indexChanged(mCurrentIndex);
    }
    setNextTrack();
}

void Playlist::playSong(TrackObject *track)
{
    unsigned int pos = rowCount();
    insertAt(track,pos);
    playPosition(pos);
}

void Playlist::removePosition(int position)
{
    if (mCurrentIndex == position) {
        // currently playing try to advance
        next();
    }
    beginRemoveRows(QModelIndex(),position,position);
    delete(mTrackList->at(position));
    mTrackList->removeAt(position);
    endRemoveRows();

    if ( position < mCurrentIndex ) {
        mCurrentIndex--;
        indexChanged(mCurrentIndex);
    }
    setNextTrack();
}

void Playlist::playPosition(int position)
{
    mNextIndex = -1;
    mHaveNextTrack = false;
    qDebug() << "player state: " << mPlayer->state();
    qDebug() << "jumping to position: " << position;
    mPlayer->stop();
    mCurrentIndex = position;
    qDebug() << "Setting player media to: " << mTrackList->at(position)->getURL().toLocalFile();
    mPlayer->setMedia(mTrackList->at(position)->getURL());
    qDebug() << "player set";
    mPlayer->play();
    qDebug() << "play called";
    indexChanged(mCurrentIndex);
//    updateStatus();

    setNextTrack();
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
    roles[TrackURNRole] = "trackurn";
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
    case TrackURNRole:
        return mTrackList->at(index.row())->getURN();
    case DurationFormattedRole:
        return mTrackList->at(index.row())->getLengthFormatted();
    case PlayingRole:
        if ( (mPlayer->state() == QMediaPlayer::PlayingState || mPlayer->state() == QMediaPlayer::PlayingState) &&
             mCurrentIndex == index.row() ) {
            return true;
        }
        return false;
       }
    if (role == SectionRole) {
        QString album = mTrackList->at(index.row())->getAlbum();
        QString artist = mTrackList->at(index.row())->getArtist();

        QString sectionString;
        sectionString = artist + '|' + album;
        return sectionString;
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
}

void Playlist::updateState()
{
    qDebug() << "Player state changed:" << mPlayer->state();
    if ( (mPlayer->state() == QMediaPlayer::StoppedState) && mHaveNextTrack ) {
        playPosition(mNextIndex);
    } else if( (mPlayer->state() == QMediaPlayer::StoppedState) && !mHaveNextTrack)  {
        stop();
    }
    updateStatus();
}

void Playlist::updateStatus()
{
    qDebug() << "new status";
    int index = mCurrentIndex;
    int playing = mPlayer->state();
    qDebug() << "playing: " << playing;
    QString title;
    QString artist;
    QString album;
    QUrl url;
    QUrl urn;
    int length = 0;
    int elapsed = mPlayer->position()/1000;;
    int tracknr = 0;
    int discnr = 0;
    int playlistposition = mCurrentIndex;
    int playlistlength = mTrackList->size();
    int random = mRandom;
    int repeat = mRepeat;

    if (mTrackList->size() > 0 && index >= 0) {
        title = mTrackList->at(index)->getTitle();
        artist = mTrackList->at(index)->getArtist();
        album = mTrackList->at(index)->getAlbum();
        url = mTrackList->at(index)->getURL();
        urn = mTrackList->at(index)->getURN();
        length = mTrackList->at(index)->getLength();
        tracknr = mTrackList->at(index)->getTrackNr();
        discnr = mTrackList->at(index)->getDiscNr();
    }
    if ( mStatusObject != 0 ) {
        mStatusObject->setInformation(title,artist,album,url.toEncoded(),urn,length,tracknr,discnr,elapsed,playlistposition,playlistlength,playing,random,repeat);
    }
}

void Playlist::next()
{
    if ( mHaveNextTrack ) {
        playPosition(mNextIndex);
    } else {
        stop();
    }
}

void Playlist::previous()
{
//    mQPlaylist->previous();
    if ( mCurrentIndex > 0 ) {
        playPosition(mCurrentIndex -1);
    } else {
        stop();
    }
}

void Playlist::pause()
{
    mPlayer->pause();
    indexChanged(mCurrentIndex);
    updateStatus();
}

void Playlist::play()
{
    mPlayer->play();
    indexChanged(mCurrentIndex);
    updateStatus();
}

void Playlist::clear()
{
    stop();
    beginResetModel();
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
    } else if ( mPlayer->state() == QMediaPlayer::PausedState) {
        play();
    } else {
        if ( mTrackList->size() > 0 ) {
            playPosition(0);
        }
    }
}

void Playlist::stop()
{
    qDebug() << "stopping playback";
    mHaveNextTrack = false;
    mPlayer->stop();
    mPlayer->setPosition(0);
    mPlayer->media();
    mCurrentIndex = 0;
    mNextIndex = -1;
    indexChanged(mCurrentIndex);
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
    mRepeat = false;
    mRandom = random;
    setNextTrack();
    updateStatus();
}

void Playlist::setRepeat(bool repeat)
{
    mRepeat = repeat;
    mRandom = 0;
    setNextTrack();
    updateStatus();
}

void Playlist::moveTrack(int from, int to)
{
    if ( mCurrentIndex == from || from >= rowCount()) {
        // Abort here
        return;
    }

    if ( from < to ) {
        to--;
    }

    beginRemoveRows(QModelIndex(),from,from);
    TrackObject *tempTrack = mTrackList->at(from);
    mTrackList->removeAt(from);
    endRemoveRows();
    beginInsertRows(QModelIndex(),to,to);
    mTrackList->insert(to,tempTrack);
    endInsertRows();

    // Correct indices
    if ( from < mCurrentIndex ) {
        mCurrentIndex--;
        setNextTrack();
        indexChanged(mCurrentIndex);
    }
}

void Playlist::playNext(int position)
{
    moveTrack(position,mCurrentIndex+1);
    // create a copy of the track
//    if ( position >= mTrackList->size() || position < 0 ) {
//        return;
//    }
//    TrackObject *copy = new TrackObject(*(mTrackList->at(position)));
//    insertAt(copy,mCurrentIndex + 1);
}

int Playlist::currentIndex()
{
    return mCurrentIndex;
}

void Playlist::savePlaylist(QString name)
{
    name = name.replace(".pls","");
    QString outputPath = Playlist::getXDGMusicDir() + '/' + name + ".pls";
    QUrl destination = QUrl::fromLocalFile(outputPath);
    PLSWriter::writePlaylist(mTrackList,destination,name);
}

QString Playlist::getXDGMusicDir()
{
    QString xdgConfigHome = qgetenv("XDG_CONFIG_HOME");
    QString homePath = qgetenv("HOME");
    QString configPath = "";
    if ( xdgConfigHome != "") {
        configPath = xdgConfigHome + "/.config/user-dirs.dirs";
    } else {
        configPath = homePath + "/.config/user-dirs.dirs";
    }
    qDebug() << "Try getting xdg dirs at: " << configPath;
    QFile xdgConfig(configPath);
    if ( !xdgConfig.open(QIODevice::ReadOnly) ) {
        return "";
    }
    QTextStream inputStream(&xdgConfig);
    while ( !inputStream.atEnd() ) {
        QString textLine = inputStream.readLine();
        if ( textLine.startsWith("XDG_MUSIC_DIR") ) {
            QStringList splitList = textLine.split('=');
            QString envString = splitList[1];
            envString = envString.replace('\"',"");
            envString = envString.replace("$HOME",homePath);
            qDebug() << "Music env:" << envString;
            return envString;
        }
    }
    return "";
}

void Playlist::savePlaylistToSql()
{
    emit requestPlaylistStateSave(mTrackList);
}

void Playlist::receiveSavedPlaybackStateList(QList<TrackObject *> *list)
{
    if ( list != 0 ) {
        if ( mTrackList != 0 ) {
            qDeleteAll(*mTrackList);
            delete(mTrackList);
        }

        beginResetModel();
        mTrackList = list;
        endResetModel();
        // FIXME disabled for now
//        qDebug() << "Try resuming at index: " << mResumeIndex << " at position: " << mResumeTime;
//        if ( mTrackList->size() > mResumeIndex ) {
//            mQPlaylist->setCurrentIndex(mResumeIndex);
//            qDebug() << "Index done";
//        }
//        mPlayer->play();
//        while ( mPlayer->state() != QMediaPlayer::PlayingState ) {

//        }
//        if ( mPlayer->isSeekable()) {
//            mPlayer->setPosition(mResumeTime * 1000);
//            mPlayer->pause();
//            qDebug() << "Seek done";
//        }
//        mPlayer->pause();
    }
    emit sendBusy(false);
}

void Playlist::resumePlaylist()
{
    emit sendBusy(true);
    emit requestSavedPlaylistState();
}


void Playlist::setResumeIndex(unsigned int pos)
{
    mResumeIndex = pos;
}

void Playlist::setResumeTime(unsigned int pos)
{
    mResumeTime = pos;
}

void Playlist::addUrl(QString url)
{
    TrackObject *dummyTrack = new TrackObject(url,"","",url,QUrl(url),0,0,0,0);
    addFile(dummyTrack);
}

unsigned int Playlist::getRandomIndex()
{
    return qrand() % (mTrackList->size() -1);
}

void Playlist::setNextTrack()
{
    // Determine next playing index
    if ( !mRandom && !mRepeat ) {
        if ( mCurrentIndex + 1 < mTrackList->size() ) {
            mNextIndex = mCurrentIndex + 1;
            mHaveNextTrack = true;
        } else
        {
            mHaveNextTrack = false;
        }
    } else if ( mRandom ) {
        mNextIndex = getRandomIndex();
        mHaveNextTrack = true;
    } else if ( mRepeat ) {
        if ( mCurrentIndex + 1 < mTrackList->size()) {
            mNextIndex = mCurrentIndex + 1;
            mHaveNextTrack = true;
        } else {
            mNextIndex = 0;
            mHaveNextTrack = true;
        }
    }
    qDebug() << "next index is: " << mNextIndex;
}

void Playlist::shufflePlaylist()
{
    beginResetModel();
    QList<TrackObject*> *newTracks = new QList<TrackObject*>();
    while ( !mTrackList->empty() ) {
        int trackPos = qrand() % mTrackList->size();
        newTracks->append(mTrackList->at(trackPos));
        mTrackList->removeAt(trackPos);
        qDebug() << "Removed and appended track at: " << trackPos;
    }
    delete(mTrackList);
    mTrackList = newTracks;
    endResetModel();
    qDebug() << "finished shuffling the playlist";
}
