package com.smartclassroom.common;

import java.util.Collection;
/**
 * 
 * @author DUYPT
 *
 */
public class CommonUtils {

	/**
	 * @author DUYPT
	 * @param data
	 * @return
	 */
	public static boolean isNullOrEmpty(Collection<Object> data) {
		if (data != null && !data.isEmpty()) {
			return true;
		}
		return false;
	}

	/**
	 * @author DUYPT
	 * @param data
	 * @return
	 */
	public static String toString(Object data) {
		if (data != null) {
			return data.toString();
		}
		return null;
	}
}
