#include "playlist.h"

Playlist::Playlist(QObject *parent) :
    QAbstractListModel(parent)
{
    mQPlaylist = new QMediaPlaylist(this);
    mTrackList = new QList<TrackObject*>();
    mPlayer = new QMediaPlayer(this);
    mPlayer->setPlaylist(mQPlaylist);
    connect(mQPlaylist,SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
}


void Playlist::addFile(TrackObject *track)
{
    bool retVal = mQPlaylist->addMedia(track->getURL());
    if ( !retVal ) {
        qDebug() << "Couldn't add " << track->getURL();
        return;
    }
    mTrackList->append(track);
    qDebug() << "File: " << track->getURL() << " added";
}

void Playlist::removePosition(int position)
{

}

void Playlist::playPosition(int position)
{
    int oldpos = mQPlaylist->currentIndex();
    qDebug() << "jumping to position: " << position;
    mQPlaylist->setCurrentIndex(position);
    mPlayer->play();
    emit dataChanged(index(oldpos),index(oldpos));
    emit dataChanged(index(position),index(position));
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
        return mQPlaylist->currentIndex() == index.row() ? true : false;
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
