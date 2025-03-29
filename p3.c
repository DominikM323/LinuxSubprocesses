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
	signal(sig, SIG_IGN);
	return;
}

void s4_resender(int sig){//macierzysty rozsyla syg. efektywny do p2
	
	kill(p2,SIGCHLD);
	
	signal(sig, SIG_IGN);
	return;
}

void s4_handler(int sig){//wykonanie sygnalu efektywnego
	//signal(sig, SIG_IGN);
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
	return;
}

int main(void){
	
	//gcc -o p3.out ./p3.c
	
	char in2[100],out2[100];//bufory fifo
	
	int koniec = 0;
	mkfifo("kol",0666);
	int fifo;
	char* w;
	
	//p3=getpid();
	printf("P3: %i , PP: %i\n",getpid(),getppid());
	//sleep(5);
	size_t x =100;
	while(1){
		
		signal(SIGQUIT,s1_sender);//s1 wyslij wyzej
		signal(SIGILL,s4_sender);//s4 wyslij wyzej
		signal(SIGURG,s2_sender);//s2 wyslij
		signal(SIGUSR1,s2_handler);//s2 wykonaj
		signal(SIGCONT,s3_sender);//s3 wyslij
		signal(SIGUSR2,s3_handler);//s3 wykonaj
			
		while(s23 ==1){
			signal(SIGQUIT,s1_sender);//s1 wyslij wyzej
			signal(SIGILL,s4_sender);//s4 wyslij wyzej
			signal(SIGURG,s2_sender);//s2 wyslij
			signal(SIGUSR1,s2_handler);//s2 wykonaj
			signal(SIGCONT,s3_sender);//s3 wyslij
			signal(SIGUSR2,s3_handler);//s3 wykonaj
			
			
			sleep(1);
		}
			
		if(s23 == 0){
			
			fifo = open("kol",O_RDONLY);//otworz fifo do odczytu
			if(read(fifo,out2,x)>0) printf("\nodczyt: %s\n\n",out2);//odczyt fifo
			close(fifo);
				
		}
			
			
			
	}
	
	return 0;
}

