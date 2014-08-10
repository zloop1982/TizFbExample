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

#include "tizFacebook.hpp"
#include "eventSlave.hpp"

#ifdef __ANDROID__
#include "login/loginAndroid.hpp"
#else
#include "login/loginStd.hpp"
#endif


#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QTimer>

TizFacebook::TizFacebook(QObject *parent)
    :QObject(parent)
    ,accessToken(QString())
    ,readPermissions(QStringList())
    ,publishPermissions(QStringList())
    ,grantedPermissions(QStringList())
    ,timeOut(0)
    ,loginSystem(new LoginSystem())
    ,facebookDir("facebook")
    ,online(false)
    ,stdPicture(new QPixmap())
{
    connect(loginSystem, SIGNAL(accessTokenReceived()),
            this, SLOT(onAccessTokenChanged()));
}

void TizFacebook::onAccessTokenChanged()
{
    accessToken = loginSystem->getAccessToken();
    expirationDate = loginSystem->getExpirationDate();
    grantedPermissions = loginSystem->getGrantedPermissions();
    testConnection();
    qDebug()<<"Facebook Loginstate changed: loggedin="<<!accessToken.isNull()<<
              "isOnline="<<isOnline();
    emit loginChanged(!accessToken.isNull(), isOnline());
}

void TizFacebook::login()
{
    if(clientToken.isEmpty())
        qDebug()<<"Error: TizFacebook: clientToken not set";
    else {
        if(!isLogedIn())
            loginSystem->login(clientId, clientToken, readPermissions, publishPermissions);
        else {
            qDebug()<<"Login requestet but allready logged in";
            qDebug()<<"Please logout an log in again if you whant to update permissions";
        }
    }
}

void TizFacebook::logout()
{
    if(isLogedIn())
    {
        loginSystem->logout();
        accessToken = "";
        expirationDate = QDateTime();
        grantedPermissions = QStringList();
        emit loginChanged(!accessToken.isEmpty(), false);
    } else {
        qDebug()<<"You are already logged out";
    }
}

QJsonDocument TizFacebook::get(const QString &path,const QString &fields, int to) const
{
    //QNetworkReply *reply;
    QJsonDocument data;

    //setting up url
    QString url = "https://graph.facebook.com";
    url.push_back(path);
    if(fields.isEmpty())
        url.push_back("?access_token=");
    else {
        url.push_back("?fields=");
        url.push_back(fields);
        url.push_back("&access_token=");
    }
    url.push_back(accessToken);
    //qDebug()<<url;
    QByteArray reply = simpleGet(QUrl(url), to);
    QJsonParseError parse_error;
    data = QJsonDocument::fromJson(reply, &parse_error);
    if(data.isEmpty())
        qDebug()<<"Error: TizFacebook: reply failed with parse error: "<<parse_error.errorString()
                <<" Offset:"<<parse_error.offset
                <<" Reply size: "<<reply.size();

    return data;
}

QByteArray TizFacebook::simpleGet(const QUrl &url, int to) const
{
    QEventLoop waitingLoop;
    QNetworkReply *reply;
    QByteArray puffer;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager;
    EventSlave eventSlave;
    QTimer timeOutTimer;
    QNetworkRequest request(url);
    timeOutTimer.setInterval(to);
    timeOutTimer.setSingleShot(true);

    reply = accessManager->get(request);
    connect(reply, SIGNAL(finished()),
        &eventSlave, SLOT(onReplyReceived()));
    connect(&timeOutTimer, SIGNAL(timeout()),
            &eventSlave, SLOT(onTimeOutTimer()));
    connect(&eventSlave, SIGNAL(ready()),
            &waitingLoop, SLOT(quit()));

    if(to)
        timeOutTimer.start();
    waitingLoop.exec();

    if(eventSlave.succeed())
        puffer = reply->readAll();
    else
        emit connectionFailed();
    //qDebug()<<"reply size: "<<puffer.size();
    delete reply;
    delete accessManager;
    return puffer;
}

