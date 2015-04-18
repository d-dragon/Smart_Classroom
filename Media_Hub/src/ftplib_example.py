from ftplib import FTP

def getFile(hostdomain, filename,user,passwd):

   	print(hostdomain)
	ftp = FTP(hostdomain)
    	ftp.login(user, passwd)
    	ftp.cwd('/demo')
	localfile = open(filename, 'wb')
	print(filename)
	ftp.retrbinary('RETR ' + filename, localfile.write, 1024)
	ftp.quit()
	localfile.close()
	return 123
#getFile(3,4)
