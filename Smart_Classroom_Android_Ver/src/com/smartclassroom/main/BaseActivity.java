package com.smartclassroom.main;

import android.app.Activity;
import android.view.View;

import com.smartclassroom.listener.OnEventControlListener;

public class BaseActivity extends Activity implements OnEventControlListener {

	@Override
	public void onEvent(View view, int type, Object data) {

	}

}
