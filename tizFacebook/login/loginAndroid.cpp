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

#ifdef __ANDROID__

#include "loginAndroid.hpp"

#include <QObject>
#include <QDebug>
#include <jni.h>
#include <QtAndroidExtras>
#include <QApplication>
#include <QTimer>
#include <QStringList>

const QString loginClassName(".TizActivityFb");
const QString packageName("org.schabi.tiz");

LoginSystem::LoginSystem(QObject *parent)
    :QObject(parent)
    ,expirationDate(QDateTime())
    ,grantedPermissions(QStringList())
    ,accessToken(QString())
    ,checkTimer(new QTimer(this))
    ,statusChecks(0)
    ,maxStatusChecks(10)
{
    checkTimer->setInterval(500);
    checkTimer->setSingleShot(false);
    connect(checkTimer, SIGNAL(timeout()),
            this, SLOT(checkState()));
    checkTimer->start();
}

void LoginSystem::login(const QString &cid,
                        const QString &ct, const QStringList &rp, const QStringList &pp)
{
    Q_UNUSED(cid)
    QString state = QAndroidJniObject::callStaticObjectMethod<jstring>(
                                                QString(packageName + loginClassName).toLatin1(),
                                                "getSessionState").toString();

    qDebug()<<"Session state:"<<state;

    if(state == "CLOSED_LOGIN_FAILED")
    {
        qDebug()<<"Error: CLOSED_LOGIN_FAILED, can not go on !!!";
        return;
    }

    if(state != "OPENED")
    {
        QString sr;
        for(int i = 0; i < rp.length(); i++)
        {
            sr.append(rp.at(i));
            if(i < rp.length()-1)
            {
                sr.append(",");
            }
        }
        qDebug()<<"readPermissions:"<<sr;

        QString sp;
        for(int i = 0; i < pp.length(); i++)
        {
            sp.append(pp.at(i));
            if(i < pp.length()-1)
            {
                sp.append(",");
            }
        }
        qDebug()<<"publishPermissions:"<<sp;

        QAndroidJniObject::callStaticMethod<void>(QString(packageName + loginClassName).toLatin1(),
                                                  "login",
                                                  "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
                                                  QAndroidJniObject::fromString(sr).object<jstring>(),
                                                  QAndroidJniObject::fromString(sp).object<jstring>(),
                                                  QAndroidJniObject::fromString(ct).object<jstring>());
        checkTimer->start();
    } else {
        privateGetAccessToken();
    }
}

void LoginSystem::checkState()
{
    qDebug()<<"fb_login: checking";
    QString sessionState = QAndroidJniObject::callStaticObjectMethod<jstring>(
                                                            QString(packageName + loginClassName).toLatin1(),
                                                            "getSessionState").toString();
    ProcessState processState = (ProcessState)QAndroidJniObject::callStaticMethod<int>(
                                                            QString(packageName + loginClassName).toLatin1(),
                                                            "getProcessState");
    qDebug()<<"fb_login: sessionState:"<<sessionState;
    qDebug()<<"fb_login: processStateCode:"<<processState;

    if(sessionState == "CREATED")
    {
        if(statusChecks > maxStatusChecks)
        {
            checkTimer->stop();
            statusChecks = 0;
            qDebug()<<"Checket"<<maxStatusChecks<<"times. User seams not to be loged in";
        } else
            statusChecks++;
        return;
    }

    if(sessionState == "CLOSED_LOGIN_FAILED")
    {
        qDebug()<<"Error: CLOSED_LOGIN_FAILED, can not go on !!!";
        checkTimer->stop();
        return;
    }

    if(sessionState.isEmpty())
    {
        qDebug()<<"Abord: empty Session State string received";
        checkTimer->stop();
        return;
    }

    if((sessionState != "OPENED" && sessionState != "OPENING") &&
            sessionState != "OPENED_TOKEN_UPDATED")
    {
        qDebug()<<"Error: Session state is neither OPENED nor OPENING nor OPENED_TOKEN_UPDATED";
        checkTimer->stop();
        return;
    }

    if((sessionState == "OPENED" || sessionState == "OPENED_TOKEN_UPDATED")
            && processState == loged_in)
        privateGetAccessToken();
}

void LoginSystem::privateGetAccessToken()
{
    checkTimer->stop();

    QAndroidJniObject aT;
    accessToken = QAndroidJniObject::callStaticObjectMethod<jstring>(
                QString(packageName + loginClassName).toLatin1(),
                "getAccessToken").toString();
    if(!accessToken.isEmpty())
    {
        expirationDate = QDateTime::fromMSecsSinceEpoch(QAndroidJniObject::callStaticMethod<jlong>(
                    QString(packageName + loginClassName).toLatin1(),
                    "getExpirationDate"));
        grantedPermissions = QAndroidJniObject::callStaticObjectMethod<jstring>(
                        QString(packageName + loginClassName).toLatin1(),
                        "getGrantedPermissions").toString().split(";");
        qDebug()<<"AccessToken data received on C++ side";
        //qDebug()<<accessToken;  //ony for debuging
        qDebug()<<"AccessToken ExpirationDate:"<<expirationDate;
        emit accessTokenReceived();
    }
}

void LoginSystem::logout()
{
    QAndroidJniObject::callStaticMethod<void>(
                QString(packageName + loginClassName).toLatin1(),
                "logout");
}

#endif
