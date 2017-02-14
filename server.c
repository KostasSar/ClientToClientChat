/* A simple server in the internet domain using TCP/
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

#define MAX_CLIENTS 100
#define MSG_SIZE 80

typedef struct {				//client structure
	char name[10];				//client name
	int unique_id;				//client unique id
	int free;
}client;

client clients[MAX_CLIENTS] = { { "", 0, 0 } };	//array of active clients declaration and initialization

void error(char *msg)
{
    perror(msg);
    return;
}

//method to display all active users
void display_users(int sockfd){
	int counter=0;
	char s[1000];//size to be changed

	int n;
	printf("Printing active users!\n");
//	while(counter < MAX_CLIENTS && clients[counter].unique_id != 0){//iterate through array of clients till end or empty cell is reached
	//	printf("The name is:%s, the counter is:%d\n", clients[counter].name, counter);
		//sprintf(s, "%s\n", clients[counter].name);
//		if(counter ==1){
//			strcpy(s, clients[counter].name);
//		}else{
//			strcat(s, clients[counter].name);
//		}
//		printf("%s  %d", clients[counter].name, &counter);
//		counter++;
//	}
//	printf("HERE MALAKA: %s", s);
	bzero(s, 1000);
	sprintf(s,"hello");
	n = write(sockfd, s, strlen(s));
}

 int unique_name(char* name){//part of the check for unique client names
	int counter=1;
	while(counter<=MAX_CLIENTS && clients[counter].unique_id !=0){//iterate array
		if(strcmp(clients[counter].name,name) == 0){
			printf("User name already exists\n");
			return -1;//false 
		}
		counter++;
	}
	return 0;//true
}

//method to find specific user, in order to send the message
int find_user(char* name){
	int counter=0;
	while(counter<MAX_CLIENTS && strcmp(clients[counter].name,name) !=0){//iterate array until matching name is found
		//possible change to print message to the client
		if(counter == MAX_CLIENTS){ //match was not found and the whole array was iterated
		//printf("Match was not found\n");
			return -1;//not found equals false
		}else{//match was found and there is 
			//printf("Match was found");
			int receiver_id;
			if(strcmp(clients[counter].name,name) == 0){
				receiver_id = clients[counter].unique_id;		
			}	
			return receiver_id;//found equals true
		}
		counter++;
	}
	
}

//code to remove user from list of active users when he disconnects
void remove_user(char* name){
	//code of display users must also be updated, or shuffle array of active users elements to the left
	int counter=1;
	int index;
	while(counter<=MAX_CLIENTS && clients[counter].unique_id !=0){//iterate through the array
		if(strcmp(clients[counter].name,name) ==0 ){//user found
			printf("user removed\n");
			index=counter;
			while(index<MAX_CLIENTS && clients[index + 1].unique_id !=0 ){//sicne one element is removed <=  is not necessary
				clients[index] = clients[index + 1];
				index++;
			}
		}
	}
}


int main(int argc, char *argv[])
{
	//delet buffer, myarray


	int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        struct hostnet *a;
	int mycounter=0, id;    //variables i added myself
	char name[10];
	fd_set readfds, testfds,clientfds;
	int result;
	char msg[100];
	
	//variables used in forwarding message part
	char receiver[10], temp[80], *temp_pointer = temp;
	int position, receiver_id;
	
        if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
                return;
        }
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
		error("ERROR opening socket");
 	}
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ 
		error("ERROR on binding");
	}
        listen(sockfd,5);
	//initializes the file descriptor readfds to have zero bits for all file descriptors
	FD_ZERO(&readfds);
	//sets the bit for the file descriptor sockfd in the file descript or clientfdset
	FD_SET(sockfd, &readfds);
	//sets the bit for the stdin in the file descriptor readsfds
	FD_SET(0, &readfds);
	int fd;
	
	printf("The server is now running..........\n");//server is running
	while(1){//wait for server to respond
		testfds = readfds;
		//specifies if the file descriptor testfds is ready for reading
		select(FD_SETSIZE, &testfds, NULL,NULL,NULL);
		for(fd=0;fd<FD_SETSIZE;fd++){
			//accept new connection request
			if(fd == sockfd){
				clilen = sizeof(cli_addr);
				newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
				if(newsockfd < 0){
					error("ERROR on accept\n");
				}
				printf("New client has connected\n");
				display_users(newsockfd);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				bzero(name, 32);
				n = read(newsockfd, name, 31);//read from client his name
				if(n<0){//error occured
					error("ERROR reading from socket\n");
				}
				if(mycounter<MAX_CLIENTS){//if the array still has some space
					char *tok;
					tok = strtok(name,"@#");
					
					client temp; //save client
					strcpy(temp.name, tok);
					temp.unique_id=newsockfd;
					temp.free = 0;
					clients[mycounter]=temp;
					printf("the name is: %s the id is: %d\n", name, newsockfd);
					mycounter++;
				}else{//array is full
					printf("We cannot support client\n");
					write(newsockfd,"The server is full, please try later!\n",38);
					close(newsockfd);
				}

			}
	//this part compiles but the first if is negated have to change if conditions
/*			else if(fd == 0){//process user's innput
				continue;
			}
			else if(fd){//process client specific activity
				result = read(fd,msg,sizeof(msg -1));//read message
				if(result == -1){
					error("Error reading client's action!\n");
					//break;
				}
				if(strcmp(msg,"quit\n")==0){
				//	remove_user();
					printf("client is leaving %d", fd);
			
	
				}else{
					printf("client has typed something\n");
					result = read(fd,msg,sizeof(msg-1));

					//prints the whole message, including sender and receiver
					printf("%s",msg);

					
					//keeps only the receiver part
					msg_pointer = strstr(msg , "@#");
					strcpy(temp, msg_pointer + 2);
					msg_pointer = strpbrk(temp, "@");
					position = msg_pointer - temp_pointer;
					memcpy(receiver, temp, position);	
					printf("The receiver is\n%s\n", receiver);

					//finds the receivers unique id and fowards him the message
					receiver_id = find_user(receiver);
					write(receiver_id, msg, MSG_SIZE);
				}
			}*/
		}
	}
        return  0;
}
