#ifndef ALBUMTRACKSMODEL_H
#define ALBUMTRACKSMODEL_H

#include <QAbstractListModel>
#include <QSparqlQueryModel>
#include <QThread>
#include <QSparqlConnection>

class AlbumTracksModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount)

    enum EntryRoles {
        TitleRole = Qt::UserRole + 1,
        ArtistRole,
        AlbumRole,
        DurationRole,
        TrackNumberRole,
        DiscNumberRole,
        FileURLRole,
        TrackURNRole,
        DurationFormattedRole
    };
public:
    explicit AlbumTracksModel(QObject *parent = 0, QSparqlConnection *connection = 0, QThread *fetchthread = 0);

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void modelReady();

public slots:
    void requestAlbumTracks(QString);
    void requestAlbumTracksReverseFromTrack(QString urn);
    void requestArtistTracks(QString artisturn);

    void clearData();

private:
    QString mAlbumTracksQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;
    QThread *mThread;

    inline QString getLengthFormatted(int length) const;

private slots:
    void sparqlModelfinished();

};

#endif // ALBUMTRACKSMODEL_H
