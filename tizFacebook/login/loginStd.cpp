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
#ifndef __ANDROID__

#include "loginStd.hpp"
#include <QtWebKit>
#include <QFile>
#include <QWebFrame>
#include <QDateTime>

LoginSystem::LoginSystem(QWidget *parent)
    :QWebView(parent)
    ,permissions(QStringList())
    ,accessToken(QString())
    ,expirationDate(QDateTime())
{
    connect(this, SIGNAL(urlChanged(QUrl)),
            this, SLOT(onUrlChanged(QUrl)));

}

void LoginSystem::show()
{
    //prepare permissions
    QString scope;
    for(int i = 0; i < permissions.length(); i++)
    {
        scope.append(permissions.at(i));
        if(i < permissions.length()-1)
        {
            scope.append(",");
        }
    }
    qDebug()<<"Permissions:"<<scope;

    //-------
    QWebView::show();
    QString url;
    url.push_back("https://m.facebook.com/dialog/oauth?client_id=");
    url.push_back(clientId);
    if(!scope.isEmpty())
    {
        url.push_back("&scope=");
        url.push_back(scope);
    }
    url.push_back("&redirect_uri=http://www.facebook.com/connect/login_success.html&response_type=token");
    qDebug()<<url;
    load(QUrl(url));
}

void LoginSystem::onUrlChanged(QUrl u)
{
    if(u.toString().contains("http://www.facebook.com/connect/login_success.html#access_token"))
    {
        hide();
        QString puffer = u.fragment().remove("access_token=");
        int expPos = puffer.indexOf("&expires_in=");
        int expLength = puffer.length() - expPos;

        //compute expiration Date
        QDateTime d = QDateTime::currentDateTime();
        expirationDate = d.addSecs(2 * 60 * 60);         // 2h
        qDebug()<<expirationDate.toString();

        accessToken = puffer.remove(expPos, expLength);
        emit accessTokenReceived();
    }
}

void LoginSystem::login(const QString &cid, const QString &ct,
                        const QStringList &rp, const QStringList &pp)
{
    //regular

    if(clientId.isEmpty())
    {
        qDebug()<<"Error: clientId is empty";
        qDebug()<<"Try to set the clientId with TizFacebook::setClientId()";
    }
    clientId = cid;
    permissions = rp + pp;
    show();


    //alternative for debuging
    /*
    accessToken = "<accessToken here>";
    emit accesTokenReceived(accessToken);
    */
}

#endif      //__ANDROID__
