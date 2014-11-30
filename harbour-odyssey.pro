# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-odyssey

CONFIG += sailfishapp qtsparql

SOURCES += src/harbour-odyssey.cpp \
    src/maincontroller.cpp \
    src/model/albumsmodel.cpp \
    src/model/artistsmodel.cpp \
    src/model/albumtracksmodel.cpp

OTHER_FILES += qml/harbour-odyssey.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-odyssey.changes.in \
    rpm/harbour-odyssey.spec \
    rpm/harbour-odyssey.yaml \
    translations/*.ts \
    harbour-odyssey.desktop \
    qml/pages/MainPage.qml \
    qml/pages/AlbumsList.qml \
    qml/components/AlbumListDelegate.qml \
    qml/components/SectionScroller.js \
    qml/components/SpeedScroller.js \
    qml/components/AlbumDelegate.qml \
    qml/components/AlbumShowDelegate.qml \
    qml/components/SectionScroller.qml \
    qml/components/SpeedScroller.qml \
    qml/pages/ArtistsList.qml \
    qml/components/ArtistDelegate.qml \
    qml/components/ArtistListDelegate.qml \
    qml/components/ArtistShowDelegate.qml \
    qml/commonfunctions/clickHandler.js \
    qml/pages/AlbumTracks.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/harbour-odyssey-de.ts

HEADERS += \
    src/maincontroller.h \
    src/model/albumsmodel.h \
    src/global.h \
    src/model/artistsmodel.h \
    src/model/albumtracksmodel.h

