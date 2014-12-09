#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QAbstractListModel>
#include <QTimer>
#include <QThread>

#include "../model/trackobject.h"
#include "../model/playbackstatusobject.h"
#include "../model/playbackstate.h"

class Playlist : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit Playlist(QObject *parent = 0);
    ~Playlist();

    enum EntryRoles {
        TitleRole = Qt::UserRole + 1,
        ArtistRole,
        AlbumRole,
        DurationRole,
        TrackNumberRole,
        DiscNumberRole,
        FileURLRole,
        TrackURNRole,
        DurationFormattedRole,
        PlayingRole,
        SectionRole
    };

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    QVariant data(const QModelIndex &index, int role) const;

    int currentIndex();

    void registerStatusObject(PlaybackStatusObject *obj);

    static QString getXDGMusicDir();

    void savePlaylistToSql();

signals:
    void requestSavedPlaylistState();
    void requestPlaylistStateSave(QList<TrackObject*>*);
    void sendBusy(bool);

public slots:
    void addFile(TrackObject *track);
    void playSong(TrackObject *track);
    void insertAt(TrackObject *track, int pos);
    void removePosition(int position);
    void moveTrack(int from, int to);
    void playNext(int position);
    void playPosition(int position);
    void setPlaybackMode(QMediaPlaylist::PlaybackMode mode);
    void setRandom(bool random);
    void setRepeat(bool repeat);

    void clear();

    // Control slots
    void next();
    void previous();
    void pause();
    void play();
    void togglePlayPause();
    void stop();
    void seek(int pos);

    void savePlaylist(QString name);
    void resumePlaylist();



private:
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mQPlaylist;
    QList<TrackObject*> *mTrackList;
    int mOldIndex;
    PlaybackStatusObject *mStatusObject;
    PlaybackState *mPlaybackState;

    QThread *mBackgroundThread;



private slots:
    void indexChanged(int index);
    void updateStatus();

    void receiveSavedPlaybackStateList(QList<TrackObject*>*);

};

#endif // PLAYLIST_H
