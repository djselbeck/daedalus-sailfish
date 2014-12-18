import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: scrobblePage
    SilicaFlickable {
        id: mainFlickable
        anchors.fill: parent
        clip: true
        contentHeight: mainColumn.height
        Column {
            id: mainColumn
            spacing: Theme.paddingSmall
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: listPadding
                rightMargin: listPadding
            }
            DialogHeader {
                id: header
                title: qsTr("Scrobbling")
            }
            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: implicitHeight
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: "WordWrap"
                text: qsTr("Please enter your last.fm credentials if you want to scrobble your songs to last.fm. Only an authentication hash is saved on the device.")
            }
            Separator {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - Theme.paddingLarge
                color: Theme.primaryColor
            }
            Label {
                text: qsTr("username:")
                anchors.right: parent.right
                anchors.left: parent.left
            }
            TextField {
                anchors.right: parent.right
                anchors.left: parent.left
                id: usernameText
                placeholderText: qsTr("input username")
                inputMethodHints: Qt.ImhNoPredictiveText
            }
            Label {
                text: qsTr("password:")
                anchors.right: parent.right
                anchors.left: parent.left
            }
            TextField {
                anchors.right: parent.right
                anchors.left: parent.left
                id: passwordText
                placeholderText: qsTr("input username")
                inputMethodHints: Qt.ImhNoPredictiveText
                echoMode: TextInput.Password
            }
            Button {
                id: clearAuthentication
                anchors.right: parent.right
                anchors.left: parent.left
                text: qsTr("clear authentication")
                onClicked: {
                    clearLastFMAuthentication();
                }
            }

            Label {
                color: Theme.secondaryColor
                text: lastfmsessionkey === "" ? qsTr("not authenticated") : qsTr("authenticated with key:\n") + lastfmsessionkey
                anchors.right: parent.right
                anchors.left: parent.left
            }
        }
    }
    onAccepted: {
        if ( usernameText.text !=="" && passwordText.text !== "") {
            newLastfmUserData([usernameText.text,passwordText.text]);
        }
    }
}
