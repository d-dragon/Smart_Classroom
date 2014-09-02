package com.smartclassroom.common;

import android.util.Log;

/**
 * Log info for debugging purpose
 * @author DuyPT
 *
 */
public class Logger {

	public static final String TAG = "SmartClassroom";

	public static void show(String msg) {
		if (!Settings.DEBUGGABLE) {
			return;
		}
		Log.v(TAG, msg);
	}

	public static void show(String tag, String msg) {
		if (!Settings.DEBUGGABLE) {
			return;
		}
		Log.v(tag, msg);
	}
	
	public static void show(String tag, String msg, Throwable throwable) {
		if (!Settings.DEBUGGABLE) {
			return;
		}
		Log.v(tag, msg, throwable);
	}
}
