Copyright (c) 2014 Christian Schabesberger


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

-------------------------------------------------------------------------
|This is the instruction to get this App or the TizFacebook SDK running.|
-------------------------------------------------------------------------

First things first. TizFacebook is a wrapper for the regular Facebook SDK for Android 
(current supported version: 3.16) and the Open Graph API (v. 2.0). 
Its made to use and connect to facebook with an Qt Android app, written in C++. 
TizFacebook only uses Facebook SDK for requesting longterm access-token in order to manage 
login and logout. The regular request is then done by the JSON based Open Graph API, 
using QNetworkManager, and the QJson classes. TizFacebook can be compiled for 
Android and PC. The PC version only provides rudimentary login functionality 
(it only requests shortterm access-token). The TizFacebook repository is not 
delivering the original Facebook SDK. So in order to get your App running you also 
need to download Facebook SDK for Android (v. 3.16).

---------------------------
|Getting this App running:|
---------------------------
This app requests the public_profile permission, and than simply displays the users profile image.

Preparation:
------------

1. First go to https://developers.facebook.com and create a new App.
2. Enter the apps settings. Click on “+ Add Platform”. Then choose “Android”.
3. - For Package Name enter: org.schabi.example.TizFbExample
   - For Class name enter: org.schabi.tiz.TizActivityFb
4. Create and store a Kay Hash:

   On Windows run the following command:
   keytool -exportcert -alias androiddebugkey -keystore %HOMEPATH%\.android\debug.keystore | openssl sha1 -binary | openssl base64

   on Linux and Mac OSX run:
   keytool -exportcert -alias androiddebugkey -keystore ~/.android/debug.keystore | openssl sha1 -binary | openssl base64

   You will be prompted for a password. If not something went wrong and you must check if 
   debug.keystore is present somewhere. If you where prompted for one just enter “android” as password. 
   Now copy the generated key and paste it into the “Key Hashes” field of your App Settings.
5. At last make sure “Single Sign On” is enabled, then save your changes.

Merge the Facebook SDK into TizFbExample project:
-------------------------------------------------

1. Download Facebook SDK for Android (v. 3.16) https://developers.facebook.com/docs/android/downloads
2. unpack the facebook-android-3.16 folder and enter it
3. enter the facebook folder
4. copy all .jar files from the libs directory into TizFbExample/android/libs
5. copy the content of the src directory into TizFbExample/android/src
6. from res copy every folder except the values folder into TizFbExample/android/res
7. now copy all files from res/values except strings.xml into TizFbExample/android/res/values
   the strings.xml file does not need to be changed it already contains all necessary
8. Change the location of the Android resource file in the Facebook SDK sourcecode:
   - open up the Qt Creator enter the TizFbExample project and press Strg + Shift + F
   - for scope select “Files on File System”
   - the phrase you search for is “com.facebook.android”
   - for the target directory enter <path_to_project>/TizFbExample/android/src/com/facebook
   - now click “Search & Replace”
9. At the field “Replace with:” enter “org.schabi.example.TizFbExample” and click “Replace”


Setting up TizFbExample App:
----------------------------

1. At file android/res/strings.xml in line 11:
   replace <app_id here> with the app ID witch is displayed at the dashboard 
   of your Facebook App.
2. At your apps dashboard go to settings and click there on the “Advanced” tab. 
   Now you should see a field called “Client Token”. Copy this token and replace 
   <client_token here> at file src/mainWindow.cpp in line 22.

Now if everything worked well you should be ready to compile and run TizFbExample.

------------------------------------------------
|Install TizFacebook SDK into your own project:|
------------------------------------------------

Perpetration:
-------------

1. You want go to https://developers.facebook.com and create a new App.
2. Enter the apps settings. Click on “+ Add Platform”. Then choose “Android”.
3. For Package Name enter the package name of your app
   For Class name enter: org.schabi.tiz.TizActivityFb 
4. Create a Kay Hash:

   on Windows run the following command:
   keytool -exportcert -alias androiddebugkey -keystore %HOMEPATH%\.android\debug.keystore | openssl sha1 -binary | openssl base64

   on Linux and Mac OSX run:
   keytool -exportcert -alias androiddebugkey -keystore ~/.android/debug.keystore | openssl sha1 -binary | openssl base64

   You will be prompted for a password. If not something went wrong you must check if debug.keystore 
   is present somewhere. If you where prompted for one just enter “android” as password. 
   Now copy the generated key and past it into the “Key Hashes” field of your App Settings.

5. At last make sure “Single Sign On” is enabled, then save your changes.

Merge the Facebook SDK into your app:
-------------------------------------

1. Download Facebook SDK for Android (v. 3.16) https://developers.facebook.com/docs/android/downloads
2. unpack the facebook-android-3.16 folder and enter it
3. enter the facebook folder
4. copy all .jar files from the libs directory into <your_project_root>/android/libs
5. copy the content of the src directory into <your_project_root>/android/src
6. from res copy every folder except the values folder into <your_project_root>/android/res
7. now copy all files from res/values except strings.xml into <your_app_root>/android/res/values
8. If you have a costumized strings.xml file copy the facebook stuff section from 
   TizFbExample/android/res/values/strings.xml to your own strings.xml. If you do not have a costumized 
   strings.xml simply copy the whole file into <your_app_root>/android/res/values.
10. At your now created strings.xml file you will find a token called “<string name="app_id"><app_id here></string>”. 
    Replace <app_id here> with the app id displayed on the dashboard of your facebook app.
9. Change the location of the Android resource file in the Facebook SDK sourcecode:	
        - open up the Qt Creator enter your project and press Strg + Shift + F
	- for scope select “Files on File System”
	- the phrase you search for is “com.facebook.android”
	- for the target directory enter <your_project_root>/android/src/com/facebook
	- now click “Search & Replace”
10. At the field “Replace with” put in the package name of your app and click “Replace”
11. From TizFbExample/android/AndroidManifest.xml copy the Facebook stuff section
    into the application section of your own AndroidManifest.xml

Merge this TizFacebook SDK into your project:
--------------------------------------------

1. Copy the org folder from TizFbExample/android/src to <your_project_root>/android/src
2. Change the main activity in your AndroidManifest.xml 
   from org.qtproject.qt5.android.bindings.QtActivity to org.schabi.tiz.TizActivityFb
3. Copy the tizFacebook folder into the root directory of your project
4. include tizFacebook/tizFacebook.pro into your qmake project file ( include(tizFacebook/tizFacebook.pri) ). 
   Now your app should be runnable


Preparing your code:
--------------------

1. Set the client Token of your app with setClientToken(QString). 
   You will find the client token at your apps dashbord under the tab “Advanced”
