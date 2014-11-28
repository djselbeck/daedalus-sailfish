#include "maincontroller.h"

// remove me
#include <QSparqlError>

MainController::MainController(QObject *parent) :
    QObject(parent)
{
}

MainController::MainController(QQuickView *viewer, QObject *parent) : QObject(parent)
{
    mSparQLConnection = new QSparqlConnection("QTRACKER_DIRECT");
    mAlbumsModel = new AlbumsModel(this,mSparQLConnection);
    connect(mAlbumsModel,SIGNAL(finished()),this,SLOT(finished()));
    qDebug() << "SparQL connection has " << mSparQLConnection->drivers().size() << " drivers";
    qDebug() << "SparQL connection isValid: " << mSparQLConnection->isValid() << endl;
    foreach (QString driver, mSparQLConnection->drivers()) {
        qDebug() << "SparQL driver: " << driver;
    }
    qDebug() << "Model error: " << mAlbumsModel->lastError().message() << endl;
    qDebug() << "Row count: " << mAlbumsModel->rowCount() << endl;
    mQuickView = viewer;

    mQuickView->rootContext()->setContextProperty("albumsModel",mAlbumsModel);
    mQuickView->rootContext()->setContextProperty("listImageSize", 1);
}

void MainController::requestAlbums()
{

}

void MainController::finished()
{
    qDebug() << "Model finished" << endl;
    qDebug() << "Size: " << mAlbumsModel->rowCount() << ":" << mAlbumsModel->columnCount() << endl;
    for( int i = Qt::UserRole + 1;i<= Qt::UserRole + 4 ; i++) {
        qDebug() << "role " << i << " name: " << QString::fromLatin1(mAlbumsModel->roleNames()[i].data()) << endl;
    }
    mQuickView->rootContext()->setContextProperty("albumsModel",mAlbumsModel);
}

void MainController::albumsReady()
{

}

void MainController::artistsReady()
{

}

void MainController::playlistsReady()
{

}

void MainController::folderReady()
{

}
