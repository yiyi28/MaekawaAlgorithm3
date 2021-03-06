#include "Controller.h"
#include <vector>
#include <string>
#include <sstream>
#include<iostream>
using namespace std;




//Msg type=100
Controller::Controller(void):QuorumTable{//{0,1},{1,2},{0,2} {0,1,2},{1,3,5},{2,4,5},{0,3,4},{1,4,6},{0,5,6},{2,3,6}
		{0, 1, 2, 3, 4, 8, 12},
		{0, 1, 2, 3, 5, 9, 13},
		{0, 1, 2, 3, 6, 10, 14},
		{0, 1, 2, 3, 7, 11, 15},
		{4, 5, 6, 7, 0, 8, 12},
		{4, 5, 6, 7, 1, 9, 13},
		{4, 5, 6, 7, 2, 10, 14},
		{4, 5, 6, 7, 3, 11, 15},
		{8, 9, 10, 11, 0, 4, 12},
		{8, 9, 10, 11, 1, 5, 13},
		{8, 9, 10, 11, 2, 6, 14},
		{8, 9, 10, 11, 3, 7, 15},
		{12, 13, 14, 15, 0, 4, 8},
		{12, 13, 14, 15, 1, 5, 9},
		{12, 13, 14, 15, 2, 6, 10},
		{12, 13, 14, 15, 3, 7, 11}
}
{
	Algorithm = 0;
}

Controller::~Controller(void)
{
	//delete QuorumTable;
}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}


void DieWithError1(char* errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void DieWithError1(char *errorMessage);  /* Error handling function */

void Controller::handle(int clntSock1,char* client_ip,int counter,Controller *con)
{
	communication com;
	char msg[128]={'\0',};
	int sd=0;
	com.readFromSocket(clntSock1,msg,128);
	puts("returned\n");
	printf("Client %s is UP",msg);
	puts("\n");
	int qColSize=QuorumSize;
	int qRowSize=MAXNODES;
	puts("\n");
	com.writeToSocket(clntSock1,&counter,sizeof(int));
	com.writeToSocket(clntSock1,&qColSize,sizeof(int));
	com.writeToSocket(clntSock1,&qRowSize,sizeof(int));
	puts("quorum table");
	for(int i=0;i<qRowSize;i++){
		for(int j=0;j<qColSize;j++){
			printf("%d\t",QuorumTable[i][j]);
		}
		printf("\n");	
		
	}
	string strToSend,tempStr; strToSend.clear(); tempStr.clear();
	char temp[5] = {0,};
	for(int i=0;i<qRowSize;i++){
		for(int j=0;j<qColSize;j++){
			snprintf(temp,4,"%d",QuorumTable[i][j]);
			tempStr = temp;
			strToSend += tempStr;
			strToSend += ":";
			memset(&temp,0,sizeof(temp));
			tempStr.clear();
		}
		//printf("size: %d\n",sizeof(row)*sizeof(int));
		//com.writeToSocket(clntSock1,row,sizeof(row)*sizeof(int));	
		
	}
	com.writeToSocket(clntSock1,const_cast<char*> (strToSend.c_str()), strToSend.length());	
	cout<<"Sent Msg - "<<strToSend<<endl;
	
	printf("Counter value %d, ",counter);
	strcpy(mapIPtoID[counter], client_ip);
	printf("MaptoIP %s \n",mapIPtoID[counter]);

}






void *csListener(void*) {
	
	//Controller *con = (Controller*)c;
	//printf("In CS Listener\n");
	/*communication com;
	com.serverListen(1235,m_queue);*/
	int counter=0;
	int servSock;                    /* Socket descriptor for server */
	int clntSock;                    /* Socket descriptor for client */
	struct sockaddr_in echoServAddr; /* Local address */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned short echoServPort;     /* Server port */
	socklen_t clntLen;            /* Length of client address data structure */


	echoServPort = LISTEN_PORT_CS;  /* First arg:  local port */

	/* Create socket for incoming connections */
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError1("socket() failed");

	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
	echoServAddr.sin_family = AF_INET;                /* Internet address family */
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	echoServAddr.sin_port = htons(echoServPort);      /* Local port */

	/* Bind to the local address */
	if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError1("bind() failed");

	/* Mark the socket so it will listen for incoming connections */
	if (listen(servSock, MAXPENDING) < 0)
		DieWithError1("listen() failed");
	//int del=0;
	for (;;) /* Run forever */
	{
		/* Set the size of the in-out parameter */
		clntLen = sizeof(echoClntAddr);

		/* Wait for a client to connect */
		if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,&clntLen)) < 0)
		{
			close(servSock);
			DieWithError1("accept() failed");
		}

		/* clntSock is connected to a client! */
		char *client_ip = inet_ntoa(echoClntAddr.sin_addr);
		//printf("\nClient socket %d, addlen : %d %s\n",clntSock,sizeof(client_ip),client_ip);
		/*con->handle(clntSock,client_ip,counter,con);
		counter++;*/
		communication com;
		Packet p;
		com.readFromSocket(clntSock,&p,sizeof(p));
		if(p.TYPE == ENTER_CS){
		printf("Node %d Entered CS",p.ORIGIN);
	/*	shutdown(clntSock,0);
				int k = close(clntSock);
				if (k < 0) {
						printf("\nError in Closing");
						exit(0);
				}*/
		}
		if(p.TYPE == END_CS)
		{
			printf("Node %d Exit the CS",p.ORIGIN);
			int confirmation=1;
			com.writeToSocket(clntSock,&confirmation,sizeof(int));
			
				
		}
		shutdown(clntSock,0);
		int k = close(clntSock);
						if (k < 0) {
								printf("\nError in Closing");
								exit(0);
						}
		
		
	}
	shutdown(servSock,2);
	int k = close(servSock);
	if (k < 0) {
		printf("\nError in Closing");
		exit(0);
	}

	return NULL;
}













