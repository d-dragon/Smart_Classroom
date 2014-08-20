package com.smartclassroom.network;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

import android.content.Context;
import android.os.Handler;

import com.smartclassroom.common.Constants;
import com.smartclassroom.common.Logger;
import com.smartclassroom.listener.OnEventControlListener;

public class Socket_ini {
	private boolean mIsConnected;
	private String ip;
	private int port;
	Handler mHandler = null;
	Socket mSocket = null;
	Thread mThread = null;
	ClientThread newThread;
	public String serverMessage;

	private OnEventControlListener onEventControlListener;

	public Socket_ini(Context context) {
		this.mHandler = new Handler();

		makeDefault();
	}

	public OnEventControlListener getOnEventControlListener() {
		return onEventControlListener;
	}

	public void setOnEventControlListener(
			OnEventControlListener onEventControlListener) {
		this.onEventControlListener = onEventControlListener;
	}

	public class ClientThread extends Thread {
		public void run() {
			try {
				Socket socket = new Socket(ip, port);
				mSocket = socket;
				mIsConnected = true;
				String msg = null;
				while (true) {
					msg = ReceiveData();
					onEventControlListener.onEvent(null,
							OnEventControlListener.EVENT_TCP_MESSAGE, msg);
				}

			} catch (Exception e) {
				e.printStackTrace();
				mIsConnected = false;
				Logger.show(e.getMessage());
				// String strException = e0.getMessage();
				// if (strException == null)
				// strException = "Connection closed";
				// else
				// strException = "Cannot connect to the server:\r\n"
				// + strException;
			}
		}

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

	public void connectToNetwork() {
		newThread = new ClientThread();
		newThread.start();
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

	public void makeDefault() {
		ip = Constants.IP;
		port = Constants.PORT;
	}

	public ClientThread getNewThread() {
		return newThread;
	}

	/*
	 * public void PostCommand(String strCommand) { if (mSocket != null &&
	 * mIsConnected) {
	 * 
	 * try { OutputStream streamOutput = mSocket.getOutputStream();
	 * 
	 * try { byte[] arrayOutput = strCommand.getBytes(); int nLen =
	 * arrayOutput.length; streamOutput.write(arrayOutput, 0, nLen); } catch
	 * (Exception e0) { // final String strMessage = //
	 * "Error while sending to server:\r\n" + // e0.getMessage(); }
	 * 
	 * } catch (IOException e1) { e1.printStackTrace(); } try { PrintWriter out
	 * = new PrintWriter(new BufferedWriter( new
	 * OutputStreamWriter(mSocket.getOutputStream())), true);
	 * out.print("Xin chao"); } catch (IOException e) { // TODO Auto-generated
	 * catch block e.printStackTrace(); } } else { // Toast.makeText(context,
	 * "Please connect to the server first", // Toast.LENGTH_SHORT).show(); } }
	 */
	public void SendCommand(String str) {

		PrintWriter out = null;
		BufferedWriter bufferedWriter = null;
		OutputStreamWriter outputStreamWriter = null;
		try {
			outputStreamWriter = new OutputStreamWriter(
					mSocket.getOutputStream());
			bufferedWriter = new BufferedWriter(outputStreamWriter);
			out = new PrintWriter(bufferedWriter, true);
			out.println(str);
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	public String ReceiveData() {
		BufferedReader in = null;
		InputStreamReader inputStreamReader = null;
		try {
			inputStreamReader = new InputStreamReader(mSocket.getInputStream());
			in = new BufferedReader(inputStreamReader);
			serverMessage = in.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return serverMessage;
	}

}