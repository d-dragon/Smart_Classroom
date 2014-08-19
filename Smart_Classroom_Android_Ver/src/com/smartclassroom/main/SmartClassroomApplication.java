package com.smartclassroom.main;
import com.smartclassroom.network.*;

import android.app.Application;
import android.os.Handler;

public class SmartClassroomApplication extends Application {

	private Socket_ini new_soc;
	private Socket_UDP new_socUDP;
	Handler mHandler;
	static SmartClassroomApplication instance;

	@Override
	public void onCreate() {
		super.onCreate();
		instance = this;
		
		new_soc = new Socket_ini(getApplicationContext());
		
		new_socUDP = new Socket_UDP(getApplicationContext(), mHandler);
	}
	
	public static SmartClassroomApplication getInstance(){
		if(instance==null){
			instance = new SmartClassroomApplication();
		}
		return instance;
	}

	public Socket_ini getNetworkSocket_TCP() {
		return new_soc;
		
	}
	public Socket_UDP getNetworkInSocket_UDP(){
		return new_socUDP;
	}
	
}
