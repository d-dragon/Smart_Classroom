package com.smartclassroom.main;

import java.util.ArrayList;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;
import client.smart_classroom.R;

import com.smartclassroom.adapter.ListAdapter;
import com.smartclassroom.common.CommonUtils;
import com.smartclassroom.common.Logger;
import com.smartclassroom.common.Settings;
import com.smartclassroom.listener.OnEventControlListener;
import com.smartclassroom.network.SocketTCP;
import com.smartclassroom.network.SocketUDP;

public class ConnectActivity extends BaseActivity implements OnClickListener {

	private SocketTCP socketTCP;
	private SocketUDP socketUDP = null;
	// private SmartClassroomApplication shared;
	private Button btnConnect, btP1, btnMenu;
	private ListView listView;
	private ListAdapter listRoomAdapter;
	// private ProgressBar pb;
	// private TextView tv;

	private ArrayList<Room> roomList;
	final String P1 = "P1";

	public EditText edtIP, edtPort;
	public String IP = "", readBuf;
	public int Port = 1991, temp;

	// Debugging
	private static final String TAG = "BcastChat";

	// Message types sent from the BluetoothChatService Handler
	public static final int MESSAGE_READ = 1;
	public static final int MESSAGE_TOAST = 2;

	// Key names received from the BroadcastChatService Handler
	public static final String TOAST = "toast";

	// The Handler that gets information back from the BluetoothChatService
	// @SuppressLint("HandlerLeak")
	// public final Handler mHandler = new Handler() {
	// @Override
	// public void handleMessage(Message msg) {
	//
	// if (Settings.DEBUGGABLE)
	// Log.e(TAG, "[handleMessage !!!!!!!!!!!! ]");
	//
	// switch (msg.what) {
	//
	// case MESSAGE_READ:
	// readBuf = (String) msg.obj.toString();
	// IP = readBuf.trim();
	// // temp = IP.length();
	//
	// etIP.setText(IP);
	// etPort.setText("1991");
	// break;
	// case MESSAGE_TOAST:
	// Toast.makeText(getApplicationContext(),
	// msg.getData().getString(TOAST), Toast.LENGTH_SHORT)
	// .show();
	// break;
	// }
	// }
	// };

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		roomList = new ArrayList<Room>();
		Logger.show(TAG, "+++ ON CREATE +++");
		// Show the splash screen
		setContentView(R.layout.connect_layout);
		View view = findViewById(R.id.titleBar);
		btnMenu = (Button) view.findViewById(R.id.btnMenu);
		btnMenu.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				btnMenuClick();
			}
		});

		socketTCP = SmartClassroomApplication.getInstance()
				.getNetworkSocket_TCP();
		socketTCP.setOnEventControlListener(this);
		socketUDP = SmartClassroomApplication.getInstance()
				.getNetworkInSocket_UDP();
		socketUDP.setOnEventControlListener(this);

		edtIP = (EditText) findViewById(R.id.etIP);
		edtPort = (EditText) findViewById(R.id.etPort);
		btP1 = (Button) findViewById(R.id.btP1);
		btP1.setOnClickListener(this);

		btnConnect = (Button) findViewById(R.id.btnC);
		btnConnect.setOnClickListener(this);
		btnConnect.setVisibility(View.VISIBLE);
		
		listView = (ListView) findViewById(R.id.lstvRoom);
		
	}

	public void onStart() {
		super.onStart();
		Logger.show(TAG, "++ ON START ++");
		// setup();
	}

	@Override
	public synchronized void onResume() {
		super.onResume();
		Logger.show(TAG, "+ ON RESUME +");
		socketUDP.start();
		// connector_UDP.start();
	}

	public void onStop() {
		super.onStop();
		if (socketUDP == null) {
			return;
		}
		socketUDP.stop();
		Logger.show(TAG, "-- ON STOP --");
	}

	public void onDestroy() {
		super.onDestroy();
		// Stop the Broadcast chat services
		if (socketUDP == null) {
			return;
		}
		socketUDP.stop();
		Logger.show(TAG, "--- ON DESTROY ---");
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

	public void btnMenuClick() {
		if (edtIP.getVisibility() == View.VISIBLE
				|| edtPort.getVisibility() == View.VISIBLE) {
			edtIP.setVisibility(View.GONE);
			edtPort.setVisibility(View.GONE);
		} else {
			edtIP.setVisibility(View.VISIBLE);
			edtPort.setVisibility(View.VISIBLE);
		}
	}

	@Override
	public void onClick(View v) {

		switch (v.getId()) {
		case R.id.btnC:
			if (Settings.DEBUGGABLE) {
				IP = CommonUtils.toString(edtIP.getText());
				try {
					Port = Integer.parseInt(CommonUtils.toString(edtPort
							.getText()));
				} catch (NumberFormatException e) {
					Toast.makeText(this,
							"Please Enter only interger type in Port Section",
							Toast.LENGTH_SHORT).show();
				}
			}
			socketTCP.setIp(IP);
			socketTCP.setPort(Port);
			socketTCP.start();
			break;
		case R.id.btP1:
			roomList = new ArrayList<Room>();
			listRoomAdapter = new ListAdapter(roomList, this);
			listView.setAdapter(listRoomAdapter);
			try {
				sendMessage(P1);
			} catch (Exception e) {
				e.printStackTrace();
			}

			break;
		}
	}

	// private void setup() {
	// conSocket_UDP = new Socket_UDP(this, mHandler);
	// }

	/**
	 * Sends a message.
	 * 
	 * @param msg
	 *            A string of text to send.
	 */
	private void sendMessage(String msg) {
		if (msg.isEmpty()) {
			return;
		}
		Logger.show(TAG, "[sendMessage]");
		// Get the message bytes and tell the BluetoothChatService to write
		byte[] sendingBytes = msg.getBytes();
		Logger.show("btn R1 send:" + msg);
		socketUDP.sendMessage(sendingBytes);
	}

	@Override
	public void onEvent(View view, int type, Object data) {
		String msg = null;
		Room room = null;
		switch (type) {
		case OnEventControlListener.EVENT_UDP_MESSAGE:
			room = new Room();
			msg = (String) data;
			// readBuf = (String) msg.obj.toString();
			IP = msg.trim();
			room.setIp(IP);
			room.setPort(Port);
			room.setName(P1);
			roomList.add(room);
			
			Logger.show(IP + Port);
			// Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
			// temp = IP.length();
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					edtIP.setText(IP);
					edtPort.setText("1991");
					listRoomAdapter.notifyDataSetChanged();
					// btnConnect.setVisibility(View.VISIBLE);
				}
			});

			break;

		case OnEventControlListener.EVENT_TCP_STATUS:
			boolean isConnected = (Boolean) data;
			if (isConnected) {
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Intent mainIntent = new Intent(ConnectActivity.this,
								ControlActivity.class);
						ConnectActivity.this.startActivity(mainIntent);
						ConnectActivity.this.finish();
					}
				});

			} else {
				// btnConnect.setVisibility(View.VISIBLE);
				Toast.makeText(getApplicationContext(),
						"Connecting Fail! Try again", Toast.LENGTH_SHORT)
						.show();
			}
			break;
		default:
			break;
		}
	}

}