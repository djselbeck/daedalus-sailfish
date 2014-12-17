#include "daedalusdbusadaptor.h"

#include <QMediaPlayer>
#include <QDBusConnection>
#include <QDBusMessage>

DaedalusDBUSAdaptor::DaedalusDBUSAdaptor(Playlist *playlist, PlaybackStatusObject *playbackState, QObject *parent) : QDBusAbstractAdaptor(parent)
{
    mPlaylist = playlist;
    mPlaybackStatus = playbackState;
    if ( mPlaybackStatus != 0 ) {
        connect(mPlaybackStatus,SIGNAL(titleChanged()),this,SLOT(titleChanged()));
        connect(mPlaybackStatus,SIGNAL(randomChanged()),this,SLOT(randomChanged()));
        connect(mPlaybackStatus,SIGNAL(repeatChanged()),this,SLOT(repeatChanged()));
        connect(mPlaybackStatus,SIGNAL(positionChanged()),this,SLOT(positionChanged()));
        connect(mPlaybackStatus,SIGNAL(playingChanged()),this,SLOT(playbackStateChange()));
    }
}

void DaedalusDBUSAdaptor::Next() {
    if ( mPlaybackStatus->getPlaying() ) {
        mPlaylist->next();
    }
}

void DaedalusDBUSAdaptor::Previous() {
    if ( mPlaybackStatus->getPlaying() ) {
        mPlaylist->previous();
    }
}
void DaedalusDBUSAdaptor::Pause() {
    mPlaylist->pause();
}
void DaedalusDBUSAdaptor::PlayPause() {
    mPlaylist->togglePlayPause();
}
void DaedalusDBUSAdaptor::Stop() {
    mPlaylist->stop();
}
void DaedalusDBUSAdaptor::Play() {
    mPlaylist->play();
}
void DaedalusDBUSAdaptor::Seek(qint64 pos) {
    mPlaylist->seek(pos/(1000*1000));
}

void DaedalusDBUSAdaptor::SetPosition(QString trackid, qint64 position)
{
    Q_UNUSED(trackid);
    Q_UNUSED(position);
}

void DaedalusDBUSAdaptor::OpenUri(QString url)
{
    Q_UNUSED(url);
}

QString DaedalusDBUSAdaptor::playbackStatus() {
    switch(mPlaybackStatus->getPlaying()) {
    case QMediaPlayer::PlayingState:
        return "Playing";
    case QMediaPlayer::PausedState:
        return "Paused";
    case QMediaPlayer::StoppedState:
        return "Stopped";
    }
}
QString DaedalusDBUSAdaptor::loopStatus() {
    if ( mPlaybackStatus->getRepeat() ) {
        return "Playlist";
    } else {
        return "None";
    }
}
double  DaedalusDBUSAdaptor::rate() {
    return 1.0;
}
bool DaedalusDBUSAdaptor::shuffle() {
    return mPlaybackStatus->getRandom();
}
QVariantMap DaedalusDBUSAdaptor::metaData() {
    QVariantMap metaData;
    metaData.insert("mpris:trackid",mPlaybackStatus->getPlaylistPosition());
    metaData.insert("mpris:length",mPlaybackStatus->getLength() * 1000 * 1000);
    metaData.insert("xesam:album",mPlaybackStatus->getAlbum());
    metaData.insert("xesam:artist",mPlaybackStatus->getArtist());
    metaData.insert("xesam:discNumber",mPlaybackStatus->getDiscNr());
    metaData.insert("xesam:title", mPlaybackStatus->getTitle());
    metaData.insert("xesam:trackNumber",mPlaybackStatus->getTrackNr());
    metaData.insert("xesam:url", QString(mPlaybackStatus->getURL().toEncoded()));
    return metaData;
}
double DaedalusDBUSAdaptor::volume() {
    return 1.0;
}
qint64 DaedalusDBUSAdaptor::position() {
    return mPlaybackStatus->getElapsed() * 1000 * 1000;
}
double DaedalusDBUSAdaptor::minimumRate() {
    return 1.0;
}
double DaedalusDBUSAdaptor::maximumRate() {
    return 1.0;
}
bool DaedalusDBUSAdaptor::canPlay() {
    return true;
}
bool DaedalusDBUSAdaptor::canPause() {
    return true;
}
bool DaedalusDBUSAdaptor::canSeek() {
    return true;
}
bool DaedalusDBUSAdaptor::canControl() {
    return true;
}
bool DaedalusDBUSAdaptor::canGoNext() {
    return true;
}
bool DaedalusDBUSAdaptor::canGoPrevious() {
    return true;
}

void DaedalusDBUSAdaptor::playbackStateChange()
{
    // Send new metadata
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties","PropertiesChanged" );
    signal << "org.mpris.MediaPlayer2.Player";
    QVariantMap propertiesChanged;
    propertiesChanged.insert("Metadata", metaData());
    propertiesChanged.insert("PlaybackStatus",playbackStatus());
    propertiesChanged.insert("Shuffle",shuffle());
    propertiesChanged.insert("LoopStatus",loopStatus());
    propertiesChanged.insert("Position",position());
    signal << propertiesChanged;
    QDBusConnection::sessionBus().send(signal);
}


void DaedalusDBUSAdaptor::titleChanged() {
    // Send new metadata
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties","PropertiesChanged" );
    signal << "org.mpris.MediaPlayer2.Player";
    QVariantMap propertiesChanged;
    propertiesChanged.insert("Metadata", metaData());
    propertiesChanged.insert("PlaybackStatus",playbackStatus());
    propertiesChanged.insert("Position",position());
    signal << propertiesChanged;
    QDBusConnection::sessionBus().send(signal);
}

void DaedalusDBUSAdaptor::randomChanged() {
    // Send new metadata
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties","PropertiesChanged" );
    signal << "org.mpris.MediaPlayer2.Player";
    QVariantMap propertiesChanged;
    propertiesChanged.insert("Shuffle",shuffle());
    signal << propertiesChanged;
    QDBusConnection::sessionBus().send(signal);
}

void DaedalusDBUSAdaptor::repeatChanged() {
    // Send new metadata
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties","PropertiesChanged" );
    signal << "org.mpris.MediaPlayer2.Player";
    QVariantMap propertiesChanged;
    propertiesChanged.insert("LoopStatus",loopStatus());
    signal << propertiesChanged;
    QDBusConnection::sessionBus().send(signal);
}

void DaedalusDBUSAdaptor::positionChanged() {
    // Send new metadata
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties","PropertiesChanged" );
    signal << "org.mpris.MediaPlayer2.Player";
    QVariantMap propertiesChanged;
    propertiesChanged.insert("Position",position());
    signal << propertiesChanged;
    QDBusConnection::sessionBus().send(signal);
}
