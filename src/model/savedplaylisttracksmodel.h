#ifndef SAVEDPLAYLISTTRACKSMODEL_H
#define SAVEDPLAYLISTTRACKSMODEL_H

#include <QAbstractListModel>
#include <QSparqlQueryModel>
#include <QThread>
#include <QUrl>

#include "trackobject.h"

class SavedPlaylistTracksModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SavedPlaylistTracksModel(QObject *parent = 0,QList<QUrl> *tracks = 0, QSparqlConnection *connection = 0, QThread *fetchthread = 0);

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
        SectionRole
    };

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    TrackObject *getTrack(int position);
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void sendBusy(bool);

public slots:
    void requestTrack(QUrl track);

private:
    QList<QUrl>  *mURLs;
    QList<TrackObject*>* mTracks;
    QString mCurrentFile;

    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;

    QThread *mThread;

private slots:
    void trackReady();

};

#endif // SAVEDPLAYLISTTRACKSMODEL_H
