//assignment 4
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <cstdlib>
#include <sstream>
using namespace std;
float probability;

int bet = 1;
int numstates = 0;
struct state{
	int sum;
	bool phasace; //player has ace?
	int dsum; //sum of cards of dealer
	bool dhasace; //dealer has ace?
	int numcards; //special when this is 2
	bool bj; //player got blackjack?
	int pair; //is the player has a pair and if so then which?
	bool turn; //true means my turn, false means dealer's turn.
	//float v=0; //the V of the state
};

/* a mapping from integers to states      */
//vector<pair<int,state>> mappings;

/* end of the mapping function*/


// allstates contains the list of all the states
// initializing the initial state space of 330 states
vector<state> allstates;
void sspace()
{
	int count = 0;
	vector<state> states;

	for(int i=5; i<=19; i++)
	{
		for(int j=2;j<=11;j++) //j = 11 means that dealer has ace
		{
		  state s;
		  s.sum = i;
		  s.phasace = false;
		  s.dsum = (j==11)?10:j;
		  s.dhasace = (j==11)?true:false;
		  s.numcards = 2;
		  s.bj = false;
		  s.pair = false;
		  s.turn = true;
		  allstates.push_back(s);
		  //mappings.push_back(make_pair(count,s));
		  count++;
	    }
	}

	for(int i=2; i<=9; i++)
	{
		for(int j=2;j<=11;j++) //j = 11 means that dealer has ace
		{
		  state s;
		  s.sum = i+11;
		  s.phasace = true;
		  s.dsum = (j==11)?10:j;
		  s.dhasace = (j==11)?true:false;
		  s.numcards = 2;
		  s.bj = false;
		  s.pair = false;
		  s.turn = true;
		  allstates.push_back(s);
		  //mappings.push_back(make_pair(count,s));
		  count++;
	    }
	}


	for(int i=2; i<=10; i++)
	{
	   for(int j=2;j<=11;j++) //j = 11 means that dealer has ace
		{
		  state s;
		  s.sum = i+i;
		  s.phasace = false;
		  s.dsum = (j==11)?10:j;
		  s.dhasace = (j==11)?true:false;
		  s.numcards = 2;
		  s.bj = false;
		  s.pair = true;
		  s.turn = true;
		  allstates.push_back(s);
		  //mappings.push_back(make_pair(count,s));
		  count++;
	    }	
	}

	state s;
	for(int j=2;j<=11;j++) //j = 11 means that dealer has ace
		{
		  state s;
		  s.sum = 12;
		  s.phasace = true;
		  s.dsum = (j==11)?10:j;
		  s.dhasace = (j==11)?true:false;
		  s.numcards = 2;
		  s.bj = false;
		  s.pair = true;
		  s.turn = true;
		  allstates.push_back(s);
		  //mappings.push_back(make_pair(count,s));
		  count++;
	    }

	    cout<<"count is \n"<<count;
	    numstates = count;
}

// finished the initializing the initial state space of 330 states


enum action {hit, stand, dd, split, dealer};
action actions[5];
//declaring v k-1, every index corresponds to the state number 
vector<int> vkm1;

state mycurrstate, mynextstate, dealercurrstate, dealernextstate;

float reward(state s){
 if((s.turn)&&(s.sum<=21)) //means it is my turn then it is a non terminal state!!!!
	return 0;

 if(s.bj) 
	{ if((s.dsum==21)&&(s.dhasace))
		return 0;       //push

	  else 
	  	return 1.5;   //blackjack
	  }

 else if (s.sum > 21)  //player busted
	{return -1;}

 else if (s.dsum > 21) //dealer busted
	{return 1;}

 else if (s.dsum > s.sum) //dealer's sum is more than that of player's
	{return -1;}

 else if (s.sum > s.dsum) //player's sum is more than that of dealer's
	{return 1;}

 else if (((s.dsum==21)&&(s.dhasace))&&(!s.bj)&&(s.sum == 21))
	{return -1;}

 else if (s.dsum == s.sum)
	{return 0;}

}

