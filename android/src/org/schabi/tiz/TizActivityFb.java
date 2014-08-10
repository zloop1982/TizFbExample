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

package org.schabi.tiz;

import java.util.Date;
import java.util.Arrays;
import java.util.List;

import android.os.Bundle;
import android.content.Intent;
import android.util.Log;
import android.app.Activity;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.QtNative;

import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.LoggingBehavior;
import com.facebook.Settings;



public class TizActivityFb extends QtActivity {
    final static private String msgTag = "TizActivityFb";
    private static List<String> publishPermissions;
    private static List<String> readPermissions;

    private static boolean logedIn = false;
    private static boolean readingPermissionsRequested = false;
    private static boolean readingPermissionsGranted = false;
    private static boolean publishPermissionsRequested = false;
    private static boolean publishPermissionsGranted = false;
    private static boolean calledLogin = false;

    private static Session.StatusCallback statusCallback = new Session.StatusCallback() {
        @Override
        public void call(Session session, SessionState state, Exception exception) {
            Log.d(msgTag, "status callback: " + state.toString());
            if(state.isOpened()) {
                Log.d(msgTag, "Session is opened :)");

                if(publishPermissionsRequested == false) {
                    if(calledLogin == false) {
                        logedIn = true;         //this will be callen when
                                                //accessToken is loaded from cache
                    } else {
                        Log.d(msgTag, "read Permissions now available:");
                        List<String> permissions = session.getPermissions();
                        for(String p : permissions) {
                            Log.d(msgTag, p);
                        }
                        if(!publishPermissions.isEmpty()) {
                            requestPublicPermissions();
                        } else {
                            logedIn = true;     //this will be callen when
                                                //no publish Permissions are set
                            Log.d(msgTag, "no publish permissions requestet");
                       }
                    }
                } else {
                    publishPermissionsGranted = true;
                    logedIn = true;             //this will be callen when
                                                //basic permissions are granted
                    Log.d(msgTag, "read/publish Permissions now available:");
                    List<String> permissions = session.getPermissions();
                    for(String p : permissions) {
                        Log.d(msgTag, p);
                    }
                    //Log.d(msgTag, "AccessToken: " + session.getAccessToken());
                }

            } else if(state.toString().equals("CLOSED_LOGIN_FAILED")) {
                Log.e(msgTag, "Error while requesting accessToken: CLOSED_LOGIN_FAILED:");
                exception.printStackTrace();
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Settings.addLoggingBehavior(LoggingBehavior.INCLUDE_ACCESS_TOKENS);


        Session session = Session.getActiveSession();
        if (session == null) {
            Log.d(msgTag, "Session is null");
            if (savedInstanceState != null) {
                Log.d(msgTag, "restore form savedInstacneState");
                session = Session.restoreSession(this, null, statusCallback, savedInstanceState);
            }
            if (session == null) {
                Log.d(msgTag, "Session still is null");
                session = new Session(this);
            }
            Session.setActiveSession(session);

            if (session.getState().equals(SessionState.CREATED_TOKEN_LOADED)) {
                Log.d(msgTag, "open session for read");
                session.openForRead(new Session.OpenRequest(this).setCallback(statusCallback));
            }
        }
    }

    //facebook login stuff comunication

    static public void login(String rp, String pp, String ct) {
        try {
            readingPermissionsRequested = true;
            calledLogin = true;
            Log.d(msgTag, "login");
            Activity mainActivity = QtNative.activity();
            //Session session = Session.getActiveSession();

            Settings.setClientToken(ct);

            //prepare permissions
            readPermissions = Arrays.asList(rp.split(","));
            for(int i = 0; i < readPermissions.size(); i++) {
                Log.d(msgTag, "readPermisson: " + readPermissions.get(i));
            }

            publishPermissions = Arrays.asList(pp.split(","));
            for(int i = 0; i < publishPermissions.size(); i++) {
                Log.d(msgTag, "publishPermission: " + publishPermissions.get(i));
            }

            if(readPermissions.isEmpty()) {
                Session.openActiveSession(mainActivity, true, statusCallback);
            } else {
                Session.openActiveSession(mainActivity, true, readPermissions, statusCallback);
            }

        } catch (Exception e) {
            Log.e(msgTag, "Error cought at login()");
            e.printStackTrace();
        }
    }

    public static void requestPublicPermissions() {
        publishPermissionsRequested = true;
        Activity mainActivity = QtNative.activity();
        Session activeSession = Session.getActiveSession();
        Session.NewPermissionsRequest newPermissionsRequest = new Session.NewPermissionsRequest(
            mainActivity, publishPermissions);
        activeSession.requestNewPublishPermissions(newPermissionsRequest);
    }


    @Override
    public void onStart() {
        super.onStart();
        Session.getActiveSession().addCallback(statusCallback);
    }

    @Override
    public void onStop() {
        super.onStop();
        Session.getActiveSession().removeCallback(statusCallback);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(msgTag, "Acitity result received");
        Session.getActiveSession().onActivityResult(this, requestCode, resultCode, data);
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        Log.d(msgTag, "save instance");
        Session session = Session.getActiveSession();
        Session.saveSession(session, outState);
    }

    static public void logout() {
        Log.d(msgTag, "logout");
        Session session = Session.getActiveSession();
        session.closeAndClearTokenInformation();
    }

    static public String getAccessToken() {
        String accessToken = Session.getActiveSession().getAccessToken();
        return accessToken;
    }


    static public long getExpirationDate() {
        Date expirationDate = Session.getActiveSession().getExpirationDate();
        //Log.d(msgTag, expirationDate.toString());
        return expirationDate.getTime();
    }

    static public String getSessionState() {
        Session session = Session.getActiveSession();
        return session.getState().toString();
    }

    static public int getProcessState() {
        if(logedIn)
            return 6;       //loged_in
        if(publishPermissionsGranted)
            return 4;       //publish_permissions_granted
        if(publishPermissionsRequested)
            return 3;       //publish_permissions_requested
        if(readingPermissionsGranted)
            return 2;       //reading_permissions_granted
        if(readingPermissionsRequested)
            return 1;       //reading_permissions_requested
        if(!calledLogin)
            return 5;       //before_login

        return 0;
    }

    static public String getGrantedPermissions() {
        Session session = Session.getActiveSession();
        List<String> permissions = session.getPermissions();
        String permissionsString = "";
        for(int i = 0; i < permissions.size(); i++) {
            permissionsString += permissions.get(i);
            if(i != permissions.size()-1) {
                permissionsString += ";";
            }
        }
        //Log.d(msgTag, permissionsString);
        return permissionsString;
    }
}
