package com.smartclassroom.main;

import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;
import client.smart_classroom.R;

import com.smartclassroom.listener.OnEventControlListener;
import com.smartclassroom.network.SocketTCP;

public class ControlActivity extends BaseActivity implements OnClickListener {

	private SocketTCP connector;
	// private SmartClassroomApplication shared;
	Button btStart, btAuto, btPre, btMan, btOff, btEqu1, btEqu2, btPro1,
			btPro2, btPro3, btPro4;
	EditText etCommand;
	TextView tvTemperature, light;
	String comamnd;
	TabHost tabs;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.control_layout);

		// shared = (SmartClassroomApplication) getApplicationContext();
		// connector = shared.getNetworkSocket_TCP();
		connector = SmartClassroomApplication.getInstance()
				.getNetworkSocket_TCP();
		connector.setOnEventControlListener(this);
		tvTemperature = (TextView) findViewById(R.id.tvTemp);
		btStart = (Button) findViewById(R.id.btStart);
		btStart.setOnClickListener(this);
		btAuto = (Button) findViewById(R.id.btAuto);
		btAuto.setOnClickListener(this);
		btPre = (Button) findViewById(R.id.btPre);
		btPre.setOnClickListener(this);
		btMan = (Button) findViewById(R.id.btMan);
		btMan.setOnClickListener(this);
		btOff = (Button) findViewById(R.id.btOff);
		btOff.setOnClickListener(this);
		btEqu1 = (Button) findViewById(R.id.btEqu1);
		btEqu1.setOnClickListener(this);
		btEqu2 = (Button) findViewById(R.id.btEqu2);
		btEqu2.setOnClickListener(this);
		btPro1 = (Button) findViewById(R.id.btPro1);
		btPro1.setOnClickListener(this);
		btPro2 = (Button) findViewById(R.id.btPro2);
		btPro2.setOnClickListener(this);
		btPro3 = (Button) findViewById(R.id.btPro3);
		btPro3.setOnClickListener(this);
		btPro4 = (Button) findViewById(R.id.btPro4);
		btPro4.setOnClickListener(this);

		tabs = (TabHost) findViewById(R.id.tabHost);
		tabs.setup();
		TabHost.TabSpec tab1 = tabs.newTabSpec("TabFirst");
		tab1.setIndicator("Equipment");
		tab1.setContent(R.id.tab1);

		TabHost.TabSpec tab2 = tabs.newTabSpec("TabSecond");
		tab2.setIndicator("Projector");
		tab2.setContent(R.id.tab2);

		tabs.addTab(tab1);
		tabs.addTab(tab2);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	protected void onStop() {
		super.onStop();
		// connector.SendCommand("exit");
//		 connector.closeSocket();
//		connector.networkDestroy(0);
		connector.stop();

	}

	@Override
	protected void onPause() {
		super.onPause();
		// connector.closeSocket();
	}

	@Override
	public void onClick(View view) {
		switch (view.getId()) {
		case R.id.btStart:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("STA");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}

			}
			break;
		case R.id.btAuto:
			if (connector.ismIsConnected()) {

				try {
					connector.SendCommand("AUT");

				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
				// String serverMessage = connector.ReceiveData();
				// Toast.makeText(getApplicationContext(), serverMessage,
				// Toast.LENGTH_SHORT).show();
				// serverMessage = "";
			}
			break;
		case R.id.btPre:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("PRE");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btMan:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("MAN");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}

			}
			break;
		case R.id.btOff:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("OFF");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btEqu1:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("E1");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btEqu2:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("E2");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btPro1:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("1");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btPro2:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("2");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btPro3:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("3");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btPro4:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("4");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		}
	}

	/**
	 * Receive message here
	 * 
	 * @author PTD
	 */
	@Override
	public void onEvent(View view, int type, Object data) {
		String msg = null;
		switch (type) {
		case OnEventControlListener.EVENT_TCP_MESSAGE:
			msg = (String) data;
			final String strMsg = msg;
			Log.i("MESSAGE", msg);
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					tvTemperature.setText(strMsg);
				}
			});
			break;

		default:
			break;
		}

	}
}
