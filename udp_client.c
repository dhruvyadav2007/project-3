// complie using
//			gcc udp_server.c -o udp_server
//			gcc udp_client.c -o udp_client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {

    int ret,len,recv_len;
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    char send_buffer[1024];
    char recv_buffer[1024];
    char user_input[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    // The "servaddr" is the server's address and port number,
    // i.e, the destination address if the client needs to send something.
    // Note that this "servaddr" must match with the address in the
    // UDP server code.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(32000);


    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliaddr.sin_port = htons(32111);
    bind(sockfd,
         (struct sockaddr *) &cliaddr,
         sizeof(cliaddr));
   
    while (1) {

        // The fgets() function read a line from the keyboard (i.e, stdin)
        // to the "send_buffer".
        fgets(user_input,
              sizeof(user_input),
              stdin);

        // m is a variable that temporarily holds the length of the text
        // line typed by the user (not counting the "post#" or "retrieve#".
        int m = 0;

        // Compare the first five characters, check input format.
        // Note that strncmp() is case sensitive.
if (strncmp(user_input, "exit", strlen("exit")) == 0){
            break;
        }else if (strncmp(user_input, "post#", 5) == 0) {

            // Now we know it is a post message that should be sent.
            // Extract the input text line length, and copy the line to
            // the payload part of the message in the send_buffer. Note
            // that the first four bytes are the header, so when we
            // copy the input line of text to the destination memory
            // buffer, i.e., the send_buffer + 4, there is an offset of
            // four bytes after the memory buffer that holds the whole
            // message.

            // Note that in C and C++, array and pointer are interchangable.

            m = strlen(user_input) - 5;
            memcpy(send_buffer + 4, user_input + 5, m);
            if (m >0 && m<200){
                send_buffer[0] = 0x44; // These are constants you defined.
                send_buffer[1] = 0x59;
                send_buffer[2] = 0x01;
                send_buffer[3] = m;
            } else{
                printf("Error: Unrecognized command format\n");
                continue;
            }

        } else if (strncmp(user_input, "retrieve#", 9) == 0) {
            m = strlen(user_input) - 9;
            if (m==1){
                send_buffer[0] = 0x44; // These are constants you defined.
                send_buffer[1] = 0x59;
                send_buffer[2] = 0x03;
                send_buffer[3] = m;
            }else{
                printf("Error: Unrecognized command format\n");
                continue;
            }

            
        } else {
            printf("Error: Unrecognized command format\n");

            // If it does not match any known command, just skip this
            // iteration and print out an error message.

            continue;

        }
        // The sendto() function send the designated number of bytes in the
        // "send_buffer" to the destination address.
        ret = sendto(sockfd,                   // the socket file descriptor
                     send_buffer,                    // the sending buffer
                     m + 4, // the number of bytes you want to send
                     0,
                     (struct sockaddr *) &servaddr, // destination address
                     sizeof(servaddr));             // size of the address

        if (ret <= 0) {
            printf("sendto() error: %s.\n", strerror(errno));
            return -1;
        }
        memset(recv_buffer, 0,sizeof(recv_buffer));
        len = sizeof(cliaddr);
        recvfrom(sockfd, // socket file descriptor
                 recv_buffer,       // receive buffer
                 sizeof(recv_buffer),  // max number of bytes to be received
                 0,
                 (struct sockaddr *) &cliaddr,  // client address
                 &len);
        fflush(stdin) ;
        printf("%s\n",recv_buffer+4);


    }
    close(sockfd);
    return 0;
}


