#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
 #include <arpa/inet.h>


typedef struct{
    unsigned short Id;
    //flags
    unsigned char QR : 1;
    unsigned char Opcode : 4;
    unsigned char AA : 1;
    unsigned char TC: 1;
    unsigned char RD: 1;
    unsigned char RA : 1;
    unsigned char Z : 3;
    unsigned char Rcode : 4;

    unsigned short Ans_Count;
    unsigned short Auth_count;
    unsigned short Add_count;

} Dns_header_Que_query;


char question[256];
int question_size;

char sendPacket[512];