#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<unistd.h>

#define PORT 5000

struct t
{
	int r_s, id, did, d_u, b_read;
	char buff[1025];
};

int main()
{	int count=0, sockfd, len, n, ifd, readb, cid=1, flag=1, sid=0, rid=0;
	char sbuff[1025];
	struct t* tag=(struct t*)malloc(sizeof(struct t));
	if(tag==NULL)
	{
		printf("\nCould not alloc requested size!");
		return -1;
	}

	struct sockaddr_in servaddr, cliaddr;
	sockfd=socket(AF_INET, SOCK_DGRAM, 0);
	
	if(sockfd<0)
	{
		perror("socket");
		exit(0);
	}

	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	if((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))<0)
	{
		perror("bind");
		exit(0);
	}
	
	len=sizeof(cliaddr);

	while(1)
	{
		n=recvfrom(sockfd, tag, sizeof(struct t), 0, (struct sockaddr*)&cliaddr, &len);
		if(n==-1)
	                {
        	                printf("\nError in receiving file!");
                	        return -1;
               		}

		if(tag->id==0)		//new client
		{
			tag->id=cid;
			cid++;
			n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&cliaddr, len);
			if(n==-1)
	                {
        	                printf("\nError in sending file!");
                	        return -1;
               		}
		}

		else if(tag->id!=0)		//old client
		{
			if(tag->r_s==0)		//sender
			{
				if(flag==1)		//buffer is free
				{
					strcpy(sbuff,tag->buff);
					//fputs(tag->buff,stdout);
					readb=tag->b_read;
					rid=tag->did;
					sid=tag->id;
					tag->d_u=1;
					n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&cliaddr, len);
					flag=0;
				}
					
				else if(flag==0)	//buffer not free
					n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&cliaddr, len);

			}
			
			else if(tag->r_s==1)	//receiver
			{
				if(tag->id==rid)
				{
					tag->did=sid;
					tag->b_read=readb;
					strcpy(tag->buff,sbuff);
					n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&cliaddr, len);
					rid=sid=0;
					flag=1;
				}
				else
					n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&cliaddr, len);

			}
		}
	
	}
	
	return 0;
}	
