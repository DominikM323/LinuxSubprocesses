#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
//sygnaly (wysylany przez uzytkownika i przez proces mac dla dzialan s1-s4
//s1 - 3.sigquit, efektywny 2.sigint
//s2 - 23.SIGURG efektywny 10.SIGUSR1 
//s3 - 18.SIGCONT efektywny 12.SIGUSR2
//s4- 4.sigill, efektywny 17.sigchld
int p1,p2,p3;//pid procesow
int s23 = 0;
int s4 = 0; //0-kodowac 1- niekodowac

struct sigaction sa;


//sender - wywolany na child, wysyla sig do macierzysego
//resender - rozsyla sygnal efektywny z parent na potomne
//handler - dzialanie dla sygnalu efektywnego

void s1_sender(int sig){
	kill(getppid(),SIGINT);
	signal(sig, SIG_IGN);
	return;
}

void s1_handler(int sig){// parent zamyka potomne 
	system("echo  zamykam..");
	kill(p1,SIGINT);
	kill(p2,SIGINT);
	kill(p3,SIGINT);
	signal(sig, SIG_DFL);
	return;
}

void s2_sender(int sig){
	//system("echo komunikacja wstrzymana");
	kill(getppid(),SIGURG);
	//signal(sig, SIG_IGN);
	//sigaction(sig,&sa,NULL);
	return;
}

void s2_resender(int sig){
	system("echo  komunikacja wstrzymana");
	kill(p1,SIGUSR1 );
	kill(p2,SIGUSR1 );
	kill(p3,SIGUSR1 );
	//signal(sig, SIG_IGN);
	//sigaction(sig,&sa,NULL);
	return;
}

void s2_handler(int sig){
	s23 = 1;
	//signal(sig, SIG_IGN);
	//sigaction(sig,&sa,NULL);
	return;
}

void s3_sender(int sig){
	//system("echo komunikacja wznowiona");
	kill(getppid(),SIGCONT);
	//signal(sig, SIG_IGN);
	//sigaction(sig,&sa,NULL);
	return;
}

void s3_resender(int sig){
	system("echo  komunikacja wznowiona");
	kill(p1,SIGUSR2);
	kill(p2,SIGUSR2);
	kill(p3,SIGUSR2);
	//signal(sig, SIG_IGN);
	//sigaction(sig,&sa,NULL);
	return;
}

void s3_handler(int sig){
	s23 = 0;
	//signal(sig, SIG_IGN);
	//sigaction(sig,&sa,NULL);
	return;
}

void s4_sender(int sig){//przeslij s4 do macierzystego
	
	kill(getppid(),SIGILL);
	//signal(sig, SIG_IGN);
	return;
}

void s4_resender(int sig){//macierzysty rozsyla syg. efektywny do p2
	//system("echo  bbb");
	kill(p2,SIGCHLD);
	
	//signal(sig, SIG_IGN);
	return;
}

void s4_handler(int sig){//wykonanie sygnalu efektywnego
	//signal(sig, SIG_IGN);
	//system("echo  aaa");
	if(s4 == 0){
		s4=1;
		system("echo  kodowanie zatrzymane");
		//signal(sig, SIG_IGN);
	}else if(s4 == 1){
		s4=0;
		system("echo  kodowanie wznowione");
		//signal(sig, SIG_IGN);
	}
	signal(sig, SIG_IGN);
	signal(SIGCHLD,s4_handler);
	return;
}

