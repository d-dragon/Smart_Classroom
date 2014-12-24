from ftplib import FTP

def getFile(hostdomain, filename):

        print(hostdomain)
        ftp = FTP(hostdomain)
        ftp.login()
        ftp.cwd('/')
#	filename = 'IMG_3858.CR2'
	localfile = open(filename, 'wb')
	print(filename)
	ftp.retrbinary('RETR ' + filename, localfile.write, 1024)
	ftp.quit()
	localfile.close()
	return 123
#getFile(3,4)
