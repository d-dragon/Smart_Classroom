from ftplib import FTP

def getFile(hostdomain, filename,user,passwd):

   	print(hostdomain)
	print(user)
	print(passwd)
	ftp = FTP()
#	ftp = FTP(hostdomain)
	ftp.connect(hostdomain, 21212)
    	ftp.login(user, passwd)
#	ftp.login()
    	ftp.cwd('/demo')
#	ftp.cwd('/Firmware')
	localfile = open(filename, 'wb')
	print(filename)
	ftp.retrbinary('RETR ' + filename, localfile.write, 1024)
	ftp.quit()
	localfile.close()
	return 123
#getFile('10.0.0.111','README.md','user','pass')
