#ifndef TRACKOBJECT_H
#define TRACKOBJECT_H

#include <QObject>
#include <QUrl>

class TrackObject : public QObject
{
    Q_OBJECT
public:
    explicit TrackObject(QString title, QString artist, QString album, QString url, QUrl urn, int length, int tracknr, int discnr, QObject *parent = 0);
    TrackObject(TrackObject &object);
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QUrl getURL();
    QUrl getURN();
    int getLength();
    int getTrackNr();
    int getDiscNr();
    QString getLengthFormatted();

signals:

public slots:

private:
    QString mTitle;
    QString mArtist;
    QString mAlbum;
    QUrl mURL;
    QUrl mURN;
    int mLength;
    int mTrackNr;
    int mDiscNr;

};

#endif // TRACKOBJECT_H
