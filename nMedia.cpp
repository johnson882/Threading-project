// Ian Johnson
// Operating systems
// 5/31/2013
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <ctime>       /* time */
#include <cstdlib>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <fstream>
#include <string>
#include <unistd.h>


using namespace std;

/*
this is the player class the keeps track of the player. 
textFile[] holds the data of the file.
playStat holds the status of the player it can be p for play forward, r for rewind.

 */



string textFile[1000];
ifstream  myfile;
char playStat;
double playRate;
int endFile;
int currentToken;
int endProgram;

// creates the mutex table
pthread_mutex_t table = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

// prototypes
void* player(void* parameter);
void* controll(void *parameter);
void* ControllerSE(void *parameter);

void exit()
{
  endProgram = 1;
};

//loads a text file into the player
void loadFile( const string aFile)
{
 
  myfile.open(aFile.c_str());
      if (!myfile){
      cout << "no such file exists";
      exit(1);
    }
     string aString;
    int i = 0;
    int k = 0;
    while(myfile >>  aString){

    textFile[i] = aString;
      //  sleep(1);
      i++;
    }
    myfile.close();
    endFile = i;
    currentToken = 0;
    playRate = 1000000;
    playStat = 'p';
    cout << "file open!" << endl;
  

};

// sets the rate for how fast the words will be printed out
void SetRate(float rate)
{
  double r;
  r =  (double)(rate);
  r = (1000000) * rate;
  playRate = r;
  cout << r << endl;
}

//sets token to a place in the array
void SeekTo(int byteoffset)
{
  currentToken = byteoffset; 
}

// starts the player from a stoped position
void Start()
{
  playStat = 'p';
}

//stops the player from a start position
void Stop()
{
  playStat = 's';
}

// rewinds the player
void Rewind()
{
  currentToken = 0;
}



int main()
{
    string input;

    // initializes the sum and starts the srand for rand() based off machine tim
   
    srand(time(0));
    endProgram = 0; // if 1 then program ends
 


    // the names of the threads
    pthread_t playerT,controlUser, controlStart, controlEnd;

    // initiates the pthread mutex
    pthread_mutex_init(&table,NULL);

   
    
     cout << "what file do you want to use?[in the same folder.]" << endl;
     getline(cin, input);
  
  
     loadFile(input);
    

       cout << "[legend:(space) for start/stop, (r) to rewind to the beginning, (s) to change time, (l) loads a new file, (t)fastforward - for rewind,(x) to exit ]" << endl;
    pthread_create(&playerT, NULL, player,NULL);

    pthread_create(&controlUser, NULL, controll, NULL);
       pthread_create(&controlStart, NULL, ControllerSE, NULL);
      pthread_create(&controlEnd, NULL, ControllerSE, NULL);

    //  joins and kills threads
     pthread_join(playerT, NULL);
      pthread_join(controlUser,NULL);
      pthread_join(controlStart,NULL);
     
   
    
   
    cout << "done" << endl;

return 0;

}

// accepts a file as parameter, loads file into a array and plays aray one word at a time.
void* player(void* parameter)
{
  while(true)
    {
    while(currentToken <= endFile && playRate > 0) // 
    { 
      if(endProgram ==1)
	break;
      while(playStat == 's')
        {}
        if(playRate < 0)
	  break;
      pthread_mutex_lock(&table);
      cout <<  textFile[currentToken] << endl;
      currentToken++;
         pthread_mutex_unlock(&table);
      if(playRate < 0)
	  break;
      usleep(playRate);
      if(currentToken == endFile)
	{
	  cout << "end of file" << endl;
        endProgram = 1;
	exit(1);
	}
      if(endProgram == 1)
      break;

    }
    if(endProgram == 1)
      break;
   

    while(currentToken >= 0 && playRate < 0 )
    {
      if(endProgram == 1)
      break;
     if(playRate > 0)
	  break;
        while(playStat == 's')
	{}
        pthread_mutex_lock(&table);
        cout <<  textFile[currentToken] << endl;
        currentToken--;
        pthread_mutex_unlock(&table);
        if(playRate > 0)
	  break;
        usleep(playRate*(-1));
        if(currentToken == 0)
	  endProgram = 1;
    }
    if(endProgram == 1)
      break;
    }
 
    endProgram = 1; 


};

// checks input during program
void* controll(void* parameter)
{
  int toggle;
   
  char getL = 'e';
  string input = "";
  int a;
  float b;
  while(true)
    {
      if(endProgram == 1)
	break;
      getL = 'e';
      input = "";
      a = 0;
      b = 1;
   while(true)
     {
       if(endProgram ==1 )
	 break;
     getL = getchar();
 if('r' == getL)
   {
  pthread_mutex_lock(&table);
     Rewind();
            pthread_mutex_unlock(&table);
      break;
   }  

  if(' '== getL && playStat == 'p') 
   {
      pthread_mutex_lock(&table);
  Stop();
           pthread_mutex_unlock(&table);
   break;
   }
  if(getL == 'x')
    {

      exit();
    }

  if(getL == 'l')
  {
 pthread_mutex_lock(&table);
 //  Stop();
    cout << "file you want loaded?" << endl;
    while(getline(cin,input))
      { 
        if(input != "" )
	  break;
      }
  
    loadFile(input);
 
            pthread_mutex_unlock(&table);
	    break;
  }


  if(getL == 't')
   {
     pthread_mutex_lock(&table);
     cout << "what rate? [a number between 1 and zero, closer to zero for faster]" << endl;
     cin >> b;
     SetRate(b);
     pthread_mutex_unlock(&table);
     Start();
     break;
   }

  if(getL == 's')
    {
       pthread_mutex_lock(&table);
       //  Stop();
      cout << "time?" << endl;
      cin >> a;
       SeekTo(a);
       //   Start();
   pthread_mutex_unlock(&table);
       break;
    }
  // cout << playStat << endl;
  if(getL == ' '&& playStat == 's' ) 
 {
    pthread_mutex_lock(&table);
    //  playStat = 'p';
   // cout << "should be starting!" << endl;
  Start();
   pthread_mutex_unlock(&table);
   break;
 }
       
    }

  if(endProgram == 1)
    break;
    }

   

};




