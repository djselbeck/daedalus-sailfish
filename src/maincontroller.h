#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QQuickView>
#include <QQmlContext>
#include <Qt5Sparql/QSparqlConnection>

// local includes
#include "model/albumsmodel.h"

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    MainController(QQuickView *viewer,QObject *parent = 0);

signals:

public slots:
    void requestAlbums();
    void albumsReady();
    void artistsReady();
    void playlistsReady();
    void folderReady();

    void finished();

private:
    // SparQL
    QSparqlConnection *mSparQLConnection;

    AlbumsModel *mAlbumsModel;


    // Qml stuff
    QQuickView *mQuickView;

};

#endif // MAINCONTROLLER_H
