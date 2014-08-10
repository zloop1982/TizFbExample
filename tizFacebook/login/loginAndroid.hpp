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

#ifndef LOGINANDROID_HPP
#define LOGINANDROID_HPP

#include <QObject>
#include <QDateTime>
#include <QStringList>

class QTimer;

class LoginSystem
        :public QObject
{
    Q_OBJECT
public:
    explicit LoginSystem(QObject *parent = 0);

    inline void setAppName(const QString &);
    inline QDateTime getExpirationDate() const;
    inline QString getAccessToken() const;
    inline QStringList getGrantedPermissions() const;
signals:
    void accessTokenReceived();
public slots:
    void login(const QString &cid, const QString &ct,
               const QStringList &rp, const QStringList &pp);
    void logout();
private slots:
    void checkState();
private:
    void privateGetAccessToken();

    QDateTime expirationDate;
    QStringList grantedPermissions;
    QString accessToken;
    QTimer *checkTimer;
    int statusChecks;
    int maxStatusChecks;

    enum ProcessState {
        invalid_state = 0,
        reading_permissions_requested = 1,
        reading_permissions_granted = 2,
        publish_permissions_requested = 3,
        publish_permissions_granted = 4,
        state_before_login = 5,
        loged_in = 6
    };
};

inline QDateTime LoginSystem::getExpirationDate() const
{
    return expirationDate;
}

inline QString LoginSystem::getAccessToken() const
{
    return accessToken;
}

inline QStringList LoginSystem::getGrantedPermissions() const
{
    return grantedPermissions;
}

#endif // ANDROIDLOGIN_HPP


