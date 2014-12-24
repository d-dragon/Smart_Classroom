/*
 * receive_file.c
 *
 *  Created on: Jul 12, 2014
 *      Author: d-dragon
 */

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <python2.7/Python.h>

#include "FileHandler.h"
#include "sock_infra.h"
#include "logger.h"
#include "acpHandler.h"

FILE *createFileStream(char *FileName) {

	FILE *tmp_file;
	char *path_to_file;
	path_to_file = calloc(FILE_PATH_LEN_MAX, sizeof(char));

	appLog(LOG_INFO, "FileName: %s\n", FileName);
	sprintf(path_to_file, "%s%s", DEFAULT_PATH, FileName);
	appLog(LOG_INFO, "path to file: %s\n", path_to_file);

	tmp_file = fopen(path_to_file, "w");
	if (tmp_file == NULL) {
		appLog(LOG_ERR, "fopen failed\n");
		return NULL;
	}
	appLog(LOG_DEBUG, "File stream was created successfully\n");

	return tmp_file;
}

void *FileStreamHandlerThread() {

	int szwrite = 0;
	appLog(LOG_DEBUG, "start FileStreamHandlerThread--------\n");
	/*	while (!g_StartTransferFlag && g_RecvFileFlag == RECV_FILE_DISABLED) {
	 g_waitCount++;
	 if (g_waitCount <= MAX_WAITING_COUNT) {
	 usleep(1000);
	 } else {
	 appLog(LOG_DEBUG,
	 "Wating start transfer file timeout---thread exit\n");
	 g_RecvFileFlag = RECV_FILE_DISABLED;
	 pthread_exit(NULL);

	 }
	 }*/

//	appLog(LOG_DEBUG, "Start Transfer file-------------\n");
//	g_RecvFileFlag = RECV_FILE_ENABLED;
	while (1) {

		//first time need data before write it to file stream
		if (g_writeDataFlag != ENABLED) {
			continue;
		}
		pthread_mutex_lock(&g_file_buff_mutex);
		//check transfer file flow finished--thread exit
		if (g_RecvFileFlag == RECV_FILE_DISABLED) {
			g_writeDataFlag = DISABLED;
			closeFileStream();

			if (wrapperControlResp(CTRL_RESP_FILE_FINISH) == ACP_SUCCESS) {
				appLog(LOG_DEBUG,
						"Transfer file completed, FileStreamHandlerThread exit");
			} else {
				appLog(LOG_DEBUG,
						"Transfer file incomplete, FileStreamHandlerThread exit");
			}
			pthread_mutex_unlock(&g_file_buff_mutex);
			pthread_mutex_destroy(&g_file_buff_mutex);

			pthread_exit(NULL);
		}
		//write data to file
		szwrite = fwrite(g_FileBuff, 1, num_byte_read, g_file_stream);
		//				printf("%d bytes was written\n", szwrite);
		if (szwrite < num_byte_read) {
			appLog(LOG_ERR, "write data to stream file failed!\n");
			g_writeDataFlag = DISABLED;
			g_RecvFileFlag = RECV_FILE_DISABLED;
			closeFileStream();
			if (wrapperControlResp(CTRL_RESP_FAILED) == ACP_SUCCESS) {
				appLog(LOG_DEBUG,
						"Transfer file incomplete, FileStreamHandlerThread exit");
			}
			pthread_mutex_unlock(&g_file_buff_mutex);
			pthread_mutex_destroy(&g_file_buff_mutex);

			pthread_exit(NULL);
		} else if (szwrite == num_byte_read) {
			appLog(LOG_DEBUG, "wrote %d byte to file stream\n",
					(int) num_byte_read);
			g_writeDataFlag = DISABLED;
			pthread_mutex_unlock(&g_file_buff_mutex);

		}
		/*		pthread_mutex_lock(g_file_buff_mutex);
		 pthread_cond_wait(g_file_thread_cond, g_file_buff_mutex);
		 pthread_mutex_unlock(g_file_buff_mutex);*/
	}
}

void closeFileStream() {
	if (g_file_stream != NULL) {
		fclose(g_file_stream);
	}
}

int getListFile(char *DirPath, char *ListFile) {

	DIR *dir = NULL;
	struct dirent *dirnode = NULL;

	dir = opendir(DirPath);
	if (dir) {
		while ((dirnode = readdir(dir)) != NULL) {
			if (dirnode->d_type == DT_REG) {
				if ((strlen(ListFile) + strlen(dirnode->d_name)) > LIST_FILE_MAX) {
					//list file length exceed max len
					return FILE_UNKNOW;
				}
				strcat(ListFile, dirnode->d_name);
				strcat(ListFile, "|");
			}
		}
	} else {
		closedir(dir);
		appLog(LOG_DEBUG, "open directory failed");
		return FILE_ERROR;
	}
	closedir(dir);
	return FILE_SUCCESS;
}

int getFileFromFtp(char *FtpServerIP, char *FileName) {

	PyObject *pName, *pModule, *pFunc;
	PyObject *pArgs, *pValue;

	int i;
	long arg;

	//init python interpreter
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(PY_SYS_PATH);

	//load python module
	pName = PyString_FromString(PY_MODULE);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL) {
		//get python function
		pFunc = PyObject_GetAttrString(pModule, FTP_GET_FILE_FUNC);
		//checking pFunc is callable
		if (pFunc && PyCallable_Check(pFunc)) {
			//create arg list with n member
			pArgs = PyTuple_New(2);
			//convert arg from C to Python
			for (i = 0; i < 2; i++) {
				if (i == 0) {
					pValue = PyString_FromString(FtpServerIP);
				} else {
					pValue = PyString_FromString(FileName);
				}
				if (!pValue) {
					Py_DECREF(pArgs);
					Py_DECREF(pModule);
					printf("cannot convert argument\n");
					return FILE_ERROR;
				}
				//set arg to python reference to pValue
				PyTuple_SetItem(pArgs, i, pValue);
			}

			//call Python Func
			pValue = PyObject_CallObject(pFunc, pArgs);
			Py_DECREF(pArgs);
			if (pValue != NULL) {
				printf("Result of Python call: %ld\n", PyInt_AsLong(pValue));
				Py_DECREF(pValue);
			} else {
				Py_DECREF(pFunc);
				Py_DECREF(pModule);
				PyErr_Print();
				printf("call failed\n");
				return FILE_ERROR;
			}

		} else {
			if (PyErr_Occurred()) {
				PyErr_Print();
			} else {
				printf("cannot find function \'%s\'\n", FTP_GET_FILE_FUNC);
				return FILE_ERROR;
			}
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	} else {
		PyErr_Print();
		printf("failed to load %s\n", PY_MODULE);
		return 1;
	}
	Py_Finalize();
	return FILE_SUCCESS;
}
