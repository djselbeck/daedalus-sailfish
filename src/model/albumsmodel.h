#ifndef ALBUMSMODEL_H
#define ALBUMSMODEL_H

#include <QObject>
#include <Qt5Sparql/QSparqlQueryModel>
#include <QThread>

class AlbumsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount )

    enum EntryRoles {
        AlbumRole = Qt::UserRole + 1,
        TrackCountRole,
        DurationRole,
        ArtistRole,
        AlbumURNRole,
        SectionRole,
        AlbumCleandRole,
        AlbumImageRole
    };

public:
    explicit AlbumsModel(QObject *parent = 0, QSparqlConnection *connection = 0, QThread *fetchthread = 0);

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void albumsReady();

public slots:
    void requestAlbums();
    void requestAlbums(QString artist);
    void requestArtistAlbumsReverseFromTrack(QString urn);
    void sparqlModelfinished();


private:
    int mRolesCount;
    QString mAlbumsQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;
    QThread *mThread;

};

#endif // ALBUMSMODEL_H
