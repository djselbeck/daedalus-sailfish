#include "savedplaylistsmodel.h"

#include <QUrl>

SavedPlaylistsModel::SavedPlaylistsModel(QObject *parent, QSparqlConnection *connection, QThread *fetchthread) :
    QAbstractListModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
    }
    mSparqlModel = new QSparqlQueryModel(0);
    if ( fetchthread != 0  ) {
        mThread = fetchthread;
        mSparqlModel->moveToThread(mThread);
    }
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
}

void SavedPlaylistsModel::requestSavedPlaylists()
{
    // Initialize the query
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting playlists";
    mPlaylistQueryString = "SELECT ?name ?url ?playlist WHERE { ?playlist a nmm:Playlist . OPTIONAL {?playlist nie:title ?name} . ?playlist nie:url ?url }";
    mSparqlModel->setQuery(QSparqlQuery(mPlaylistQueryString),*mConnection);
    endResetModel();
}

void SavedPlaylistsModel::sparqlModelfinished()
{
    beginResetModel();
    qDebug() << "underlaying model finished result fetching";
    emit playlistsReady();
    endResetModel();
}

QHash<int, QByteArray> SavedPlaylistsModel::roleNames() const {
    QHash<int,QByteArray> roles;

    roles[NameRole] = "name";
    roles[PlaylistURLRole] = "playlisturl";
    roles[PlaylistURNRole] = "playlisturn";
    return roles;
}

int SavedPlaylistsModel::rowCount(const QModelIndex &parent) const {
    return mSparqlModel->rowCount(parent);
}

QVariant SavedPlaylistsModel::data(const QModelIndex &index, int role) const {
    QString albumTitle;
    switch ( role ) {
    case PlaylistURLRole:
        return mSparqlModel->data(index,role);
    case PlaylistURNRole:
        return mSparqlModel->data(index,role);
    }
    if ( role == NameRole) {
        QString sparQLName = mSparqlModel->data(mSparqlModel->index(index.row(),0),0).toString();
        QUrl sparQLURL = mSparqlModel->data(mSparqlModel->index(index.row(),1),0).toString();
        qDebug() << "name " << sparQLName << sparQLURL;
        if ( sparQLName == "" ) {
            return sparQLURL.fileName();
        }
        return sparQLName;
    }

    return "";
}

/**
 * This returns an pseudo item in an variant map for SectionScroller
 * @brief AlbumsModel::get
 * @param row
 * @return
 */
QVariantMap SavedPlaylistsModel::get(int row){
    QHash<int,QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> i(roles);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}
