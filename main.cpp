#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <mariadb/mysql.h>
#include <unistd.h>				
#include <fcntl.h>              
#include <termios.h>
#include <errno.h>				//gestion des erreurs

#define BAUDRATE B115200
#define PORTSERIE "/dev/ttyS2"

int main()
{
	MYSQL *conn; 
	int res;
	char buf[255];
	char *chaineTemp;
	char *chaineHum;
	char *chainePress;
	struct termios newtio;
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD ;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_lflag = ICANON;
	int sfd;
	
	sfd = open(PORTSERIE, O_RDONLY | O_NOCTTY);

	tcflush(sfd, TCIFLUSH);
	tcsetattr(sfd,TCSANOW,&newtio);

	if(sfd == -1)
	{
		std::cout << "error open serial" << std::endl;
		return(-1);
	}else
	{
		std::cout << "port serie ouvert" << std::endl;
		 for(int i = 0; i < 50; i++)
		 {
		 	res = read(sfd, buf, 127);
		 	buf[res]=0;
		 	std::cout << buf << std::endl;
		 	chaineTemp = strstr(buf,"Temp[0]");
		 	chainePress = strstr(buf,"Press[1]");
		 }
		 std::cout << "chaineTemp:" << chaineTemp;
	}

	if((conn = mysql_init(NULL)) == NULL)
	{
		fprintf(stderr, "Could not init DB\n");
		return EXIT_FAILURE;
	}

	if(mysql_real_connect(conn, "localhost", "bts", "bts", "tp11_nucleo", 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "DB Connection Error\n");
		return EXIT_FAILURE;
	}

	if(mysql_query(conn, "INSERT INTO relever(pression, temperature, taux_humidite) VALUES('1000', '24', '55')") != 0)
	{
		fprintf(stderr, "Querry Failure\n");
		return EXIT_FAILURE;
	}



	mysql_close(conn);
	return EXIT_SUCCESS;
}