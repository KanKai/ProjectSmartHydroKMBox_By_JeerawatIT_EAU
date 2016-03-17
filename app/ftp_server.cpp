/*
 * ftp_server.cpp
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: admin
 */
#include <application.h>

FTPServer ftp;


void startFTP()
{
	if (!fileExist("index.html"))
		fileSetContent("index.html", "<h3>Please connect to FTP and upload files from folder 'web/build' (details in code)</h3>");
	// Start FTP server
	ftp.listen(21);
	ftp.addUser("admin", "123"); // FTP account
}


