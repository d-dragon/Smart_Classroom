package com.smartclassroom.adapter;

import java.util.ArrayList;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import client.smart_classroom.R;

import com.smartclassroom.main.Room;

public class ListAdapter extends BaseAdapter {

	ArrayList<Room> roomList;
	Context context;

	public ListAdapter(ArrayList<Room> roomList, Context context) {
		this.roomList = roomList;
		this.context = context;
	}

	@Override
	public int getCount() {
		return roomList.size();
	}

	@Override
	public Object getItem(int id) {
		return roomList.get(id);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			LayoutInflater inflater = (LayoutInflater) context
					.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			convertView = inflater.inflate(R.layout.list_row_layout, parent,
					false);
			holder = new ViewHolder(convertView);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.render(position);
		return convertView;
	}

	class ViewHolder {
		TextView txvRoomName;

		ViewHolder(View view) {
			txvRoomName = (TextView) view.findViewById(R.id.txvRoomName);
		}

		void render(int position) {
			Room room = null;
			room = roomList.get(position);
			txvRoomName.setText(room.getName());
		}
	}
}
