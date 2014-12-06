#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QSparqlConnection>
#include <QObject>
#include "savedplaylisttracksmodel.h"

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistManager(QObject *parent = 0, QSparqlConnection *connection = 0);

signals:
    void playlistTracksReady(SavedPlaylistTracksModel *model);

public slots:
    void requestPlaylist(QString url);

private:
    SavedPlaylistTracksModel *mTracksModel;
    QSparqlConnection *mSparQLConnection;

    QThread *mThread;


};

#endif // PLAYLISTMANAGER_H
