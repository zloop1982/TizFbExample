/*
* Copyright (c) 2014 Christian Schabesberger
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef TizFacebook_H_
#define TizFacebook_H_

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QPixmap>
#include <QDir>
#include <QDateTime>
#include <QStringList>

class LoginSystem;
class QNetworkAccessManager;
class TizScoreFile;

class TizFacebook
    : public QObject
{
    Q_OBJECT
public:
    explicit TizFacebook(QObject *parent = 0);
    inline bool isLoggedIn() const;
    bool hasValidAccessToken() const;
    inline void setFacebookDir(QString);
    inline void setClientToken(const QString &);
    inline void setClientId(const QString &);
    inline void setReadPermissions(const QStringList &);
    inline void setPublishPermissions(const QStringList &);
    inline void setGeneralTimeout(const int &);
    void setStdPicture(const QPixmap &);
    inline void deleteFbDir();

    bool testConnection();
    inline QPixmap* getStdPicture() const;
    QString getId(const QString &id = "");
    QString getName(const QString &id = "");
    QString getFirstName(const QString &id = "");
    QString getLastName(const QString &id = "");
    QPixmap getPicture(const QString &id = "", bool forceDonwload = false);
    bool isMale(const QString &id = "");
    QStringList getFriendsUsingApp();
    inline QStringList getGrantedPermissions() const;
    inline QDateTime getExpirationDate() const;
    inline bool isOnline() const;

    void requestMe(bool force = false, bool save = false);
signals:
    void loginChanged(bool loggedIn, bool online);
    void connectionFailed() const;
public slots:
    void login();
    void logout();
private slots:
    void onAccessTokenChanged();
protected:
    QByteArray simpleGet(const QUrl &url, int to = 0) const;
    QJsonDocument get(const QString &path, const QString &fields, int to = 0) const;       //this automaticly includes the acces token
                                                                                          //and the https://graph.TizFacebook.com part
    bool post(const QString &path, const QString &variable, const QString &value, int to = 0);
    bool post(const QString &path, const QHash<QString, QString> &data, int to = 0);

    void createFbDir();
private:
    QByteArray getJpegPicture(const QString &id = "");
    void loadPicture(const QString &id = "", bool forceDownload = false);

    QByteArray simplePost(const QUrl&, const QHash<QString, QString> &data, int to = 0);

    QString accessToken;
    QString clientToken;
    QString clientId;
    QStringList readPermissions;
    QStringList publishPermissions;
    QStringList grantedPermissions;
    QDateTime expirationDate;

    int timeOut;

    LoginSystem *loginSystem;
    QString facebookDir;

    bool online;

    QJsonDocument me;

    QPixmap *stdPicture;
};

inline bool TizFacebook::isLoggedIn() const
{
    if(hasValidAccessToken() && online)
        return true;
    return false;
}

inline void TizFacebook::setFacebookDir(QString v)
{
    facebookDir = v;
}

inline void TizFacebook::deleteFbDir()
{
    QDir::home().remove(facebookDir);
}

inline void TizFacebook::setClientToken(const QString &v)
{
    clientToken = v;
}

inline void TizFacebook::setClientId(const QString &v)
{
    clientId = v;
}

inline void TizFacebook::setReadPermissions(const QStringList &v)
{
    readPermissions = v;
}

inline void TizFacebook::setPublishPermissions(const QStringList &v)
{
    publishPermissions = v;
}

inline void TizFacebook::setGeneralTimeout(const int &v)
{
    timeOut = v;
}

inline QPixmap* TizFacebook::getStdPicture() const
{
    return stdPicture;
}

inline bool TizFacebook::isOnline() const
{
    return online;
}

inline QStringList TizFacebook::getGrantedPermissions() const
{
    return grantedPermissions;
}

inline QDateTime TizFacebook::getExpirationDate() const
{
    return expirationDate;
}

#endif
