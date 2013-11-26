package client.smart_classroom;
import android.app.Application;

import network.Socket_ini.*;;

public class shareNetwork extends Application {

	private Socket_ini new_soc;

	@Override
	public void onCreate() {
		super.onCreate();
		
		new_soc = new Socket_ini(getApplicationContext());
	}

	public Socket_ini getNetworkInstance() {
		return new_soc;
	}
	
}
