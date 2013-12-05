package client.smart_classroom;

import network.Socket_ini.*;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class loading_Activity extends Activity implements OnClickListener {

    private final int WAIT_TIME = 1000;
    private Socket_ini connector;
    private Socket_UDP conSocket_UDP;
    private shareNetwork shared;
    private Button btnConnect;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.loading);
		
		btnConnect = (Button) findViewById(R.id.btnC);
		btnConnect.setOnClickListener(this);
		btnConnect.setVisibility(View.INVISIBLE);
		
		//findViewById(R.id.mainSpinner1).setVisibility(View.VISIBLE);
		
		shared = (shareNetwork) getApplicationContext();
		connector = shared.getNetworkSocket_TCP();
		conSocket_UDP = shared.getNetworkInSocket_UDP();
		conSocket_UDP.start();
		connector.connectToNetwork();
	
		new Handler().postDelayed(new Runnable(){ 
		@Override 
		    public void run() {
	              
			
			//Simulating a long running task
	        try {
				Thread.sleep(5000);
			} 
	        catch (InterruptedException e) {
				e.printStackTrace();
			}
		     
		     /* Create an Intent that will start the ProfileData-Activity. */
		    if(connector.ismIsConnected()) {
		    	Intent mainIntent = new Intent(loading_Activity.this, Main.class);
			    loading_Activity.this.startActivity(mainIntent); 
			    loading_Activity.this.finish();
		    }
		    else {
		    	btnConnect.setVisibility(View.VISIBLE);
		    	
		    }
			} 
		}, WAIT_TIME);
		
      }

	@Override
	public void onClick(View v) {
		switch(v.getId()) {
		case R.id.btnC:
			new Handler().postDelayed(new Runnable(){ 
				@Override 
				    public void run() {
			              
					
					//Simulating a long running task
			        try {
						Thread.sleep(1000);
					} 
			        catch (InterruptedException e) {
						e.printStackTrace();
					}
				     
				     /* Create an Intent that will start the ProfileData-Activity. */
				    if(connector.ismIsConnected()) {
				    	Intent mainIntent = new Intent(loading_Activity.this, Main.class);
					    loading_Activity.this.startActivity(mainIntent); 
					    loading_Activity.this.finish();
				    }
				    else {
				    	
				    }
					} 
				}, WAIT_TIME);
			break;
		}
		
	}
}
