#ifndef PLAYBACKSTATE_H
#define PLAYBACKSTATE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "trackobject.h"

class PlaybackState : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackState(QObject *parent = 0);


signals:
    void lastPlaylistReady(QList<TrackObject*> *list);

public slots:
    void saveCurrentPlaylist(QList<TrackObject*> *list);
    void resumeLastPlaylist();

private:
    bool openDB();
    bool createTables();

    QSqlDatabase *mDatabase;

};

#endif // PLAYBACKSTATE_H
