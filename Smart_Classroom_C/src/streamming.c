#include <stdio.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
	int status;
	if(!fork())
	{
		status = system("./mjpg_streamer -i \"input_uvc.so -d /dev/video0 -f 30 -r 640x480 \" -o \"output_http.so -p 8085 -w var/www/mjpg-streamer\"");
		_exit(EXIT_SUCCESS);
	}
	else
	{
		printf("\n You can't create process");
	}
	printf("\n Now you can do other thing in main process");

	return 0;

}