bool TizFacebook::post(const QString &path, const QString &variable, const QString &value, int to)
{
    //settings up url
    QString url = "https://graph.facebook.com";
    url.push_back(path);
    url.push_back("?access_token=");
    url.push_back(accessToken);

    //setup data
    QHash<QString, QString> data;
    data.insert(variable, value);

    QByteArray reply = simplePost(QUrl(url), data, to);
    //qDebug()<<QString(reply);
    if(QString(reply) == "true")
        return true;
    else
        return false;
}

bool TizFacebook::post(const QString &path, const QHash<QString, QString> &data, int to)
{
    //settings up url
    QString url = "https://graph.facebook.com";
    url.push_back(path);
    url.push_back("?access_token=");
    url.push_back(accessToken);

    //setup data
    //QHash<QString, QString> data;
    //data.insert(variable, value);

    QByteArray reply = simplePost(QUrl(url), data, to);
    qDebug()<<QString(reply);
    if(QString(reply) == "true")
        return true;
    else
        return false;
}

/*QByteArray TizFacebook::simplePost(const QUrl &url, const QString &variable, const QString &value)
{
    QEventLoop waitingLoop;
    QNetworkReply *reply;
    QNetworkRequest request(url);
    QNetworkAccessManager accessManager;
    QByteArray puffer;

    //request.setHeader();
    reply = accessManager.post(request, value.toLatin1());
    connect(reply, SIGNAL(finished()),
            &waitingLoop, SLOT(quit()));
    waitingLoop.exec();

    puffer = reply->readAll();

    delete reply;
    return puffer;
} */

QByteArray TizFacebook::simplePost(const QUrl &url, const QHash<QString, QString> &data, int to)
{
    QEventLoop waitingLoop;
    QNetworkReply *reply;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkAccessManager accessManager;
    QByteArray puffer;

    QTimer timeOutTimer(this);
    timeOutTimer.setInterval(to);
    timeOutTimer.setSingleShot(true);
    EventSlave eventSlave(this);

    //setup data
    {
        int ic = 0;
        for(QHash<QString, QString>::const_iterator i = data.begin();
            i != data.end(); i++)
        {
            puffer.push_back(i.key().toLatin1());
            puffer.push_back('=');
            puffer.push_back(i.value().toLatin1());
            ic++;
            if(ic < data.size())
                puffer.push_back('&');
        }
    }

    //send
    //request.setHeader();
    reply = accessManager.post(request, puffer);
    connect(reply, SIGNAL(finished()),
            &eventSlave, SLOT(onReplyReceived()));
    connect(&timeOutTimer, SIGNAL(timeout()),
            &eventSlave, SLOT(onTimeOutTimer()));
    connect(&eventSlave, SIGNAL(ready()),
            &waitingLoop, SLOT(quit()));

    if(to)
        timeOutTimer.start();
    waitingLoop.exec();

    if(eventSlave.succeed())
        puffer = reply->readAll();

    delete reply;
    return puffer;
}

QString TizFacebook::getId(const QString &id)
{
    requestMe();
    if(id.isEmpty())
        return me.object().value("id").toString();
    else
        return id;
}

QString TizFacebook::getName(const QString &id)
{
    requestMe();
    if(id.isEmpty())
        return me.object().value("name").toString();
    else {
        return get("/" + id, "name", timeOut).object()
                .value("name").toString();
    }
}

QString TizFacebook::getFirstName(const QString &id)
{
    requestMe();
    if(id.isEmpty())
        return me.object().value("first_name").toString();
    else {
        return get("/" + id, "first_name", timeOut).object()
                .value("last_name").toString();
    }
}

QString TizFacebook::getLastName(const QString &id)
{
    requestMe();
    if(id.isEmpty())
        return me.object().value("last_name").toString();
    else {
        return get("/" + id, "last_name", timeOut).object()
                .value("last_name").toString();
    }
}