void *listener(void* c) {
	
	Controller *con = (Controller*)c;
	printf("In Listener\n");
	/*communication com;
	com.serverListen(1235,m_queue);*/
	int counter=0;
	int servSock;                    /* Socket descriptor for server */
	int clntSock;                    /* Socket descriptor for client */
	struct sockaddr_in echoServAddr; /* Local address */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned short echoServPort;     /* Server port */
	socklen_t clntLen;            /* Length of client address data structure */


	echoServPort = LISTEN_PORT;  /* First arg:  local port */

	/* Create socket for incoming connections */
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError1("socket() failed");

	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
	echoServAddr.sin_family = AF_INET;                /* Internet address family */
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	echoServAddr.sin_port = htons(echoServPort);      /* Local port */

	/* Bind to the local address */
	if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError1("bind() failed");

	/* Mark the socket so it will listen for incoming connections */
	if (listen(servSock, MAXPENDING) < 0)
		DieWithError1("listen() failed");
	//int del=0;
	for (;counter<MAXNODES;) /* Run forever */
	{
		/* Set the size of the in-out parameter */
		clntLen = sizeof(echoClntAddr);

		/* Wait for a client to connect */
		if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,&clntLen)) < 0)
		{
			close(servSock);
			DieWithError1("accept() failed");
		}

		/* clntSock is connected to a client! */
		char *client_ip = inet_ntoa(echoClntAddr.sin_addr);
		printf("\nClient socket %d, addlen : %d %s\n",clntSock,sizeof(client_ip),client_ip);
		con->handle(clntSock,client_ip,counter,con);
		counter++;
		shutdown(clntSock,0);
		int k = close(clntSock);
		if (k < 0) {
				printf("\nError in Closing");
				exit(0);
		}
	}
	shutdown(servSock,2);
	int k = close(servSock);
	if (k < 0) {
		printf("\nError in Closing");
		exit(0);
	}

	return NULL;
}

void Controller::sendTokenToNode()
{
	printf("In send token");
	//Send token
	Packet pack;
	pack.TYPE=SEND_TOKEN;
	pack.ORIGIN=CONTROLLER_ID;
	pack.SEQ=1;
	pack.sender=CONTROLLER_ID;

	communication com;
	//printf("Map to ID %s\n",mapIPtoID[0]);
	char desIP[16];
	strncpy(desIP,mapIPtoID[0],16);
	printf("sending TOKEN to Node 0 at IP %s\n",desIP);

	com.sendMessage(pack,desIP,LISTEN_PORT3);

}

