#include "artistsmodel.h"

ArtistsModel::ArtistsModel(QObject *parent, QSparqlConnection *connection) :
    QAbstractListModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
        mArtistsQueryString = "SELECT  ?artist  COUNT(?album) as ?albumcount WHERE { ?album nmm:albumArtist ?artistobj . ?artistobj nmm:artistName ?artist } GROUP BY ?artist ORDER BY ?artist";
    }
    mSparqlModel = new QSparqlQueryModel(this);
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
}


void ArtistsModel::requestArtists()
{
    // Initialize the query
    //mSparqlModel->clear();
  //  beginResetModel();
    qDebug() << "getting artists";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mSparqlModel->setQuery(QSparqlQuery(mArtistsQueryString),*mConnection);
    //endResetModel();
}

void ArtistsModel::sparqlModelfinished()
{
 //   emit dataChanged(QModelIndex().child(0,0),QModelIndex().child(mSparqlModel->rowCount(),mSparqlModel->columnCount()));
    beginResetModel();
    emit modelChanged();
    qDebug() << "underlaying model finished result fetching";
    emit artistsReady();
    endResetModel();
}


QHash<int, QByteArray> ArtistsModel::roleNames() const {
    QHash<int,QByteArray> roles;
    roles[NameRole] = "artist";
    roles[AlbumCountRole] = "albumcount";
    roles[SectionRole] = "sectionprop";
    roles[ImageURLRole] = "imageURL";
    return roles;
}

int ArtistsModel::rowCount(const QModelIndex &parent) const {
    return mSparqlModel->rowCount(parent);
}

QVariant ArtistsModel::data(const QModelIndex &index, int role) const {
    QString artistName;
    switch ( role ) {
    case NameRole:
        return mSparqlModel->data(index,role);
        break;
    case SectionRole:
        artistName = mSparqlModel->data(index,NameRole).toString();
        return QString(artistName.at(0)).toUpper();
        break;
    case AlbumCountRole:

        break;
    case ImageURLRole:

        break;
    default:
        return "";
    }

    return "";
}

QVariantMap ArtistsModel::get(int row){
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
