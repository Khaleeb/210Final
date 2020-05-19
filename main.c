/*
 * CS210 Final - Rock Paper Scissors
 * Caleb Davis
 * 5/18/2020
 * main.c
 */

#include<stdio.h>
#include<signal.h>
#include<time.h>
#include<sense/sense.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>

// Constants & Global Stuff
#define BUFFSIZE 4096
int blue;
int green;
int red;
int blank;
int bitBuffer[8][8] = {{0}};
int run = 1;
void interruptHandler(){
	run = 0;
}

// Declaration
void error(char *msg);
void clearBitBuffer();
void pushBitBuffer(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);


// Main function
int main(int argc, char** argv){
	int sockfd, portno, n;
	char buffer[BUFFSIZE];
	struct sockaddr_in serv_addr;

	// Set Colors
	blue = getColor(0, 0, 255);
	green = getColor(0, 255, 0);
	red = getColor(255, 0, 0);
	blank = getColor(0, 0, 0);

	// Set up framebuffer
	pi_framebuffer_t* fb = getFBDevice();
	clearBitmap(fb->bitmap,blank);

	//Checks arguments
	if(argc != 2 && argc != 3){
		error("Usage: %s <port> <server, if applicable>", argv[0]);
	} else if ( argc == 2 ){
		// Server pi stuff
		int clien;
		struct sockaddr_in cli_addr;

		portno = atoi(argv[1]);
		printf("Server port number set to %d", portno);

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd < 1){
			error("ERROR opening socket");
		} else {
			printf("Socket opened");
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
			error("ERROR on binding");
		} else {
			printf("Binding successful");
		}
	} else {
		// Client pi stuff
		struct hostent *server;
		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sockfd < 0){
			error("ERROR opening socket\n");
		} else {
			printf("socket opened\n");
		}

		server = gethostbyname(argv[1]);
		if(server == null){
			error("ERROR no such host\n");
		} else {
			printf("hostname found\n");
		}

		portno = atoi(argv[2]);
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(portno);

		if(connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0){
			error("ERROR connecting");
		} else {
			printf("connection success\n");
		}
	}

	// Main game loop
	if(fb){
		signal (SIGINT, interruptHandler);
		while(run){

		}
		freeFrameBuffer(fb);
		return 0;
	}
	return 1;


}



// Error message
void error(char *msg){
	perror(msg);
	exit(0);
}

// Clears bitmap
void clearBitBuffer(){
	for(int r = 0; r < 8; r++){
		for(int c = 0; c < 8; c++){
			bitBuffer[r][c] = blank;
		}
	}
}

// Pushes bitmap to Pi's framebuffer
void pushBitBuffer(int bitBuffer[8][8], pi_framebuffer_t* fb){
	for(int r = 0; r < 8; r++){
		for(int c = 0; c < 8; c++){
			fb->bitmap->pixel[r][c] = bitBuffer[r][c];
		}
	}
}
