import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: scrobblePage
    SilicaFlickable {
        id: mainFlickable
        anchors.fill: parent
        clip: true
        Column {
            id: mainColumn
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
                text: qsTr("username:")
                anchors.right: parent.right
                anchors.left: parent.left
            }
            TextField {
                anchors.right: parent.right
                anchors.left: parent.left
                id: usernameText
                text: lastfmusername
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
                text: lastfmpassword
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
                text: lastfmsessionkey === "" ? qsTr("not authenticated") : qsTr("authenticated with key:\n") + lastfmsessionkey
                anchors.right: parent.right
                anchors.left: parent.left
            }
        }
    }
    onAccepted: {
        newLastfmUserData([usernameText.text,passwordText.text]);
    }
}
