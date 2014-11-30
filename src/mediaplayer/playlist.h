#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QAbstractListModel>

#include "../model/trackobject.h"

class Playlist : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit Playlist(QObject *parent = 0);

    enum EntryRoles {
        TitleRole = Qt::UserRole + 1,
        ArtistRole,
        AlbumRole,
        DurationRole,
        TrackNumberRole,
        DiscNumberRole,
        FileURLRole,
        DurationFormattedRole,
        PlayingRole
    };

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:
    void addFile(TrackObject *track);
    void removePosition(int position);
    void playPosition(int position);
    void setPlaybackMode(QMediaPlaylist::PlaybackMode mode);



private:
    QMediaPlayer *mPlayer;
    QMediaPlaylist *mQPlaylist;
    QList<TrackObject*> *mTrackList;
    int mOldIndex;

private slots:
    void indexChanged(int index);

};

#endif // PLAYLIST_H
