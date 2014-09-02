package com.smartclassroom.network;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

import android.content.Context;
import android.os.Handler;

import com.smartclassroom.common.Constants;
import com.smartclassroom.listener.OnEventControlListener;

public class SocketTCP {
	private boolean mIsConnected;
	private String ip;
	private int port;
	Handler mHandler = null;
	Socket mSocket = null;
	Thread mThread = null;
	ReceiveThread receiveThread;
	public String serverMessage;
	private boolean isRunning = true;

	private OnEventControlListener onEventControlListener;

	public SocketTCP(Context context) {
		this.mHandler = new Handler();

		makeDefault();
	}

	public OnEventControlListener getOnEventControlListener() {
		return onEventControlListener;
	}

	public void setOnEventControlListener(OnEventControlListener listener) {
		this.onEventControlListener = listener;
	}

	public class ReceiveThread extends Thread {
		public void run() {
			try {
				Socket socket = new Socket(ip, port);
				mSocket = socket;
				mIsConnected = true;
				onEventControlListener.onEvent(null,
						OnEventControlListener.EVENT_TCP_STATUS, mIsConnected);
				String msg = null;
				while (isRunning()) {
					msg = ReceiveData();
					onEventControlListener.onEvent(null,
							OnEventControlListener.EVENT_TCP_MESSAGE, msg);
				}

			} catch (Exception e) {
				e.printStackTrace();
				mIsConnected = false;
				onEventControlListener.onEvent(null,
						OnEventControlListener.EVENT_TCP_STATUS, mIsConnected);
			}
		}

	}

	public void stop() {
		closeSocket();
		stopThread();
	}

	public void networkDestroy(int whatToDis) {
		if (mIsConnected) {
			mIsConnected = false;
			switch (whatToDis) {
			case 0:
				try {
					if (this.mSocket != null) {
						this.mSocket.close();
						this.mSocket = null;
					}
				} catch (Exception e) {
					e.printStackTrace();
				}

			case 1:
				try {
					if (this.mThread != null) {
						Thread threadHelper = this.mThread;
						this.mThread = null;
						threadHelper.interrupt();
					}
				} catch (Exception e1) {
				}
				break;
			}

		}
	}

	public void start() {
		receiveThread = new ReceiveThread();
		receiveThread.start();
	}

	public void closeSocket() {
		if (mSocket == null) {
			return;
		}
		try {
			mSocket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void stopThread(){
		if(receiveThread == null){
			return;
		}
		setRunning(false);
	}

	public void makeDefault() {
		ip = Constants.IP;
		port = Constants.PORT;
	}

	/**
	 * Send message to server
	 * 
	 * @param str
	 */
	public void SendCommand(String str) {
		PrintWriter out = null;
		BufferedWriter bufferedWriter = null;
		OutputStreamWriter outputStreamWriter = null;
		OutputStream outputStream = null;
		try {
			outputStream = mSocket.getOutputStream();
			outputStreamWriter = new OutputStreamWriter(outputStream);
			bufferedWriter = new BufferedWriter(outputStreamWriter);
			out = new PrintWriter(bufferedWriter, true);
			out.println(str);
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	/**
	 * Receive message from server
	 * 
	 * @return
	 */
	public String ReceiveData() {
		BufferedReader in = null;
		InputStreamReader inputStreamReader = null;
		InputStream inputStream = null;
		try {
			inputStream = mSocket.getInputStream();
			inputStreamReader = new InputStreamReader(inputStream);
			in = new BufferedReader(inputStreamReader);
			serverMessage = in.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return serverMessage;
	}

	public boolean isRunning() {
		return isRunning;
	}

	public void setRunning(boolean isRunning) {
		this.isRunning = isRunning;
	}

	public boolean ismIsConnected() {
		return mIsConnected;
	}

	public void setIp(String ip) {
		this.ip = ip;
	}

	public void setPort(int port) {
		this.port = port;
	}
}