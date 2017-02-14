#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <string.h>

#define MSG_SIZE 100

void error(char *msg)
{
    perror(msg);
    return;

}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;

        struct sockaddr_in serv_addr;
        struct hostent *server;
	char tokenizer[]="@#";
	char myarray[148]; 
	char mes[80];
	char name[10], receiver[10], active[1000], buffer[256];
	char tempname[10];//kosta here  new variable to help with tokenizing
	int count=0, result, fd, size;//the same here
	char msg[100];
	fd_set readfds, testfds, clientfds;
        if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
                return;
        }
        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0){ 
		error("ERROR opening socket");
	}
	server = gethostbyname(argv[1]);
//        printf("The server's id is: %d", server);
        if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	return;
    	}
    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
    	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,        server->h_length);
    	serv_addr.sin_port = htons(portno);
    	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
	        error("ERROR connecting");
		return;
	}
	bzero(active,1000);//added
        n = read(sockfd,active,999);
        printf("The active users are: %s\n", active);
	printf("You are now connected! What do you want to do?\nPlease enter your name!\n");
	do{
		if(count!=0){
			printf("an error occured.Try again\n");
		}
		fgets(name,10,stdin);///////////////////////
	}while(strlen(name) > sizeof(name));
	size = strlen(name) - 1;//kosta here
	name[size]='\0';
	strcpy(tempname,name);
	strcat(tempname,"@#@#");
	printf("the name is:%s\n", name);
	n = write(sockfd,name,strlen(name));
	        do{//added
                printf("The firts message is:%s", msg);
                bzero(msg,1000);
                n =read(sockfd,msg,99);
                printf("the msg is:%s", msg);
                if(strcmp(msg,"accepted") ==0){
                        printf("The name was succesfylly submitted!\n");
                }else{
                        printf("Name already in use.Choose another\n");
                        fgets(name,10,stdin);
                        size = strlen(name)-1;
                        name[size]='\0';
                        strcpy(tempname,name);
                        strcat(tempname,"@#@#");
                        n = write(sockfd,name,strlen(name));
                }
        }while(strcmp(msg,"exists") == 0);

	//initializes the file descriptor clientfds to have zero bits for all file descriptors
	FD_ZERO(&clientfds);
	//sets the bit for the file descriptor sockfd in the file descriptor clientfdset
	FD_SET(sockfd, &clientfds);
	//sets the bit for the stdin in the file descriptor clientfds
	FD_SET(0, &clientfds);
	//wait for the server to respond
	//send list of active users !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	n = read(sockfd, active, sizeof(active -1));

	printf("the returned function is: %s\n", active);
	printf("You are ready. Type something to send a message or 'quit' to quit\n");
	while(1){
		testfds=clientfds;
		//specifies if file descriptor testfds is ready for reading
		select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
		for(fd=0;fd<FD_SETSIZE;fd++){
			//returns a non-zero value if the bit for the file descriptor fd is set in the file descriptor testfds, and 0 otherwise
			if(FD_ISSET(fd,&testfds)){
				//accepts data from open socket
				if(fd == sockfd){
					result = read(sockfd,msg, sizeof(msg -1));
					printf("The msg is: %s", msg);
				}
				else if(fd ==0){//process keyboard activity
					fgets(mes,sizeof(mes), stdin);
					if(strcmp(mes,"quit\n") ==0){
						write(sockfd,mes,sizeof(msg));
						//read
						close(sockfd);
						printf("You are now quitting!\n");
						return(0);
					
					}else{//and here
						size = strlen(mes)-1;
						mes[size]='\0';
						printf("You typed something: %s\n", mes);
						printf("Type the receiver\n");
						fgets(receiver,10,stdin);
						size = strlen(receiver) - 1;
						receiver[size]='\0';
						strcpy(msg, name);
						strcat(msg, tokenizer);
						strcat(msg,receiver);
						strcat(msg,tokenizer);
						strcat(msg,mes);
						printf("The message is: %s", msg);
						n = write(sockfd,mes,strlen(mes));
						if(n<0){
							error("Error sending message");
						}
					}
				}
				else{
//					continue;
					printf("else\n");
				}
			}
		}
	}


	//the system needs name, receiver, and message content
	do{//if the data given is bigger than the data that can be sent
		if(count!=0){
			printf("An error occured.Type again.Please mind the size of the fields\n");
		}
		 //code to get all the necessary information for the message struct
		printf("Please enter the receiver (32 characters):\n");
		scanf("%s",receiver);
		printf("Now type your message (80 characters):\n");
		getchar();//getchar is used to ignore characters left by scanf() not ideal, but experimental 
		fgets(mes,80,stdin);

	}while(strlen(name) > sizeof(name) || strlen(receiver) > sizeof(receiver) || strlen(mes) > sizeof(mes));
	//code to change if strlen and sizeof are equall is to be added here
	bzero(myarray, 148);
	//store inputs into array
	strcpy(myarray, name);//adds name
	strcat(myarray, tokenizer);//adds special tokenizer @# see variable declaration part
	strcat(myarray, receiver);//adds receiver
	strcat(myarray, tokenizer);
	strcat(myarray,mes);//adds message
	printf("%d", sizeof(myarray));
	printf("tokenized array is: %s", myarray);//check the sequence to be send
    	n = write(sockfd,myarray,strlen(myarray));
	if (n < 0){ 
        	error("ERROR writing to socket");
	}
	

    	bzero(buffer,256);
    	n = read(sockfd,buffer,255);
    	if (n < 0){ 
        	error("ERROR reading from socket");
	}
    	printf("%s\n",buffer);
    	return 0;
}
/*
	char choice[20];
	scanf("%s", choice);//get input to perform action
	if(strcmp(choice,"quit\n") == 0){//check
		sprintf(sockfd, choice,strlen(choice));
		close(sockfd);//close current client
		exit(0);
	}else if(strcmp(choice,"send\n") ==0){
		printf("Type your message!\m");
		...........
	}else{
		printf("invalid action\n");
	}


*/
