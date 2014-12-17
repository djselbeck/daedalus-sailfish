#ifndef DAEDALUSDBUSADAPTOR_H
#define DAEDALUSDBUSADAPTOR_H

#include <QDBusAbstractAdaptor>


#include "../mediaplayer/playlist.h"
#include "../model/playbackstatusobject.h"

class DaedalusDBUSAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.mpris.MediaPlayer2.Player")
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
    Q_PROPERTY(QString Loop_Status READ loopStatus)
    Q_PROPERTY(double Rate READ rate)
    Q_PROPERTY(bool Shuffle READ shuffle)
    Q_PROPERTY(QVariantMap Metadata READ metaData)
    Q_PROPERTY(double Volume READ volume)
    Q_PROPERTY(qint64 Position READ position)
    Q_PROPERTY(double MinimumRate READ minimumRate)
    Q_PROPERTY(double MaximumRate READ maximumRate)
    Q_PROPERTY(bool CanPlay READ canPlay)
    Q_PROPERTY(bool CanPause READ canPause)
    Q_PROPERTY(bool CanSeek READ canSeek)
    Q_PROPERTY(bool CanControl READ canControl)
    Q_PROPERTY(bool CanGoNext READ canGoNext)
    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
public:
    DaedalusDBUSAdaptor(Playlist *playlist, PlaybackStatusObject *playbackState, QObject *parent = 0);

public slots:
    // All directly callable from dbus
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qint64 pos);
    void SetPosition(QString trackid, qint64 position);
    void OpenUri(QString url);

signals:
    void Seeked(int position);

private:
    QString playbackStatus();
    QString loopStatus();
    double  rate();
    bool shuffle();
    QVariantMap metaData();
    double volume();
    qint64 position();
    double minimumRate();
    double maximumRate();
    bool canPlay();
    bool canPause();
    bool canSeek();
    bool canControl();
    bool canGoNext();
    bool canGoPrevious();

    Playlist *mPlaylist;
    PlaybackStatusObject *mPlaybackStatus;

private slots:
    void playbackStateChange();
    void titleChanged();
    void randomChanged();
    void repeatChanged();
    void positionChanged();

};

#endif // DAEDALUSDBUSADAPTOR_H
