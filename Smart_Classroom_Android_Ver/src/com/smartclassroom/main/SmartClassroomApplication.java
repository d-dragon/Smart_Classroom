package com.smartclassroom.main;
import com.smartclassroom.network.*;

import android.app.Application;
import android.os.Handler;

public class SmartClassroomApplication extends Application {

	private SocketTCP new_soc;
	private SocketUDP new_socUDP;
	Handler mHandler;
	static SmartClassroomApplication instance;

	@Override
	public void onCreate() {
		super.onCreate();
		instance = this;
		
		new_soc = new SocketTCP(getApplicationContext());
		
		new_socUDP = new SocketUDP(getApplicationContext(), mHandler);
	}
	
	public static SmartClassroomApplication getInstance(){
		if(instance==null){
			instance = new SmartClassroomApplication();
		}
		return instance;
	}

	public SocketTCP getNetworkSocket_TCP() {
		return new_soc;
		
	}
	public SocketUDP getNetworkInSocket_UDP(){
		return new_socUDP;
	}
	
}
