#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//Pour interagir avec le port série
#include <fcntl.h> //Descripteurs de fichiers
#include <termios.h> //Contrôle des ports de communication asynchrone
#include <errno.h>//Gestion des erreurs
#include <mariadb/mysql.h>
#include <iostream>
using namespace std;

#define PORTSERIE "/dev/ttyS0"//Défini la constante sur le chemin sur lequel la carte est branchée
#define BAUDRATE 115200


int main(){

    int sfd, c, res;
    char buf[255];
    string temperature;
    string humidity;
    string pression;
    string requeteMySQL = "INSERT INTO mesure (pression,temperature,humidite) VALUES ('";
    char requete[1024];

    struct termios newtio;
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_lflag = ICANON;
    sfd = open(PORTSERIE, O_RDONLY | O_NOCTTY);//Ouvre le port série en lecture seule
    tcflush(sfd, TCIFLUSH);
    tcsetattr(sfd,TCSANOW,&newtio);
    if (sfd == -1){//Si le port série ne s'ouvre pas
   	 cout << "Error no is : " << errno << endl;
   	 cout << "Error description is : " << strerror(errno) << endl;//Donne la description de l'erreur
   	 return(-1);
    }
    else{//S'il n'y a pas d'erreur

   	 cout << "Port serie ouvert en lecture.\n";//Message pour etre sur que le port soit ouvert

   	 for (int i = 0; i < 150; i++){//Tourne 150 fois
   		 res = read(sfd,buf,125);
   		 string chaineDeCarac(buf);
   		 buf[res]=0;

   		 if (chaineDeCarac.find("Temp[0]") != string::npos){// cherche le caracter voulue puis temperature prend la valeur
   			 temperature = chaineDeCarac.substr(10, chaineDeCarac.find(" d"));
   			 temperature = temperature.substr(0,temperature.find(" "));
   			 cout << temperature << endl;
   		 }
   		 if(chaineDeCarac.find("Hum[0]") != string::npos){// cherche le caracter voulue puis hulidite prend la valeur
   			 humidity = chaineDeCarac.substr(8, chaineDeCarac.find(" %"));
   			 humidity = humidity.substr(0,humidity.find(" "));
   			 cout << humidity << endl;
   		 }
   		 if(chaineDeCarac.find("Press[1]") != string::npos){// cherche le caracter voulue puis pression prend la valeur
   			 pression = chaineDeCarac.substr(10, chaineDeCarac.find(" h"));
   			 pression = pression.substr(0,pression.find(" "));
   			 cout << pression << endl;
   		 }
   	 }

   	 cout << endl;
   	 requeteMySQL += pression +="','";
   	 requeteMySQL += temperature += "','";
   	 requeteMySQL += humidity += "');";//requete complete
   	 char requete[requeteMySQL.size()];
   	 strcpy(requete, requeteMySQL.c_str());
    }

    close(sfd);

    MYSQL * conn;

    if ((conn = mysql_init (NULL)) == NULL){
   	 cout << stderr << "Could not init DB\n";
   	 return EXIT_FAILURE;
    }

    if (mysql_real_connect (conn, "localhost", "bts", "snir", "TPNucleoMesures", 0, NULL, 0) == NULL){//connexion a la base de donnee
   	 cout << stderr << "DB Connection Error\n";
   	 return EXIT_FAILURE;
    }

    if (mysql_query(conn, requete) != 0){//execution de la requete si differ de 0
   	 cout << stderr << "Query Failure\n";
   	 return EXIT_FAILURE;
    }


    mysql_close(conn);
    return EXIT_SUCCESS;
}
