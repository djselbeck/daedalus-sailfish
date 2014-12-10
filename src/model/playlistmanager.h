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
    void sendBusy(bool);

public slots:
    void requestPlaylist(QString url);
    void deleteActivePlaylist();
    void deletePlaylist(QString name);

private:
    SavedPlaylistTracksModel *mTracksModel;
    QSparqlConnection *mSparQLConnection;

    QThread *mThread;
    QString mActiveFile;


};

#endif // PLAYLISTMANAGER_H
