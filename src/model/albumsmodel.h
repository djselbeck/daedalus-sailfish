#ifndef ALBUMSMODEL_H
#define ALBUMSMODEL_H

#include <QObject>
#include <Qt5Sparql/QSparqlQueryModel>

class AlbumsModel : public QAbstractListModel
{
    Q_OBJECT

    enum EntryRoles {
        AlbumRole = Qt::UserRole + 1,
        SectionRole,
        ArtistRole,
        AlbumCleandRole,
        AlbumImageRole
    };

public:
    explicit AlbumsModel(QObject *parent = 0, QSparqlConnection *connection = 0);

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void albumsReady();

public slots:
    void requestAlbums();
    void sparqlModelfinished();


private:
    int mRolesCount;
    QString mAlbumsQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;

};

#endif // ALBUMSMODEL_H
