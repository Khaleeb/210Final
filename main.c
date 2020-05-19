/*
 * CS210 Final - Rock Paper Scissors
 * Caleb Davis
 * 5/18/2020
 * main.c
 */

#include<stdio.h>
#include<stdlib.h>
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
#define LEFT 105
#define RIGHT 106
#define ENTER 28

int blue, green, red, blank;
int state, selected;
int bitBuffer[8][8] = {{0}};
int run = 1;
void interruptHandler(){
	run = 0;
}

// Declaration
void error(char *msg);
void clearBitBuffer();
void pushBitBuffer(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);
void drawRock(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);
void drawPaper(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);
void drawScissors(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);
void drawWin(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);
void drawLose(int bitBuffer[8][8], pi_framebuffer_t* framebuffer);


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
		fprintf(stderr, "Usage: %s <port> <server, if applicable>\n", argv[0]);
		exit(0);
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

		server = gethostbyname(argv[2]);
		if(server == NULL){
			error("ERROR no such host\n");
		} else {
			printf("hostname found\n");
		}

		portno = atoi(argv[1]);
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
		int state = 50; // state % 3? 0: Paper, 1: Scissors, 2: Rock
		int selected = 0;
		drawRock(bitBuffer, fb);
		while(run){
			while (!selected){
				if(state % 3 == 0){
					drawPaper(bitBuffer, fb);
				} else if (state % 3 == 1){
					drawScissors(bitBuffer, fb);
				} else if (state % 3 == 2){
					drawRock(bitBuffer, fb);
				}
				sleep(.5);
			}
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


// Draws Rock
void drawRock(int bitBuffer[8][8], pi_framebuffer_t* fb){
	clearBitBuffer();
	for(int i = 0; i < 6; i++){
		bitBuffer[1+i][2] = red;
		bitBuffer[1][2+(i/3)] = red;
		bitBuffer[4][2+(i/3)] = red;
		bitBuffer[3+(i/3)][4+(i/3)] = red;
	}
	bitBuffer[2][5] = red;
	bitBuffer[3][5] = red;
	pushBitBuffer(bitBuffer, fb);
}


// Draws Paper
void drawPaper(int bitBuffer[8][8], pi_framebuffer_t* fb){
	clearBitBuffer();
	for(int i = 0; i < 6; i++){
		bitBuffer[1+i][2] = green;
		bitBuffer[1][2+(i/3)] = green;
		bitBuffer[4][2+(i/3)] = green;
	}
	bitBuffer[2][5] = green;
	bitBuffer[3][5] = green;
	pushBitBuffer(bitBuffer, fb);
}


// Draws Scissors
void drawScissors(int bitBuffer[8][8], pi_framebuffer_t* fb){
	clearBitBuffer();
	for(int i = 0; i < 4; i++){
		bitBuffer[1][2+i] = blue;
		bitBuffer[3][2+i] = blue;
		bitBuffer[6][2+i] = blue;
		bitBuffer[3+i][5] = blue;
	}
	bitBuffer[2][2] = blue;
	pushBitBuffer(bitBuffer, fb);
}

// Draws win
void drawWin(int bitBuffer[8][8], pi_framebuffer_t* fb){
	clearBitBuffer();
	for(int i = 0; i < 2; i++){
		bitBuffer[1+i][1] = blue;
		bitBuffer[3+i][1] = blue;
		bitBuffer[1+i][6] = blue;
		bitBuffer[3+i][6] = blue;
		bitBuffer[3+i][3] = blue;
		bitBuffer[3+i][4] = blue;
		bitBuffer[5+i][2] = blue;
		bitBuffer[5+i][5] = blue;
	}
	pushBitBuffer(bitBuffer, fb);
}

// Draws lose
void drawLose(int bitBuffer[8][8], pi_framebuffer_t* fb){
	clearBitBuffer();
	for(int i = 0; i < 4; i++){
		bitBuffer[1+i][2] = blue;
		bitBuffer[6][2+i] = blue;
	}
		bitBuffer[5][2] = blue;
	pushBitBuffer(bitBuffer, fb);
}