int main(void){
	
	sa.sa_handler=SIG_IGN;
	
	int pdes[2];//pipe
	char in[100],out[100];//bufory pipe
	char in2[100],out2[100];//bufory fifo
	pipe(pdes);
	int koniec = 0;
	mkfifo("kol",0666);
	int fifo;
	char* w;
	
	if(p1 = fork()==0){//proc 1
		//p1=getpid();
		printf("P1: %i , PP: %i\n",getpid(),getppid());
		close(pdes[0]);
		char path[100];
		char command[120];
		size_t x =100;
		while(1){
			
			signal(SIGQUIT,s1_sender);//s1 wyslij
			signal(SIGILL,s4_sender);//s4 wyslij
			signal(SIGURG,s2_sender);//s2 wyslij
			signal(SIGUSR1,s2_handler);//s2 wykonaj
			signal(SIGCONT,s3_sender);//s3 wyslij
			signal(SIGUSR2,s3_handler);//s3 wykonaj
			
			printf("podaj sciezke:\n");
			scanf("%s",path);
			
			int n =0;
			while ( path[n]!='\0' && path[n]!='\n' ) n++;
			path[n]='\0';
			
			char* p = realpath(path,NULL);
			
			
			size_t len = 0;
			
			
			strcpy(command,"");
			strcat(command,"ls ");//komenda ls
			strcat(command,path);
			strcat(command," >/home/student/contents.txt");
			system(command);
			
			FILE* f = fopen("contents.txt","r");
			
			if(f==NULL)printf("NULL");
			
			
			int flag = 0;
			while( flag == 0 ){//stworz pelna sciezke i wyslij
				sleep(1);
				
				signal(SIGQUIT,s1_sender);//s1 wyslij
				signal(SIGILL,s4_sender);//s4 wyslij
				signal(SIGURG,s2_sender);//s2 wyslij
				signal(SIGUSR1,s2_handler);//s2 wykonaj
				signal(SIGCONT,s3_sender);//s3 wyslij
				signal(SIGUSR2,s3_handler);//s3 wykonaj
				
				while(s23 == 1){
					signal(SIGQUIT,s1_sender);//s1 wyslij
					signal(SIGILL,s4_sender);//s4 wyslij
					signal(SIGURG,s2_sender);//s2 wyslij
					signal(SIGUSR1,s2_handler);//s2 wykonaj
					signal(SIGCONT,s3_sender);//s3 wyslij
					signal(SIGUSR2,s3_handler);//s3 wykonaj
					sleep(1);
				}
				
				if(getline(&w,&len,f)==-1){
					flag = 1;
					break;
				}
				
				strcpy(in,"");
				strcat(in,p);
				strcat(in,"/");
				strcat(in,w);
				strcat(in,"\0");
				write(pdes[1],in,x);
			}
			
			fclose(f);
			perror("sciezka");
		}
		
	}else if(p2 = fork()==0){//proc 2
		//p2=getpid();
		printf("P2: %i , PP: %i\n",getpid(),getppid());
		close(pdes[1]);
		size_t x =100;
		while(1){
			
			signal(SIGQUIT,s1_sender);//s1 wyslij
			signal(SIGILL,s4_sender);//s4 wyslij
			signal(SIGCHLD,s4_handler);//s4 wykonaj
			signal(SIGURG,s2_sender);//s2 wyslij
			signal(SIGUSR1,s2_handler);//s2 wykonaj
			signal(SIGCONT,s3_sender);//s3 wyslij
			signal(SIGUSR2,s3_handler);//s3 wykonaj
			
			while(s23 ==1){
				
				signal(SIGQUIT,s1_sender);//s1 wyslij
				signal(SIGILL,s4_sender);//s4 wyslij
				signal(SIGCHLD,s4_handler);//s4 wykonaj
				signal(SIGURG,s2_sender);//s2 wyslij
				signal(SIGUSR1,s2_handler);//s2 wykonaj
				signal(SIGCONT,s3_sender);//s3 wyslij
				signal(SIGUSR2,s3_handler);//s3 wykonaj
				
				//sleep(1);
			}
			
			if(s23==0){
				
				read(pdes[0],out,x); //odczyt pipe
				unsigned char strH[200],strH2[200];
				if(out[0]!='\n'){
				
					int i,j;
					size_t n = 0;
    				while ( out[n]!='\0' && out[n]!='\n' ) n++;	//oblicz dlugosc sciezki bez znakow konca linii
				
					if(s4==0){
						for (i = 0, j = 0; i < n; i++, j += 2) {//konwersja
    	   					sprintf((char*)strH + j, "%02X", out[i]);
	    				}
						strH[j] = '\0';
						strcpy(in2,strH);
					}else if(s4==1){
						strcpy(strH,"");
						strH[0]='\0';
						//strcpy(strH2,"kodowanie wstrzymane");
						strcpy(in2,out);
					}
					//perror("p2 dane");
					out[strcspn(out,"\n")] = 0;//usun \n z sciezki jawnej
					if(out[0]!='\0')printf("\n%s -:- %s\n" ,out,strH);//wypisz sciezki
					//system("echo lul");
				
					fifo = open("kol", O_WRONLY);//otworz fifo do zapisu
					write(fifo,in2,x);//zapis fifo
					close(fifo);
				
					out[0]='\0';
					
					
				}
			}
			
		}
		
		
	}else if(p3 = fork()==0){//proc 3 execlp
	
		execlp("./p3.out","p3.out",(char*)NULL);
	
//		//p3=getpid();
//		printf("P3: %i , PP: %i\n",getpid(),getppid());
//		//sleep(5);
//		size_t x =100;
//		while(1){
//			
//			signal(SIGQUIT,s1_sender);//s1 wyslij wyzej
//			signal(SIGILL,s4_sender);//s4 wyslij wyzej
//			signal(SIGURG,s2_sender);//s2 wyslij
//			signal(SIGUSR1,s2_handler);//s2 wykonaj
//			signal(SIGCONT,s3_sender);//s3 wyslij
//			signal(SIGUSR2,s3_handler);//s3 wykonaj
//			
//			while(s23 ==1){
//				sleep(1);
//			}
//			
//			if(s23 == 0){
//			
//				fifo = open("kol",O_RDONLY);//otworz fifo do odczytu
//				if(read(fifo,out2,x)>0) printf("\nodczyt: %s\n\n",out2);//odczyt fifo
//				close(fifo);
//				
//			}
//			
//			
//			
//		}
	
	}else{//proc mac
		
		while(1){
			signal(SIGINT,s1_handler);//s1 wykonaj
			signal(SIGILL,s4_resender);//s4 wyslij nizej
			signal(SIGURG,s2_resender);//s2 wyslij nizej
			signal(SIGCONT,s3_resender);//s3 wyslij nizej
			signal(SIGUSR1,SIG_IGN);
			signal(SIGUSR2,SIG_IGN);
		}
		
	}
	
	return 0;
}