QByteArray TizFacebook::getJpegPicture(const QString &id)
{
    requestMe();
    QString url;
    if(id.isEmpty())
    {
        url = me.object().value("picture").toObject()
                                .value("data").toObject()
                                .value("url").toString();
    } else {
         url = get("/" + id, "picture", timeOut).object()
                 .value("picture").toObject()
                 .value("data").toObject()
                 .value("url").toString();
    }
    return simpleGet(QUrl(url), timeOut);
}

void TizFacebook::loadPicture(const QString &id, bool forceDonwload)
{
    requestMe();

    createFbDir();
    //if(!QDir(facebookDir).exists())
      //  QDir::home().mkdir(facebookDir);

    QFile file(QDir::homePath() + "/" + facebookDir + "/" + getId(id) + ".jpeg");


    if(!file.exists() || forceDonwload)
    {
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug()<<"Error: TizFacebook: Couldn't open to wrtie: "<<file.fileName();
            qDebug()<<"Occurred error was: "<<file.errorString();
        }

        file.write(getJpegPicture(id));

        file.close();
    }
}

QPixmap TizFacebook::getPicture(const QString &id, bool forceDownload)
{
    loadPicture(id);
    QPixmap img;
    if(!img.load(QDir::homePath() + "/" + facebookDir + "/" + getId(id) + ".jpeg")
            || forceDownload)
        img = *stdPicture;
    return img;
}

bool TizFacebook::isMale(const QString &id)
{
    requestMe();
    if(id.isEmpty())
    {
        if(me.object().value("gender").toString() == "male")
            return true;
        else
            return false;
    } else {
        if(get("/" + id, "gender", timeOut)
                .object().value("gender").toString() == "male")
            return true;
        else
            return false;
    }
}

QStringList TizFacebook::getFriendsUsingApp()
{
    const QString fields = "friends.fields(id)";
    QStringList target;
    QJsonArray friends=get("/me", fields, timeOut).object()
            .value("friends").toObject()
            .value("data").toArray();
    for(int i = 0; i < friends.size(); i++)
        target.push_back(friends.at(i).toObject()
                         .value("id").toString());
    return target;
}

void TizFacebook::requestMe(bool force, bool save)
{
    const QString meFile = QDir::homePath() + "/" + facebookDir + "/me.json";

    if(isOnline())
    {
        QString fields("id,name,first_name,last_name,gender,"
                       "picture.width(100).height(100)");
        if(me.isEmpty() || force)
            me = get("/me", fields, timeOut);

        if(save)
        {
            createFbDir();
            QFile file(meFile);
            if(!file.open(QIODevice::WriteOnly))
            {
                qDebug()<<"Error: TizFacebook: Couldn't open to write: "<<file.fileName();
                qDebug()<<"Occurred error was: "<<file.errorString();
            }
            file.write(me.toJson());
            file.close();
        }
    } else {
        QFile file(meFile);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Error: TizFacebook: Couldn't open to read: "<<file.fileName();
            qDebug()<<"Occurred error was: "<<file.errorString();
        }
        QJsonParseError parse_error;
        QJsonDocument target = QJsonDocument::fromJson(file.readAll(), &parse_error);
        if(target.isEmpty())
            qDebug()<<"Error: TizFacebook: parse"<<meFile<<"failed with parse error:"<<parse_error.errorString()
                    <<"Offset:"<<parse_error.offset;
        else
            me = target;
        file.close();
    }

}

bool TizFacebook::testConnection()
{
    const QString fields = "id";
    QString id = get("/me", fields, timeOut).object()
            .value("id").toString();
    online = !id.isEmpty();
    return online;
}

void TizFacebook::createFbDir()
{
    if(!QDir(QDir::homePath() + facebookDir).exists())
        //qDebug()<<QDir::homePath()+facebookDir;
        QDir::home().mkdir(facebookDir);
}

void TizFacebook::setStdPicture(const QPixmap &picture)
{
    delete stdPicture;
    stdPicture = new QPixmap(picture);
}

bool TizFacebook::hasValidAccessToken() const
{
    if(!accessToken.isEmpty())
    {
        return true;
    }
    return false;
}
