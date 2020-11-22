#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<sys/stat.h>
#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256


typedef struct packet{
	char x[50];
}P;

typedef struct frame{
	int type_of_frame;
	int seq_no;
	int flag;
	int ack;
	P packet;
}F;

int main(){
	struct sockaddr_in sin, newAddr;
	char buf[MAX_LINE];
	socklen_t addr_size;
	int len, check =0;
	int jaydeep =1;
	int jay = 4;
	int s, new_s, recv_wnd = 40;
	int frame_id = 0;
	F frame_send;
	F frame_recv;
	char str[INET_ADDRSTRLEN];
	int cnwd = 1;
	int ssthresh; 
	int temp = 0;

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);
	s = socket(PF_INET, SOCK_STREAM, 0);
	inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
	printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

	bind(s, (struct sockaddr *)&sin, sizeof(sin)); 
	printf("Server bind done.\n");
	listen(s, MAX_PENDING);
	while(1) 
	{
		if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
	 	perror("simplex-talk: accept");
	 	exit(1);
    	}
    	printf("Server Listening.\n");

	  while(1)
	  {
		int f_recv_size ;
		f_recv_size = recv(new_s, &frame_recv, sizeof(struct frame), 0);
		
		if (f_recv_size > 0)
		{
			if(frame_recv.seq_no%recv_wnd == 0 && frame_recv.seq_no!=0 && check == 0)
			{
				printf("-----Frame not Received ------------>>>>>>>>>\n");
				check = 1;
			}
			else if (frame_recv.flag ==1)
			{	
				printf("-----CHECK ---------Frame Received  %d\n", frame_recv.seq_no);
				frame_send.ack = frame_recv.seq_no +3;
				send(new_s, &frame_send, sizeof(struct frame), 0);
				printf("-----CHECK--------- ACK SENT %d\n", frame_send.ack);
				check = 0;
				frame_recv.flag =0;
			}
			
			else
			{
				if(frame_recv.seq_no%recv_wnd == 1 && frame_recv.seq_no!=1)
				{
					temp = 2;
				}
				if(frame_recv.seq_no%recv_wnd == 2 && frame_recv.seq_no!=2)
				{
					temp = 3;
				}
				if(frame_recv.seq_no%recv_wnd == 3 && frame_recv.seq_no!=3)
				{
					temp = 4;				
				}
				printf("-----Frame Received  %d\n", frame_recv.seq_no);	
				frame_send.seq_no = frame_recv.seq_no;
				frame_send.type_of_frame = 0;
				frame_send.ack = frame_recv.seq_no - temp;
				send(new_s, &frame_send, sizeof(struct frame), 0);		
				printf("-----ACK SENT %d\n\n", frame_send.ack);
				temp = 0;
			}
		}
		else
		{
			printf("-----Frame not received\n");
			break;
		}	
		}
	close(s);
    }
}
