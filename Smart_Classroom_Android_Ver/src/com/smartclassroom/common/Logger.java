package com.smartclassroom.common;

import android.util.Log;

public class Logger {

	public static final String TAG = "SmartClassroom";

	public static void show(String msg) {
		if (!Settings.DEBUGGABLE) {
			return;
		}
		Log.v(TAG, msg);
	}
}
