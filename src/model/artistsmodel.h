#ifndef ARTISTSMODEL_H
#define ARTISTSMODEL_H

#include <QAbstractListModel>
#include <Qt5Sparql/QSparqlQueryModel>


class ArtistsModel : public QAbstractListModel
{
    Q_OBJECT    
    Q_PROPERTY(int count READ rowCount)

public:
    explicit ArtistsModel(QObject *parent = 0, QSparqlConnection *connection = 0);

    enum EntryRoles {
        NameRole = Qt::UserRole + 1,
        AlbumCountRole,
        SectionRole,
        ImageURLRole
    };

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE QVariantMap get(int row);


signals:
    void artistsReady();

public slots:
    void requestArtists();

private slots:
    void sparqlModelfinished();


private:
    QString mArtistsQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;

};

#endif // ARTISTSMODEL_H
