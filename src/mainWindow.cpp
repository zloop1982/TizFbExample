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

#include "mainWindow.hpp"
#include "ui_mainWindow.h"

#include "tizFacebook/tizFacebook.hpp"

#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tizFacebook(new TizFacebook(this))
{
    ui->setupUi(this);
    connect(ui->loginButton, SIGNAL(clicked()),
            this, SLOT(onLoginButton()));
    connect(tizFacebook, SIGNAL(loginChanged(bool,bool)),
            this, SLOT(onLoginStateChanged(bool, bool)));

    tizFacebook->setFacebookDir(QString("facebook"));
    tizFacebook->setClientToken("<client-token here>");

    // optional for login on pc
    //tizFacebook->setClientId("<app_id here>");

    QStringList readPermissions("public_profile");
    QStringList publishPermisssions("publish_actions");
    tizFacebook->setReadPermissions(readPermissions);
    //tizFacebook->setPublishPermissions(publishPermisssions);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoginButton()
{
    if(!tizFacebook->isLoggedIn())
        tizFacebook->login();
    else
        tizFacebook->logout();
}

void MainWindow::onLoginStateChanged(bool logedIn, bool online)
{
    if(logedIn)
    {
        ui->loginButton->setText("logout");

        //this line adds ofline access to user data
        tizFacebook->requestMe(false, true);

        ui->usrImgLbl->setPixmap(tizFacebook->getPicture());
        ui->usrTextLbl->setText(tizFacebook->getName());
    } else {
        ui->loginButton->setText("login");

        ui->usrImgLbl->setPixmap(QPixmap());
        ui->usrTextLbl->setText("");
    }
}
