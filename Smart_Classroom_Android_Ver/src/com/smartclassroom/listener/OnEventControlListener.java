package com.smartclassroom.listener;

import android.view.View;

/**
 * Custom event listener
 * @author DUYPT
 *
 */
public interface OnEventControlListener {

	public static final int EVENT_TCP_MESSAGE = 1;
	public static final int EVENT_UDP_MESSAGE = EVENT_TCP_MESSAGE + 1;
	public static final int EVENT_TCP_STATUS = EVENT_UDP_MESSAGE + 1;
	
	void onEvent(View view, int type, Object data);
}