void Controller::decideAlgorithm(){
	printf("Enter the Algorithm for the nodes to follow:\n");
	printf("\t\t1:Maekawa\t2:Torum\n");
	cin>>Algorithm;

	communication con;
	for(int i=0;i<MAXNODES;i++){
		printf("mapIP:%s -->",mapIPtoID[i]);
		int sockfd = con.connectToServer(mapIPtoID[i],LISTEN_PORT2);

		con.writeToSocket(sockfd,&Algorithm,sizeof(int));
		string strToSend,tempStr; strToSend.clear(); tempStr.clear();
		for(int j=0;j<MAXNODES;j++){
			//printf("ipaddress:%s\n",mapIPtoID[j]);
			tempStr = mapIPtoID[j];
			strToSend += tempStr;
			strToSend += ":";
			tempStr.clear();
		}
		printf("strtosend: %s\n",strToSend.c_str());
		con.writeToSocket(sockfd,const_cast<char*> (strToSend.c_str()),4095);
		char *csfilename=CS_FILENAME;
		con.writeToSocket(sockfd,csfilename,25);
		shutdown(sockfd,0);
		close(sockfd);
	}
	//give time for Nodes to prepare for receiving messages
	sleep(10);
	if(Algorithm == 1){
		Algorithm1();
	}else if(Algorithm == 2){
		Algorithm2();

	}else
		printf("Invalid input\n");

}

void Controller::Algorithm1(){
	printf("\nYou have chosen Maekawa Distributed Mutual Exclusion Algorithm\n");
	UserInput();
}
void Controller::Algorithm2(){
	printf("\nYou have chosen Token and Quorum Based Mutual Exclusion Algorithm: Torum\n");
	sendTokenToNode();
	UserInput();

}

void Controller::UserInput(){
	printf("Enter your requests for critical section here...\n");
	printf("");
	for(int i=0;;i++){
		int id=0;
		string input;
		printf("Enter id to send request for CS (Use ':' to send the simultaneous requests )\n");
		cin>>input;
		vector<string> x = split(input, ':');
		int c=0;
			for( vector<string>::const_iterator i = x.begin(); i != x.end(); ++i)
			{

				int k=atoi((*i).c_str());
				if(k == 999){
							endProcess();
							return;
						}
				else if(c==0){
					sleep(k);
					c++;
				}
				else if(k < MAXNODES)
					sendCSrequests(k);
				else
					printf("Invalid input %d\n",k);
			    
			}
		
		/*cin>>id;
		if(id == 999){
			endProcess();
			return;
		}
		if(id == 50)
		{
			int num=0;
			printf("Enter the number of simultaneous requests:\n");
			cin>>num;
			printf("Enter the id:\n");
			for(int i=0;i<num;i++)
			{
				int nID=9999;
				printf("Enter the id:\n");
				cin>>nID;
				sendCSrequests(id);
				
			}
			
			
		}
		else
			sendCSrequests(id);*/
	}
}

void createCSFile(){
		string str = "../";
		str += CS_FILENAME;
		char *fname = new char[30];
		strcpy(fname,str.c_str());
		ofstream myfile (fname,ios::out);
		  if (myfile.is_open())
		  {
		    myfile <<"#This file contains log of Critical Section Requests from the Nodes"<<endl;
		    myfile.close();
		  }
		  else cout << "Unable to open file";
}
void Controller::sendCSrequests(int node){
	numOfCSRequests++;
	Packet pack1;
	pack1.TYPE = MAKE_REQUEST;
	pack1.ORIGIN = CONTROLLER_ID;
	pack1.SEQ =1;
	pack1.sender = CONTROLLER_ID;
	communication com;
	char desIP[16];
	strncpy(desIP,mapIPtoID[node],16);
	printf("sending MAKE_REQUEST to Node %d at IP %s\n",node,desIP);

	com.sendMessage(pack1,desIP,LISTEN_PORT3);
}
void Controller::initiate(Controller *c){
	numOfCSRequests = 0;
	TotalNoMsgs = 0;
	TotalTime = 0;
	createCSFile();
	pthread_t listen;
	pthread_create(&listen, NULL,listener,(void*)c );
	pthread_join(listen,NULL);

}

