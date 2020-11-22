#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
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

	  int main(int argc, char * argv[]){
	  struct hostent *hp;
	  struct sockaddr_in sin;
	  char *host;
	  char buf[MAX_LINE];
	  int s;
	  int k=0;
	  int addr_size = sizeof(sin);
	  int temp = 0; 
	  int check = 0;
	  int adj = 1;
	  int num = 4;
	  FILE *plot;
	  FILE *jd;
	  int len;
	  int aa = 1;
	  int yy = 1;
	  int frame_id = 1;
	  int ssthresh = 20;
	  int cwnd;
	  int recv_wnd = 40;
	  F frame_send;
	  F frame_recv;
	  plot = fopen ("cnwdVsRTT.txt","w");
	  jd = fopen("ackvsRTT.txt","w");
	  int ack_recv = 1;
	  if (argc==2) {
	    host = argv[1];
	  }
	  else {
		    fprintf(stderr, "usage: simplex-talk host\n");
		    exit(1);
	  }
	  hp = gethostbyname(host);
	  if (!hp) {
		    fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
		    exit(1);
	  }
	  else
	    printf("Client's remote host: %s\n", argv[1]);

		  bzero((char *)&sin, sizeof(sin));
		  sin.sin_family = AF_INET;
		  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
		  sin.sin_port = htons(SERVER_PORT);

		  s = socket(PF_INET, SOCK_STREAM, 0);
	    	  printf("Client created socket.\n");

	  	  connect(s, (struct sockaddr *)&sin, sizeof(sin));
	    	  printf("Client connected.\n");

		  while(1){
		 if (ack_recv = 1)
		 {
				frame_send.type_of_frame = 1;
				frame_send.ack=0;
				for (aa=1 ; aa <= yy && aa <= recv_wnd && k<=50; aa++)
				{	
					if(frame_send.seq_no%recv_wnd == 3 && frame_send.seq_no != 3)
					{	
						printf("3 duplicate ACK received\n");
						printf("------------------>>>>>>>>>>>>>>CONGESTION DETECTED<<<<<<<<<<<<<<------------------\n");
						printf("-------------------------RTT NO: %d \n",k);
						printf("------------Current Window Size: %d \n",yy);
						fprintf(plot, "%d,", k);
						fprintf(plot, "%d\n", yy);
						ssthresh = yy/2;
						yy =1;
						aa = 1;
						printf("------------------New Threshold: %d\n\n",ssthresh);
						temp=0;
						ack_recv = 0;
					}	
					if(frame_id%recv_wnd==4 && frame_id!=4)
					{
						frame_send.seq_no=frame_send.seq_no - 3;
						frame_send.flag = 1;
						send(s, &frame_send, sizeof(struct frame), 0);
						printf("-------Frame sent after Congestion : %d\n",frame_send.seq_no);
						recv(s, &frame_recv, sizeof(struct frame), 0);
						printf("----ACK recveived after Congestion : %d\n",frame_recv.ack);
						frame_send.seq_no=frame_send.seq_no + 4;
						fprintf(jd,"%d,",frame_recv.ack);
						fprintf(jd, "%d\n", k);
						frame_send.flag = 0;
					}	
					frame_send.seq_no = frame_id;
					printf("----------Frame sent : %d\n",frame_send.seq_no);
					frame_send.flag =0;
					send(s, &frame_send, sizeof(struct frame), 0);
					frame_id++;
					cwnd = yy;
					
					if(frame_send.seq_no%recv_wnd==0 && frame_send.seq_no != 0)
					{
						k++;
						printf("-------------------------RTT NO: %d \n",k);
						printf("------------Current Window Size: %d \n",cwnd);
						printf("-------ACK not recveived --------->>>>>>>> \n");
					}
					else
					{
						len=recv(s, &frame_recv, sizeof(struct frame), 0);
						printf("-------ACK recveived : %d\n\n",frame_recv.ack);
						fprintf(jd,"%d,",frame_recv.ack);
						fprintf(jd, "%d\n", k+1);
						ack_recv = 1;	
					}	
				}
				k++;
				printf("-------------------------RTT NO: %d \n",k);
				fprintf(plot, "%d,", k);
				printf("------------Current Window Size: %d \n",cwnd);
				fprintf(plot, "%d\n", cwnd);
				if(k==50)
				{
					break;
				}
				if (yy*2 > ssthresh)
				{
					yy = ssthresh + temp;
					temp++;
				}
				else
				{
				 	yy=yy*2;
				}		
		}
	}
		fclose(plot);
		fclose(jd);
		close(s);
}
