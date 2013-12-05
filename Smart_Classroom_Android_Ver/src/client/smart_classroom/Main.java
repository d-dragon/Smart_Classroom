package client.smart_classroom;

import network.Socket_ini.Socket_ini;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TabHost;
import android.widget.Toast;

public class Main extends Activity implements OnClickListener {

	private Socket_ini connector;
	private shareNetwork shared;
	Button btStart, btAuto, btPre, btMan, btOff, btEqu, btPro;
	EditText etCommand;
	String comamnd;
	TabHost tabs;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		shared = (shareNetwork) getApplicationContext();
		connector = shared.getNetworkSocket_TCP();
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
		btEqu = (Button) findViewById(R.id.btEqu);
		btEqu.setOnClickListener(this);
		btPro = (Button) findViewById(R.id.btPro);
		btPro.setOnClickListener(this);

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
		connector.SendCommand("exit");
		connector.closeSocket();

	}

	@Override
	protected void onPause() {
		super.onPause();
		connector.closeSocket();
	}

	@SuppressLint("ShowToast")
	@Override
	public void onClick(View view) {
		// TODO Auto-generated method stub
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
		case R.id.btEqu:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("Equipment");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case R.id.btPro:
			if (connector.ismIsConnected()) {
				try {
					connector.SendCommand("Projector");
				} catch (Exception e) {
					// TODO: handle exception
					Toast.makeText(getApplicationContext(),
							"Send command error", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		}
	}
}