void Controller::endProcess()
{
	Packet pack1;
	pack1.TYPE = END_PROCESS;
	pack1.ORIGIN = CONTROLLER_ID;
	pack1.SEQ =1;
	pack1.sender = CONTROLLER_ID;
	communication com;
	char desIP[16];
	for(int n=0;n<MAXNODES;n++){
	strncpy(desIP,mapIPtoID[n],16);
	struct timeval end;
	gettimeofday(&end, NULL);
	printf("sending END_PROCESS to Node %d at IP %s\n",n,desIP);
	com.sendMessage(pack1,desIP,LISTEN_PORT3);
	}


	int TotalNoMsgs = 0;
	int TotalTime = 0;
	printf("End of Algorithm!!\n Calculating the statistics . . .\n");
	int counter=0;
		int servSock;                    /* Socket descriptor for server */
		int clntSock;                    /* Socket descriptor for client */
		struct sockaddr_in echoServAddr; /* Local address */
		struct sockaddr_in echoClntAddr; /* Client address */
		unsigned short echoServPort;     /* Server port */
		socklen_t clntLen;            /* Length of client address data structure */


		echoServPort = LISTEN_PORT_END;  /* First arg:  local port */

		/* Create socket for incoming connections */
		if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			DieWithError1("socket() failed");

		/* Construct local address structure */
		memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
		echoServAddr.sin_family = AF_INET;                /* Internet address family */
		echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
		echoServAddr.sin_port = htons(echoServPort);      /* Local port */

		/* Bind to the local address */
		if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
			DieWithError1("bind() failed");

		/* Mark the socket so it will listen for incoming connections */
		if (listen(servSock, MAXPENDING) < 0)
			DieWithError1("listen() failed");

		for(int n=0;n<MAXNODES;n++){
			/* Set the size of the in-out parameter */
			clntLen = sizeof(echoClntAddr);
			/* Wait for a client to connect */
			if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,&clntLen)) < 0)
			{
				close(servSock);
				DieWithError1("accept() failed");
			}

			/* clntSock is connected to a client! */
			char *client_ip = inet_ntoa(echoClntAddr.sin_addr);
			printf("\nClient socket %d, addlen : %d %s\n",clntSock,sizeof(client_ip),client_ip);

			communication com;
			int id = -1;
			com.readFromSocket(clntSock,&id,sizeof(int));
			long msgc;
			com.readFromSocket(clntSock,&msgc,sizeof(long));
			TotalNoMsgs +=msgc;
			long time;
			com.readFromSocket(clntSock,&time,sizeof(long));
			TotalTime +=time;

			shutdown(clntSock,0);
					int k = close(clntSock);
					if (k < 0) {
							printf("\nError in Closing");
							exit(0);
					}
		}
		shutdown(servSock,2);
			int k = close(servSock);
			if (k < 0) {
				printf("\nError in Closing");
				exit(0);
			}
		float avgMsg = (float)TotalNoMsgs/numOfCSRequests;
		float avgTime = (float)TotalTime/numOfCSRequests;
		avgTime = avgTime/1000;
		printf("TotalNum of Msgs:%ld, TotalTime: %ld\n",TotalNoMsgs,TotalTime);
		printf("Average Number of Messages used: %.3f, tested on %d requests\n",avgMsg,numOfCSRequests);
		printf("Average Time Taken for each Request(ms): %.3f, tested on %d requests\n",avgTime,numOfCSRequests);

	printf("Bye!!!\n");
}




int main()
{

	Controller *c = new Controller;
	pthread_t csListenThread;
	pthread_create(&csListenThread, NULL,csListener, NULL);

	printf("Welcome to Controller Function!!!\n");
	c->initiate(c);
	c->decideAlgorithm();

	pthread_join(csListenThread,NULL);
		
	printf("end of controller\n");
		return 0;
}

