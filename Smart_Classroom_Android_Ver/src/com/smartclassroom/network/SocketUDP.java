/**
 * 
 */
package com.smartclassroom.network;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.util.Log;

import com.smartclassroom.common.Logger;
import com.smartclassroom.listener.OnEventControlListener;

/**
 * @author DuyPT
 */
public class SocketUDP {

	private static final int BCAST_PORT = 1992;
	InetAddress myBcastIP, myLocalIP;
	DatagramSocket mSocket;
	// Debugging
	private static final String TAG = "BroadcastService";
	private ReceiveThread receiveThread;
	private SendThread sendThread;
	private OnEventControlListener onEventControlListener;
	/*
	 * Thread status flag
	 */
	private boolean isRunning;

	public void setOnEventControlListener(OnEventControlListener listener) {
		this.onEventControlListener = listener;
	}

	Context mContext;

	/**
	 * Constructor. Prepares a new Broadcast service.
	 * 
	 * @param context
	 *            The UI Activity Context
	 * @param handler
	 *            A Handler to send messages back to the UI Activity
	 */
	public SocketUDP(Context context, Handler handler) {
		mContext = context;
		receiveThread = null;
		sendThread = null;
		isRunning = true;
	}

	/**
	 * Start the chat service. Specifically start ComThread to begin listening
	 * incoming broadcast packets.
	 */
	public synchronized void start() {
		Logger.show(TAG, "start");
		receiveThread = new ReceiveThread();
		receiveThread.start();
	}

	/**
	 * Stop thread
	 */
	public synchronized void stop() {
		Logger.show(TAG, "stop");
		closeSocket();
		stopThread();
	}

	public void sendMessage(byte[] out) {
		sendThread = new SendThread(out);
		sendThread.start();
	}

	private class SendThread extends Thread {

		byte[] buffer;

		public SendThread(byte[] out) {
			this.buffer = out;
		}

		@Override
		public void run() {
			if (buffer == null || buffer.length == 0) {
				return;
			}
			String data = null;
			try {
				data = new String(buffer);
				DatagramPacket packet = new DatagramPacket(data.getBytes(),
						data.length(), myBcastIP, BCAST_PORT);
				Logger.show("UDP send:" + data);
				mSocket.send(packet);
			} catch (Exception e) {
				Logger.show(TAG, "Exception during write", e);
			}
		}
	}

	private class ReceiveThread extends Thread {
		public ReceiveThread() {
			try {
				myBcastIP = getBroadcastAddress();
				Logger.show(TAG, "my bcast ip : " + myBcastIP);

				myLocalIP = getLocalAddress();
				Logger.show(TAG, "my local ip : " + myLocalIP);

				mSocket = new DatagramSocket(BCAST_PORT);
				mSocket.setBroadcast(true);

			} catch (IOException e) {
				Logger.show(TAG, "Could not make socket", e);
			}
		}

		public void run() {
			DatagramPacket packet = null;
			InetAddress remoteIP = null;
			String msg = null;
			byte[] buf = null;
			try {
				
				// Listen on socket to receive messages
				while (isRunning()) {
					buf = new byte[1024];
					packet = new DatagramPacket(buf, buf.length);
					mSocket.receive(packet);
					Logger.show(TAG,
							"Broadcast state:" + mSocket.getBroadcast());
					remoteIP = packet.getAddress();
					if (remoteIP.equals(myLocalIP)) {
						continue;
					}
					msg = new String(packet.getData(), 0, packet.getLength());
					Logger.show(TAG, "Received response " + msg);
					onEventControlListener.onEvent(null,
							OnEventControlListener.EVENT_UDP_MESSAGE, msg);
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		/**
		 * Calculate the broadcast IP we need to send the packet along.
		 */
		private InetAddress getBroadcastAddress() throws IOException {
			WifiManager mWifi = (WifiManager) mContext
					.getSystemService(Context.WIFI_SERVICE);
			WifiInfo info = mWifi.getConnectionInfo();
			Logger.show(TAG, "\n\nWiFi Status: " + info.toString());
			// DhcpInfo is a simple object for retrieving the results of a DHCP
			// request
			DhcpInfo dhcp = mWifi.getDhcpInfo();
			if (dhcp == null) {
				Logger.show(TAG, "Could not get dhcp info");
				return null;
			}
			int broadcast = (dhcp.ipAddress & dhcp.netmask) | ~dhcp.netmask;
			byte[] quads = new byte[4];
			for (int k = 0; k < 4; k++)
				quads[k] = (byte) ((broadcast >> k * 8) & 0xFF);
			// Returns the InetAddress corresponding to the array of bytes.
			return InetAddress.getByAddress(quads); // The high order byte is
													// quads[0].
		}

		private InetAddress getLocalAddress() throws IOException {
			try {
				for (Enumeration<NetworkInterface> en = NetworkInterface
						.getNetworkInterfaces(); en.hasMoreElements();) {
					NetworkInterface intf = en.nextElement();
					for (Enumeration<InetAddress> enumIpAddr = intf
							.getInetAddresses(); enumIpAddr.hasMoreElements();) {
						InetAddress inetAddress = enumIpAddr.nextElement();
						if (!inetAddress.isLoopbackAddress()) {
							// return inetAddress.getHostAddress().toString();
							return inetAddress;
						}
					}
				}
			} catch (SocketException ex) {
				Log.e(TAG, ex.toString());
			}
			return null;
		}

	}

	public void closeSocket() {
		if (mSocket == null) {
			return;
		}
		try {
			mSocket.close();
		} catch (Exception e) {
			Log.e(TAG, "close() of connect socket failed", e);
		}
	}

	public void stopThread() {
		if (receiveThread == null) {
			return;
		}
		setRunning(false);
	}

	public boolean isRunning() {
		return isRunning;
	}

	public void setRunning(boolean isRunning) {
		this.isRunning = isRunning;
	}
}
