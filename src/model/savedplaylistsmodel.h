#ifndef SAVEDPLAYLISTSMODEL_H
#define SAVEDPLAYLISTSMODEL_H

#include <QAbstractListModel>
#include <Qt5Sparql/QSparqlQueryModel>


class SavedPlaylistsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount )
public:
    explicit SavedPlaylistsModel(QObject *parent = 0, QSparqlConnection *connection = 0, QThread *fetchthread = 0);

    enum EntryRoles {
        NameRole = Qt::UserRole + 1,
        PlaylistURLRole,
        PlaylistURNRole
    };

    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariantMap get(int row);
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void playlistsReady();

public slots:
    void requestSavedPlaylists();
    void sparqlModelfinished();

private:
    int mRolesCount;
    QString mPlaylistQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;
    QThread *mThread;


};

#endif // SAVEDPLAYLISTSMODEL_H
