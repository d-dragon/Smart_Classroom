package client.smart_classroom;

import client.smart_classroom.R;
import client.smart_classroom.loading_Task.LoadingTaskFinishedListener;
import network.Socket_ini.*;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class Splashctivity extends Activity implements OnClickListener {

	private Socket_ini connector;
	private Socket_UDP conSocket_UDP = null;
	private shareNetwork shared;
	private Button btnConnect, btP1;
	// private ProgressBar pb;
	// private TextView tv;

	final String P1 = "P1";
	private static final int WAIT_TIME = 1000;

	public EditText etIP, etPort;
	public String IP = " ";
	public int Port = 1991;

	// Debugging
	private static final String TAG = "BcastChat";
	private static final boolean D = true;

	// Message types sent from the BluetoothChatService Handler
	public static final int MESSAGE_READ = 1;
	public static final int MESSAGE_TOAST = 2;

	// Key names received from the BroadcastChatService Handler
	public static final String TOAST = "toast";

	// The Handler that gets information back from the BluetoothChatService
	private final Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {

			if (D)
				Log.e(TAG, "[handleMessage !!!!!!!!!!!! ]");

			switch (msg.what) {

			case MESSAGE_READ:
				String readBuf = (String) msg.obj;
				IP = readBuf;
				// etIP.setText(IP);
				etPort.setText("1991");
				break;
			case MESSAGE_TOAST:
				Toast.makeText(getApplicationContext(),
						msg.getData().getString(TOAST), Toast.LENGTH_SHORT)
						.show();
				break;
			}
		}
	};

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (D)
			Log.e(TAG, "+++ ON CREATE +++");
		// Show the splash screen
		setContentView(R.layout.loading);

		shared = (shareNetwork) getApplicationContext();
		connector = shared.getNetworkSocket_TCP();
		conSocket_UDP = shared.getNetworkInSocket_UDP();
		etIP = (EditText) findViewById(R.id.etIP);
		etPort = (EditText) findViewById(R.id.etPort);
		btP1 = (Button) findViewById(R.id.btP1);
		btP1.setOnClickListener(this);

		btnConnect = (Button) findViewById(R.id.btnC);
		btnConnect.setOnClickListener(this);
		btnConnect.setVisibility(View.VISIBLE);

		// pb = (ProgressBar) findViewById(R.id.pbLoading);

		// tv = (TextView) findViewById(R.id.tvSplash);
		// new loading_Task(this, getApplicationContext()).execute("abc");

		// Pass in whatever you need a url is just an example we don't use it in
		// this tutorial
		/**
		 * Disable StrictMode
		 */
		if (android.os.Build.VERSION.SDK_INT > 9) {
			StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder()
					.permitAll().build();
			StrictMode.setThreadPolicy(policy);
		}

	}

	public void onStart() {
		super.onStart();
		if (D)
			Log.e(TAG, "++ ON START ++");

		setup();

	}

	@Override
	public synchronized void onResume() {
		super.onResume();
		if (D)
			Log.e(TAG, "+ ON RESUME +");
		conSocket_UDP.start();

		// connector_UDP.start();
	}

	public void onStop() {
		super.onStop();
		if (conSocket_UDP != null)
			conSocket_UDP.stop();
		if (D)
			Log.e(TAG, "-- ON STOP --");
	}

	public void onDestroy() {
		super.onDestroy();
		// Stop the Broadcast chat services
		if (conSocket_UDP != null)
			conSocket_UDP.stop();
		if (D)
			Log.e(TAG, "--- ON DESTROY ---");
	}

	// This is the callback for when your async task has finished
	/*
	 * @Override public void onTaskFinished() { completeSplash(); }
	 * 
	 * private void completeSplash() { if (connector.ismIsConnected()) {
	 * startApp(); finish(); // Don't forget to finish this Splash Activity so
	 * the user // can't return to it! } else {
	 * Toast.makeText(getApplicationContext(),
	 * "Connection Fail! Please press 'Connect' again",
	 * Toast.LENGTH_SHORT).show(); pb.setVisibility(View.INVISIBLE);
	 * btnConnect.setVisibility(View.VISIBLE); tv.setVisibility(View.INVISIBLE);
	 * } }
	 */

	/*
	 * private void startApp() { Intent intent = new Intent(Splashctivity.this,
	 * Main.class);// MainActivity.class startActivity(intent); }
	 */

	@Override
	public void onClick(View v) {

		switch (v.getId()) {
		case R.id.btnC:
			// IP = etIP.getText().toString();
			try {
				Port = Integer.parseInt(etPort.getText().toString());
			} catch (NumberFormatException e) {
				// TODO: handle exception
				Toast.makeText(getApplicationContext(),
						"Please Enter only interger type in Port Section",
						Toast.LENGTH_SHORT).show();
			}
			connector.setIp(IP);
			connector.setPort(Port);
			// pb.setVisibility(View.VISIBLE);
			// tv.setVisibility(View.VISIBLE);
			btnConnect.setVisibility(View.INVISIBLE);
			connector.connectToNetwork();
			new Handler().postDelayed(new Runnable() {
				@Override
				public void run() {

					// Simulating a long running task
					try {
						Thread.sleep(2000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}

					/*
					 * Create an Intent that will start the
					 * ProfileData-Activity.
					 */

				}
			}, WAIT_TIME);
			if (connector.ismIsConnected()) {
				Intent mainIntent = new Intent(Splashctivity.this, Main.class);
				Splashctivity.this.startActivity(mainIntent);
				Splashctivity.this.finish();
			} else {
				btnConnect.setVisibility(View.VISIBLE);

			}

			break;
		case R.id.btP1:

			try {
				sendMessage(P1);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			break;
		}
	}

	private void setup() {

		conSocket_UDP = new Socket_UDP(this, mHandler);
	}

	/**
	 * Sends a message.
	 * 
	 * @param message
	 *            A string of text to send.
	 */
	private void sendMessage(String message) {

		if (D)
			Log.e(TAG, "[sendMessage]");

		// Check that there's actually something to send
		if (message.length() > 0) {
			// Get the message bytes and tell the BluetoothChatService to write
			byte[] send = message.getBytes();

			conSocket_UDP.write(send);

		}
	}
}