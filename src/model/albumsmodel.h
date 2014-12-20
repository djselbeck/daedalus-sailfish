#ifndef ALBUMSMODEL_H
#define ALBUMSMODEL_H

#include <QObject>
#include <Qt5Sparql/QSparqlQueryModel>
#include <QThread>

#include <global.h>
#include <metadata/imagedatabase.h>
#include <metadata/imagedownloader.h>

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
        ArtistURNRole,
        SectionRole,
        AlbumCleandRole,
        AlbumImageRole
    };

public:
    explicit AlbumsModel(QObject *parent = 0, QSparqlConnection *connection = 0, QThread *fetchthread = 0, ImageDatabase *db = 0, bool downloadEnable = false);

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    QVariant data(const QModelIndex &index, int role) const;



signals:
    void albumsReady();
    void requestAlbumInformation(Albumtype album) const;
    void requestDBEnter(AlbumInformation *info);
    void sendBusy(bool);

public slots:
    void requestAlbums();
    void requestAlbums(QString artist);
    void requestArtistAlbumsReverseFromTrack(QString urn);
    void requestArtistAlbumList();
    void sparqlModelfinished();
    void setDownloadEnabled(bool enabled);
    void setDownloadSize(QString size);

    void clearData();


private:
    int mRolesCount;
    QString mAlbumsQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;
    QThread *mThread;

    bool mDownloadEnabled;
    bool mPartialModel;
    QString mDownloadSize;
    ImageDatabase *mDB;
    ImageDownloader *mDownloader;

private slots:
    void albumInformationReady(AlbumInformation *info);
    void albumEntered(QString albumName);

};

#endif // ALBUMSMODEL_H
