#include "trackobject.h"

TrackObject::TrackObject(QString title, QString artist, QString album, QString url, int length, int tracknr, int discnr, QObject *parent) :
    QObject(parent)
{
    mTitle = title;
    mArtist = artist;
    mAlbum = album;
    mURL = url;
    mLength = length;
    mTrackNr = tracknr;
    mDiscNr = discnr;
}

QString TrackObject::getTitle(){
    return mTitle;
}

QString TrackObject::getArtist()
{
    return mArtist;
}

QString TrackObject::getAlbum()
{
    return mAlbum;
}

QUrl TrackObject::getURL()
{
    return QUrl(mURL);
}

int TrackObject::getLength()
{
    return mLength;
}

int TrackObject::getTrackNr()
{
    return mTrackNr;
}

int TrackObject::getDiscNr()
{
    return mDiscNr;
}

QString TrackObject::getLengthFormatted()
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

