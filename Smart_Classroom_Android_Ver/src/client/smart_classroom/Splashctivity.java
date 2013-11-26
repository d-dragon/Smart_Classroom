package client.smart_classroom;


import client.smart_classroom.R;
import client.smart_classroom.loading_Task.LoadingTaskFinishedListener;
import network.Socket_ini.*;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
//import android.support.v4.widget.SearchViewCompatIcs.MySearchView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class Splashctivity extends Activity implements LoadingTaskFinishedListener, OnClickListener {

	private Socket_ini connector;
    private shareNetwork shared;
    private Button btnConnect;
    private ProgressBar pb;
    private TextView tv;
    public EditText etIP, etPort;
    public String IP=" ";
    public int Port=0;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Show the splash screen
        setContentView(R.layout.loading);
        
        shared = (shareNetwork) getApplicationContext();
		connector = shared.getNetworkInstance();
		etIP = (EditText)findViewById(R.id.etIP);
		etPort = (EditText)findViewById(R.id.etPort);
		
		btnConnect = (Button) findViewById(R.id.btnC);
		btnConnect.setOnClickListener(this);
		btnConnect.setVisibility(View.INVISIBLE);
		
		pb = (ProgressBar) findViewById(R.id.pbLoading);
		
		tv = (TextView) findViewById(R.id.tvSplash);
        
		new loading_Task(this, getApplicationContext()).execute("abc"); // Pass in whatever you need a url is just an example we don't use it in this tutorial
		
    }

    // This is the callback for when your async task has finished
    @Override
	public void onTaskFinished() {
		completeSplash();
	}

    private void completeSplash(){
    	if(connector.ismIsConnected()) {
			startApp();
			finish(); // Don't forget to finish this Splash Activity so the user can't return to it!
    	}
    	else {
    		Toast.makeText(getApplicationContext(), "Kết nối thất bại. Mời nhấn nút kết nối", Toast.LENGTH_SHORT).show();
    		pb.setVisibility(View.INVISIBLE);
    		btnConnect.setVisibility(View.VISIBLE);
    		tv.setVisibility(View.INVISIBLE);
    	}
    }

    private void startApp() {
		Intent intent = new Intent(Splashctivity.this, Main.class);//MainActivity.class
		startActivity(intent);
	}

	@Override
	public void onClick(View v) {
		
		switch(v.getId()) {
		case R.id.btnC:
			IP = etIP.getText().toString();
			try{
				Port = Integer.parseInt(etPort.getText().toString());
			}catch (NumberFormatException e) {
				// TODO: handle exception
				Toast.makeText(getApplicationContext(), "Chi nhap so interger cho Port", Toast.LENGTH_SHORT).show();
			}
			connector.setIp(IP);
			connector.setPort(Port);
			new loading_Task(this, getApplicationContext()).execute("abc");
			pb.setVisibility(View.VISIBLE);
			tv.setVisibility(View.VISIBLE);
			btnConnect.setVisibility(View.INVISIBLE);
			connector.connectToNetwork();
			
			break;
		}
	}
}