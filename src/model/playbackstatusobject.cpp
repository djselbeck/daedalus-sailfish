#include "playbackstatusobject.h"

#include <QDebug>

PlaybackStatusObject::PlaybackStatusObject(QString title, QString artist, QString album, QString url, QUrl urn, int length, int tracknr, int discnr, int elapsed, int playlistposition, int playlistlength, QObject *parent) :
    QObject(parent)
{
    mTitle = title;
    mArtist = artist;
    mAlbum = album;
    mURL = url;
    mURN = urn;
    mLength = length;
    mTrackNr = tracknr;
    mDiscNr = discnr;
    mElapsed = elapsed;
    mPlaylistPosition = playlistposition;
    mPlaylistLength = playlistlength;
    mPlaying = 0;
    mRepeat = 0;
    mRandom = 0;
}


QString PlaybackStatusObject::getTitle(){
    return mTitle;
}

QString PlaybackStatusObject::getArtist()
{
    return mArtist;
}

QString PlaybackStatusObject::getAlbum()
{
    return mAlbum;
}

QUrl PlaybackStatusObject::getURL()
{
    return QUrl(mURL);
}

QUrl PlaybackStatusObject::getURN()
{
    return mURN;
}

int PlaybackStatusObject::getLength()
{
    return mLength;
}

int PlaybackStatusObject::getTrackNr()
{
    return mTrackNr;
}

int PlaybackStatusObject::getDiscNr()
{
    return mDiscNr;
}

int PlaybackStatusObject::getElapsed()
{
    return mElapsed;
}

int PlaybackStatusObject::getPlaylistPosition()
{
    return mPlaylistPosition;
}

int PlaybackStatusObject::getPlaylistLength()
{
    return mPlaylistLength;
}

int PlaybackStatusObject::getPlaying()
{
    return mPlaying;
}

int PlaybackStatusObject::getRepeat()
{
    return mRepeat;
}

int PlaybackStatusObject::getRandom()
{
    return mRandom;
}

QString PlaybackStatusObject::getLengthFormatted()
{
    QString temp;
    int hours=0,min=0,sec=0;
    hours = mLength/3600;
    if(hours>0)
    {
        min=(mLength-(3600*hours))/60;
    }
    else{
        min=mLength/60;
    }
    sec = mLength-hours*3600-min*60;
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

void PlaybackStatusObject::setInformation(QString title, QString artist, QString album, QString url, QUrl urn, int length, int tracknr, int discnr, int elapsed, int playlistposition, int playlistlength, int playing, int random, int repeat)
{
    if ( title != mTitle) {
        mTitle = title;
        emit titleChanged();
    }
    if ( artist != mArtist) {
        mArtist = artist;
        emit artistChanged();
    }
    if ( album != mAlbum ) {
        mAlbum = album;
        emit albumChanged();
    }
    mURL = url;
    mURN = urn;
    mLength = length;
    mTrackNr = tracknr;
    mDiscNr = discnr;
    mElapsed = elapsed;
    mPlaylistPosition = playlistposition;
    mPlaylistLength = playlistlength;
    mPlaying = playing;
    mRandom = random;
    mRepeat = repeat;
    // Notify listeners
    emit valuesChanged();

}
