#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

int main (void){
	
	while(1){
		
		printf("\npodaj liczbe hex:\n");
	
		int i =0;
		int j =0;
	
		char in[200];
		char mid[100][2];
		int midt[100];
		char out[100];
		char str[2];
	
		while(i<100){
			out[i]='\0';
			midt[i]=0;
			i++;
		}
		scanf("%s",&in);
		i =0;
		j=0;
		strcat(in,"\n");
	
		while(j<200 &&in[j]!='\n'&& in[j+1]!='\n'&&in[j]!='\0'&& in[j+1]!='\0'){
			
			mid[j/2][0]=in[j];
			mid[j/2][1]=in[j+1];
		
			j+=2;
		}
		j=0;
		printf("\n");
		while(i<100){
			str[0]=mid[i][0];
			str[1]=mid[i][1];
			
			midt[i]=0;
			midt[i]=strtol(str,NULL,16);

			if(midt[i]>16)printf("%c",midt[i]);
			i++;
		}
		
	}
	
	
	
}
