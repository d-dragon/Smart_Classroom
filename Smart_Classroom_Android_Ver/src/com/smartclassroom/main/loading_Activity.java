package com.smartclassroom.main;

import com.smartclassroom.network.*;

import client.smart_classroom.R;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class loading_Activity extends Activity implements OnClickListener {

    private final int WAIT_TIME = 1000;
    private SocketTCP connector;
    private SocketUDP conSocket_UDP;
//    private SmartClassroomApplication shared;
    private Button btnConnect;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.connect_layout);
		
		btnConnect = (Button) findViewById(R.id.btnC);
		btnConnect.setOnClickListener(this);
		btnConnect.setVisibility(View.INVISIBLE);
		
		//findViewById(R.id.mainSpinner1).setVisibility(View.VISIBLE);
		
//		shared = (SmartClassroomApplication) getApplicationContext();
		connector = SmartClassroomApplication.getInstance().getNetworkSocket_TCP();
//		connector = shared.getNetworkSocket_TCP();
//		conSocket_UDP = shared.getNetworkInSocket_UDP();
		conSocket_UDP = SmartClassroomApplication.getInstance().getNetworkInSocket_UDP();
		conSocket_UDP.start();
		connector.start();
	
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
		    	Intent mainIntent = new Intent(loading_Activity.this, ControlActivity.class);
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
				    	Intent mainIntent = new Intent(loading_Activity.this, ControlActivity.class);
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
