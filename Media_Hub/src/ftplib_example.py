from ftplib import FTP
from os.path import expanduser

def getFile(hostdomain, filename,user,passwd):
	home = expanduser('~')
   	print(hostdomain)
	print(user)
	print(passwd)
	ftp = FTP()
#	ftp = FTP(hostdomain)
	ftp.connect(hostdomain, 21)
    	ftp.login(user, passwd)
#	ftp.login()
    	ftp.cwd('demo/')
#	ftp.cwd('/Firmware')
	filepath = '/home/pi/Smart_Classroom/Media_Hub/src/' + filename
	print filepath
	localfile = open(filepath, 'wb')
	ftp.retrbinary('RETR ' + filename, localfile.write, 1024)
	ftp.quit()
	localfile.close()
	return 123
#getFile('192.168.1.102','zwave_programmer_tool','user','pass')
