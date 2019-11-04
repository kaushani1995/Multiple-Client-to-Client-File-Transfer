#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<sys/stat.h>

#define PORT 5000

struct t
{
	int r_s, id, did, d_u, b_read;
        char buff[1025];
};

int main(int argc, char** argv)
{
	int sockfd, n, count=0, len, fd=0, cout=1, tot=0;
	char filename[80];
	struct t* tag=(struct t*)malloc(sizeof(struct t));
	if(tag==NULL)
	{
		printf("\nError in allocating tag!" );
		return -1;
	}	

	struct sockaddr_in servaddr;
	sockfd=socket(AF_INET, SOCK_DGRAM,0);
	
	if(sockfd<0)
	{
		perror("socket");
		exit(0);
	}
	
	servaddr.sin_family=AF_INET;
	if(inet_pton(AF_INET, "127.0.0.1",&servaddr.sin_addr)<0)
	{
		printf("Invalid Address");
		return -1;
	}
	servaddr.sin_port = htons(PORT);

	len=sizeof(servaddr);
	
	tag->id=0;
	tag->d_u=0;
	tag->did=0;

	n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&servaddr, len);
	n=recvfrom(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&servaddr, &len);

	printf("\nYour ID is: %d", tag->id);
	printf("\nSend(0) or Receive(1) ?");
	scanf("%d", &tag->r_s);
	
	if(tag->r_s==0)		//sender
	{
		printf("\nName of file to be sent: ");
		scanf("%s",filename);
		printf("\nSend to(Client ID) :");
		scanf("%d",&tag->did);
		fd=open(filename, O_RDONLY);
                if(fd==-1)
                {
                	printf("\nError opening file for reading!");
                        return -1;
                }
		tag->b_read=read(fd, tag->buff, 1024);
                if(tag->b_read==-1)
                {
                        printf("\nError in reading!");
                        return -1;
                }
		while(1)
		{
			n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&servaddr, len);
			n=recvfrom(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&servaddr, &len);
			if(tag->b_read==0)
				break;
			if(tag->d_u==1)
			{
				tag->b_read=read(fd, tag->buff, 1024);
         		        if(tag->b_read==-1)
                		{
                     			printf("\nError in reading!");
                        		return -1;
                		}
				//fputs(tag->buff,stdout);
				tag->d_u=0;
			}
		}

	}
	
	else if(tag->r_s==1)		//receiver
	{
		printf("\nName of file to be copied in: ");
		scanf("%s",filename);
		fd=open(filename, O_WRONLY | O_CREAT, 0644);
                if(fd==-1)
                {
                        printf("\nError in opening file for writing!");
                        return -1;
                }

		while(cout>0)
		{
			n=sendto(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&servaddr, len);
			n=recvfrom(sockfd, tag, sizeof(struct t), 0, (struct sockaddr *)&servaddr, &len);
			if(tag->did!=0)
			{
				cout=write(fd, tag->buff, tag->b_read);
				//fputs(tag->buff,stdout);
		                if(cout==-1)
               			{
                        		printf("\nError in writing!");
                        		return -1;
                		}
				tag->did=0;
				tot=tot+cout;
			}
		}
		printf("\nBytes recv=%d\n",tot);
	}
	return 0;

}
