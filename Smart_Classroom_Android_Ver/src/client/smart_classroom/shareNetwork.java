package client.smart_classroom;
import android.app.Application;
import android.os.Handler;

import network.Socket_ini.*;

public class shareNetwork extends Application {

	private Socket_ini new_soc;
	private Socket_UDP new_socUDP;
	Handler mHandler;

	@Override
	public void onCreate() {
		super.onCreate();
		
		new_soc = new Socket_ini(getApplicationContext());
		
		new_socUDP = new Socket_UDP(getApplicationContext(), mHandler);
	}

	public Socket_ini getNetworkSocket_TCP() {
		return new_soc;
		
	}
	public Socket_UDP getNetworkInSocket_UDP(){
		return new_socUDP;
	}
	
}
