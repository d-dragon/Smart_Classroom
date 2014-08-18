package com.smartclassroom.listener;

import android.view.View;

/**
 * Custom event listener
 * @author DUYPT
 *
 */
public interface OnEventControlListener {

	public static final int EVENT_TCP_MESSAGE = 1;
	
	void onEvent(View view, int type, Object data);
}
