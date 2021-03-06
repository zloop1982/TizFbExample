/*
 *
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

#ifndef __ANDROID__

#ifndef PCLOGIN_H_
#define PCLOGIN_H_

#include <QWebView>
#include <QDateTime>
#include <QStringList>

class QWebPage;

class LoginSystem
    :public QWebView
{
    Q_OBJECT
public:
    LoginSystem(QWidget *parent = 0);

    inline QDateTime getExpirationDate() const;
    inline QString getAccessToken() const;
    inline QStringList getGrantedPermissions() const;
signals:
    void accessTokenReceived();
public slots:
    void login(const QString &cid, const QString &ct,
               const QStringList &rp, const QStringList &pp);
    void logout(){}
    void show();
private slots:
    void onUrlChanged(QUrl);
private:
    QStringList permissions;
    QString accessToken;
    QString clientId;

    QDateTime expirationDate;
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
    return permissions;
}

#endif      //LoginSystem_H_
#endif      //__ANDROID__
