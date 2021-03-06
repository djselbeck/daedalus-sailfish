#ifndef PLAYBACKSTATUSOBJECT_H
#define PLAYBACKSTATUSOBJECT_H

#include <QObject>
#include <QUrl>

class PlaybackStatusObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ getArtist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ getAlbum NOTIFY albumChanged)
    Q_PROPERTY(QUrl trackurn READ getURN NOTIFY valuesChanged)
    Q_PROPERTY(QUrl url READ getURL NOTIFY valuesChanged)
    Q_PROPERTY(int length READ getLength NOTIFY valuesChanged)
    Q_PROPERTY(int elapsed READ getElapsed NOTIFY valuesChanged)
    Q_PROPERTY(int tracknr READ getTrackNr NOTIFY valuesChanged)
    Q_PROPERTY(int discnr READ getDiscNr NOTIFY valuesChanged)
    Q_PROPERTY(int playlistposition READ getPlaylistPosition NOTIFY valuesChanged)
    Q_PROPERTY(int playlistlength READ getPlaylistLength NOTIFY valuesChanged)
    Q_PROPERTY(QString lengthformatted READ getLengthFormatted NOTIFY valuesChanged)
    Q_PROPERTY(int playing READ getPlaying NOTIFY valuesChanged)
    Q_PROPERTY(int repeat READ getRepeat NOTIFY valuesChanged)
    Q_PROPERTY(int random READ getRandom NOTIFY valuesChanged)


public:
    explicit PlaybackStatusObject(QString title = "", QString artist = "", QString album = "", QString url = "", QUrl urn = QUrl(), int length = 0, int tracknr = 0, int discnr = 0, int elapsed = 0, int playlistposition = 0, int playlistlength = 0, QObject *parent = 0);

    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QUrl getURL();
    QUrl getURN();
    int getLength();
    int getTrackNr();
    int getElapsed();
    int getDiscNr();
    int getPlaylistPosition();
    int getPlaylistLength();
    int getPlaying();
    int getRepeat();
    int getRandom();
    QString getLengthFormatted();

    void setInformation(QString title, QString artist, QString album, QString url, QUrl urn, int length, int tracknr, int discnr, int elapsed, int playlistposition, int playlistlength, int playing, int random, int repeat);
signals:
    void valuesChanged();
    void artistChanged();
    void albumChanged();
    void titleChanged();
    void playingChanged();
    void positionChanged();
    void repeatChanged();
    void randomChanged();

public slots:

private:
    QString mTitle;
    QString mArtist;
    QString mAlbum;
    QString mURL;
    QUrl mURN;

    int mLength;
    int mElapsed;
    int mTrackNr;
    int mDiscNr;
    int mPlaylistPosition;
    int mPlaylistLength;
    int mPlaying;
    int mRandom;
    int mRepeat;


};

#endif // PLAYBACKSTATUSOBJECT_H