//checks if the given state already exists, if not then inserts and makes the mapping
//it will also zero out its v value
//it will also return the v value of that state
float checkninsert(state s)
{
	bool found = false;
	for(int i=0;i<numstates;i++)
	{
      if((s.sum==allstates[i].sum)&&(s.phasace==allstates[i].phasace)&&(s.dsum==allstates[i].dsum)&&(s.dhasace==allstates[i].dhasace)&&(s.numcards==allstates[i].numcards)&&(s.bj==allstates[i].bj)&&(s.pair==allstates[i].pair)&&(s.turn==allstates[i].turn))
      	{found = true;

      		return vkm1[i];}
      else 
      	continue;
	}

	if(!found)
	{
		allstates.push_back(s);
		//mappings.push_back(make_pair(numstates++,s));
		vkm1.push_back(0);
		return 0;
	}
}



/* trying to find the q value for the hit action*/

float qhit(int i, int a) //i is the state number a is the action number
{
	state cs = allstates[i];
	int qhit;
	if(a==0)
	{
    //for cards ranging from 2 to 9
	for(int cardval=2; cardval<=9; cardval++)
	{
	 state s;
	 s.sum = cs.sum+cardval;
	 s.phasace = cs.phasace;
	 s.dsum = cs.dsum;
	 s.dhasace = cs.dhasace;
	 s.numcards = cs.numcards + 1;
	 s.bj = false;
	 s.pair = ((cs.numcards == 1)&&(cs.sum==cardval))?true:false;
	 s.turn = (s.sum>21)?false:true;

     float prevv = checkninsert(s);
     qhit+= ((1-probability)/9)*((reward(s))+(prevv));
    }
   //for ace
    state s_ace;
	 s_ace.sum = (cs.sum+11>21)?(cs.sum+1):(cs.sum+11);
	 s_ace.phasace = true;
	 s_ace.dsum = cs.dsum;
	 s_ace.dhasace = cs.dhasace;
	 s_ace.numcards = cs.numcards + 1;
	 s_ace.bj = ((cs.numcards==1)&&(cs.sum==10))?true:false;
	 s_ace.pair = ((cs.numcards == 1)&&(cs.phasace))?true:false;
	 s_ace.turn = (s_ace.sum>21)?false:true;

     float prevv_ace = checkninsert(s_ace);
     qhit+= ((1-probability)/9)*((reward(s_ace))+(prevv_ace));

   //finished the ace case

    /* now the face cards*/
     state s_face;
     s_face.sum = cs.sum+10;
	 s_face.phasace = cs.phasace;
	 s_face.dsum = cs.dsum;
	 s_face.dhasace = cs.dhasace;
	 s_face.numcards = cs.numcards + 1;
	 s_face.bj = ((cs.numcards==1)&&(cs.phasace))?true:false;
	 s_face.pair = ((cs.numcards == 1)&&(cs.sum==10))?true:false;
	 s_face.turn = (s_face.sum>21)?false:true;

     float prevv_face = checkninsert(s_face);
     qhit+= (probability)*((reward(s_face))+(prevv_face));
    /* face case ended*/

    }

   else if (a==1) //stand 
   {
     state s;
     s.sum = cs.sum;
     s.phasace = cs.phasace;
     s.bj = cs.bj;
     s.pair = cs.pair;
     s.turn = false;
   }

}

/* */

void valueiteration()
{
	for(int i=0; i<330; i++)
		vkm1.push_back(0);

	//for k=1 to h do the following....
	for(int i=0; i<330; i++) //fo each state
	{
		for(int j=0; j<3; i++) ;//not taking split at this moment
	}
}


int main (int argc, char *argv[]){

/*creating the initial 330 states*/	
sspace();
/*end of creation of initial 330 states*/
actions[0] = hit;
actions[1] = stand;
actions[2] = dd;
actions[3] = dealer;
actions[4] = split;








return 0;


}