#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "gnl.h"
#include <string.h>
int checkLineNbr(char c)
{
	if(c == '1')
		return (1);
	else if(c == '2')
		return (2);
	else if(c == '3')
		return (3);
	return (0);
}
int controlLine(char *str)
{
	int i = 0;
	while(str[i])
	{
		if(str[i] == '1')
			return 1;
		else if(str[i] == '2')
			return 2;
		else if(str[i] == '3')
			return 3;
		i++;
	}
	return 0;
}

int dd_strlen(char* str)
{
	int i = 0;
	while(str[i])
	{
		i++;
	}
	return i;
}
void printPage(int sec)
{
	int i = 0;
	int fd;
	int sd;
	int flag = 0;
	char *buff = NULL;
	sd = open("index.html", O_WRONLY);
	fd = open("pages.html",O_RDONLY);
	get_next_line(fd,&buff);
	printf("buff: %s\n", buff);
	while(1)
	{
		if((buff[i] == '_' ) && (buff[i + 1] == '?') && (checkLineNbr(buff[i + 2]) == sec))
		{
			write(1,"allah",5);
			while(1)
			{
				get_next_line(fd,&buff);
				printf("buff: %s\n", buff);
				while(buff[i])
				{
					if(buff[0] == '_')
					{
						write(1,"flag",4);
						flag = 1;
						break;
					}
					else
						write(sd,&buff[i],1);
					i++;
				}
				i = 0;
				if(flag == 1)
					break;
			}
		}
		if(flag == 1)
			break;
		get_next_line(fd,&buff);
	}

	
	close(fd);
	close(sd);
	free(buff);
}
int main()
{
	int fd;
	int sec;
	char *buff;
	while(1)
	{
		while(1)
		{
			sleep(1);
			fd = open("a.json",O_RDONLY);
			get_next_line(fd, &buff);
			if(controlLine(buff) == 1)
			{
				write(1,"test1",5);

				sec = 1;
				close(fd);
				break;
			}
			if(controlLine(buff) == 2)
			{
				write(1,"test2",5);

				sec = 2;
				close(fd);
				break;
			}
			if(controlLine(buff) == 3)
			{
				write(1,"test3",5);

				sec = 3;
				close(fd);
				break;
			}
			close(fd);
		}
		switch (sec)
		{
		case 1:
			write(1,"case1",5);
			printPage(sec);
			break;
		case 2:
			write(1,"case2",5);
			printPage(sec);
			break;
		case 3:
			write(1,"case3",5);
			printPage(sec);
			break;
		
		default:
			break;
		}
	}
}