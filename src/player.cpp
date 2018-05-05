#include <iostream>
#include <functional>
#include <cstdlib>


#include <boost/thread.hpp>
#define TIMER(SECS) \
    if ( m_timer_thread )\
    {\
       m_timer_thread->interrupt ();\
       delete ( m_timer_thread );\
       m_timer_thread = NULL;\
    }\
    m_timer_thread = new boost::thread ( delay_thread , SECS , std::bind ( &player::timer_expired , this ) );\

#include "player.h"

//global variables
//player mode, 1 = manual mode, 2 = conservative mode, 3 = Reckless mode, 4 = Basic Strategy Mode
//decision is a variable holding the player's action
int decision=0;
//ante is the bet amount of the player
int ante = 10;

//tables for basic strategy, must value of card %10 to get the proper decision
string const ace_table[10][10] ={
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //A + 10
	{"Hit","Hit","Hit","Hit","Stand","Stand","Stand","Hit","Hit","Hit"},//A+A
	{"Hit","Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+2
	{"Hit","Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+3
	{"Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+4
	{"Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+5
	{"Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+6
	{"Hit","Hit","Stand","DoubleDown","DoubleDown","DoubleDown","DoubleDown","Stand","Stand","Hit"},//A+7
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"},//A+8
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}//A+9
	};
string const total_table[20][10] = {
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //20
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //1
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //2
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //3
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //4
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //5
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //6
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //7
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //8
	{"Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"}, //9
	{"Hit","Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown"}, //10
	{"Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown"}, //10
	{"Hit","Hit","Hit","Hit","Stand","Stand","Stand","Hit","Hit","Hit"}, //12
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //13
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //14
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //15
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //16
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //17
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //18
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"} //19
	};

//FLTK FUNCTIONS

//Callback Function for Exit Button
void exitCB(Fl_Widget* w, void* p)
{
	/*int selection = 1;
	//Display popup choice box
	selection = Fl_Choice("Are you sure you want to exit the game?", "Yes", "No");

	//Yes
	if(selection == 1) */
	   exit(0);

	// Else do nothing (No);
}


//Drop Down Selection for Player Mode
void choiceCB(Fl_Widget* w, void* p)
{
    Fl_Choice *choice = (Fl_Choice*) w;
  
    int type = choice->value();
    
    //Game type selection
     if(type == 0)	//manual mode
	{
	  player_mode = 1;
	}
     else if (type == 1) //Conservative mode
	{
	  player_mode = 2;
	}

     else if( type == 2)  //Reckless mode
	{
	    player_mode = 3;
	}
	
     else if(type == 3)	 //Basic Strategy mode
       {
	 player_mode = 4;
       }
}

//static function to pass class access to start button callback
void player::startCB(Fl_Widget* w, void *p)
{
	player* o = (player*)p;
	o->startCB2(w);
}

//actual start button callback
void player::startCB2(Fl_Widget* w)
{
	Fl_Input* i = (Fl_Input*)this->start_window->child(1);
	string copy = i->value();
	
//processing to turn string into char*
//display player's name in next window
	string player_name = this->m_P.name;
	char copy2[player_name.length()+1];
	assert(copy2);
	strncpy(copy2,player_name.c_str(),player_name.length());
	copy2[player_name.length()] = '\0';
	this->play_window->child(21)->copy_label(copy2);

//change dealer name, if dealer is available
	if(m_dealer_list.size() > 0)
	{
		int num = stoi(copy);
		string deal_name = this->m_dealer_list[num].name;
		char copy1[deal_name.length()+1];
		assert(copy1);
		strncpy(copy1,deal_name.c_str(),deal_name.length());
		copy1[deal_name.length()]='\0';
		this->play_window->child(22)->copy_label(copy1);
	}
//show starting bet of 10 dollars
	string bet = std::to_string(ante);
	const char* c = bet.c_str();
	Fl_Output* o = (Fl_Output*)this->play_window->child(29);
	o->value(c);

//show starting balance
	string bal = std::to_string(this->m_balance);
	const char* d = bal.c_str();
	Fl_Output* p = (Fl_Output*)this->play_window->child(23);
	p->value(d);

	this->user_input(copy);
	this->start_window->hide();
	this->play_window->show();
}
//callback for hit button
void hitCB(Fl_Widget* w, void* p)
{
	decision = 1;
	
}

//callback for stand button
void standCB(Fl_Widget* w, void* p)
{
	decision = 2;
}

//callback for double down button
void doubleCB(Fl_Widget* w, void* p)
{
	decision = 3;
}

//callback for help button pushed in first screen
void helpCB(Fl_Widget* w, void* p)
{
	fl_message("Welcome to UberCasino!\n\nA list of available dealers and their numbers will appear.\nPlease enter the dealer number of the game you would like to enter.\nThen select a play mode from the drop down (Manual or one of our automatic modes).\nPress the START button when you have chosen a play mode and a valid dealer.");
}

//callback for help button pushed to explain rules of Blackjack
void howtoCB(Fl_Widget* w, void* p)
{
	fl_message("Rules of Blackjack:\nObtain a higher total than the dealer without exceeding 21.\nFace cards count as 10. Aces count as either 1 or 11.\nPress HIT to obtain another card.\nPress STAND to end your turn.\nPress DOUBLE to double your ante, obtain 1 more card, and end your turn.\n\nDealer must hit on any total below 17.\nHaving a Blackjack(A + value of 10) pays 1.5 times the ante.\nIf your total is higher than 21, you BUST and lose.\nIf your total is the same as the dealer, you PUSH and receive your ante back.\n\nENJOY YOUR GAME!");
}

//static callback to allow passing of player object
void player::plusCB(Fl_Widget* w, void* p)
{
	player* o = (player*)p;
	o->plusCB2(w);
}

//actual callback of pushing + button next to the ante
void player::plusCB2(Fl_Widget* w)
{
	if(ante < (this->m_balance - 5))
	{
		ante+=5;
	}
	string bet = std::to_string(ante);
	const char* c = bet.c_str();
	Fl_Output* o = (Fl_Output*)this->play_window->child(29);
	o->value(c);
}

//callback to pass player object
void player::minusCB(Fl_Widget* w, void* p)
{
	player* o = (player*)p;
	o->minusCB2(w);
}

//actual callback subtracting from the ante
void player::minusCB2(Fl_Widget* w)
{
	if(this->m_balance > 5)
	{
		ante-=5;
	}
	string bet = std::to_string(ante);
	const char* c =bet.c_str();
	Fl_Output* o = (Fl_Output*)this->play_window->child(29);
	o->value(c);
}

//callback to button that will create pop up of basic strategy table
void player::strategyCB(Fl_Widget* w, void* p)
{
	Fl_Window* image_window = new Fl_Window(825,825,"Basic Strategy");
	image_window->begin();
	Fl_JPEG_Image* myimage;
	Fl_Box* image_box = new Fl_Box(0,0,800,800);
	myimage = new Fl_JPEG_Image("strategy.jpg");
	image_window->end();
	image_box->image(myimage);
	image_window->show();
}

//function to update the player's balance
void player::update_bal()
{
	string bal = std::to_string(this->m_balance);
	const char* d = bal.c_str();
	Fl_Output* p = (Fl_Output*)this->play_window->child(23);
	p->value(d);
}

//function to update the player's hand total
void player::update_total(int i)
{
	string bal = std::to_string(i);
	const char* d = bal.c_str();
	Fl_Output* p = (Fl_Output*)this->play_window->child(30);
	p->value(d);
	this->plusbutton->deactivate();
	this->minusbutton->deactivate();
}

//function for updating card values in the GUI
void player::update_cards(UberCasino::card_t cards[], void* o)
{
	player* p = (player*)o;

	for(unsigned int i = 0; i < UberCasino::MAX_CARDS_PER_PLAYER;i++)
	{
		string display = "";
		if(cards[i].valid)
		{
			p->play_window->child(i)->show();
			switch(cards[i].card)
			{
				case ace: display+="A"; break;
				case two: display+="2"; break;
				case three: display+="3"; break;
				case four: display+="4"; break;
				case five: display+="5"; break;
				case six: display+="6"; break;
				case seven: display+="7"; break;
				case eight: display+="8"; break;
				case nine: display+="9"; break;
				case ten: display+="10"; break;
				case jack: display+="J"; break;
				case queen: display+="Q"; break;
				case king: display+="K"; break;
			}
			switch(cards[i].suite)
			{
				case hearts: display+="/H"; break;
				case diamonds: display+="/D"; break;
				case clubs: display+="/C"; break;
				case spades: display+="/S"; break;
			}
			char copy[display.length()+1]; //creating a char* out of a string
			assert(copy);
			strncpy(copy,display.c_str(),display.length());
			copy[display.length()]='\0';	
			p->play_window->child(i)->copy_label(copy);
		}
		else
		{
			p->play_window->child(i)->hide();
		}
	}

	p->play_window->redraw();
}

//function for updating card values in the GUI
void player::update_dealer(UberCasino::card_t cards[], void* o)
{
	player* p = (player*)o;

	for(unsigned int i = 0; i < UberCasino::MAX_CARDS_PER_PLAYER;i++)
	{
		string display = "";
		if(cards[i].valid)
		{
			p->play_window->child(i+10)->show();
			switch(cards[i].card)
			{
				case ace: display+="A"; break;
				case two: display+="2"; break;
				case three: display+="3"; break;
				case four: display+="4"; break;
				case five: display+="5"; break;
				case six: display+="6"; break;
				case seven: display+="7"; break;
				case eight: display+="8"; break;
				case nine: display+="9"; break;
				case ten: display+="10"; break;
				case jack: display+="J"; break;
				case queen: display+="Q"; break;
				case king: display+="K"; break;
			}
			switch(cards[i].suite)
			{
				case hearts: display+="/H"; break;
				case diamonds: display+="/D"; break;
				case clubs: display+="/C"; break;
				case spades: display+="/S"; break;
			}
			char copy[display.length()+1];
			assert(copy);
			strncpy(copy,display.c_str(),display.length());
			copy[display.length()]='\0';	
			p->play_window->child(i+10)->copy_label(copy);
		}
		else
		{
			p->play_window->child(i+10)->hide();
		}
	}
}

//use this function to change the status message on the play window
void player::change_status(string s)
{
	char copy[s.length()+1];
	assert(copy);
	strncpy(copy,s.c_str(),s.length());
	copy[s.length()] = '\0';
	Fl_Output* o = (Fl_Output*)this->play_window->child(20);
	o->value(copy);
}

//function for counting hand total given an array of cards
unsigned int Hand_Value ( UberCasino::card_t cards[] )
{
   unsigned int total=0;
   for (unsigned int i=0; i< UberCasino::MAX_CARDS_PER_PLAYER;i++) //iterate through all of the possible cards
   {
      if ( cards[i].valid ) //check only valid cards
      {
         switch ( cards[i].card ) //switch case to increase total based on face value of card
         {
            case ace: if(total >=11){total=total+1;cout<<"ace(1)\n";}else{total=total+11;cout<<"ace(11)\n";}break; //ace is 1 only when hand is > 11, otherwise ace = 11
            case two: total=total+2;cout<<"two\n";break;
            case three: total=total+3;cout<<"three\n";break;
            case four: total=total+4;cout<<"four\n";break;
            case five: total=total+5;cout<<"five\n";break;
            case six: total=total+6;cout<<"six\n";break;
            case seven: total=total+7;cout<<"seven\n";break;
            case eight: total=total+8;cout<<"eight\n";break;
            case nine: total=total+9;cout<<"nine\n";break;
            case ten: total=total+10;cout<<"ten\n";break;
            case jack: total=total+10;cout<<"jack\n";break;
            case queen: total=total+10;cout<<"queen\n";break;
            case king: total=total+10;cout<<"king\n";break;
         }
      }
   }
   return total; //return the calculated hand total
}

void delay_thread ( int seconds, std::function <void(void)> callback)
{
  // this routine is created as a posix thread.
  boost::this_thread::sleep_for(boost::chrono::seconds(seconds));
  callback ();
}

std::string player::to_string ( player_state_t p ) //helper function to convert player state to string
{
   // turn the player_state enumerant into a string
   std::string retval;
   switch ( p )
   {
      case Init:
         retval = "Init";
         break;
      case StartHand:
         retval = "StartHand";
         break;
      case Playing:
         retval = "Playing";
         break;
      case EndHand:
         retval = "EndHand";
         break;
   }
   return retval;
}

void player::manage_state () //function for transitioning player between states
{
   // determine if we have a state transition
   bool transition = false;
   player_state_t next_state = m_player_state; //default next state is the current state
   switch ( m_player_state )
   {
     case Init:
         {
            // a dealer is starting a game
            if ( m_Dealer_recv )
            {
                transition = true;
            }
            // the player has entered a string
            if ( m_user_event )
            {
               // you must enter in something that does not
               // exception.  like a number
               m_dealer_idx = std::stoi ( m_user_event_string ); //convert the player entry into an int which is the index of the dealer
               if ( m_dealer_idx < m_dealer_list.size () ) //only do this if player has chosen from his list of dealers
               {
                  transition = true;
                  next_state = StartHand; //move state to start hand state
               }
            }
         }
         break;
     case StartHand:
         {
 		if(m_user_event){decision = std::stoi(m_user_event_string); cout << "I MADE A CHOICE";}//store player entry as his action decision in this state
             /*if ( m_timer_event ) //if player times out, return to initial state
             {
                 transition = true;
                 next_state = Init;
             }*/
             if ( m_Game_recv_idx ) // if he is in a game
             {
                 transition = true;
                 next_state = Playing;
             }
         }
         break;
     case Playing:
         {
             if ( m_Game_recv  && m_G.gstate == end_hand ) //if you are in a game, and the game state is in end hand
             {
             std::cout << "The dealer is showing his cards" << std::endl;
                 // the dealers cards are dealt
                 transition = true;
                 next_state = EndHand; //move to endhand state
             }
             if ( m_Game_recv_idx ) //else stay in this state
             {
                 transition = true;
                 next_state = Playing;
             }
         }
         break;
     case EndHand:
         {
             if ( m_timer_event ) //if timer expired, transition back to start hand
             {
                 transition = true;
                 next_state = StartHand;
             }
         }
         break;
   }

   if ( m_player_state != next_state ) //if the current state is not the same as the next state
   {
#ifdef DEBUG_STATES
      std::cout << "State change from " << to_string (m_player_state)
                << " to " << to_string ( next_state ) << std::endl;
#endif
   }
   // if there is a transition, then we have to run the exit 
   // and entrance processing
   if (transition)
   {
      // on exit
      switch (m_player_state)
      {
         case Init:
         {
#ifdef DEBUG_STATES
           std::cout << "Init: Exit" << std::endl;
#endif
           // Wait 30 seconds for the dealer to act
           // if he does not act, then he has not accepted us into the game
           TIMER(30);
         }
         break;
         case StartHand:
         {
#ifdef DEBUG_STATES
           std::cout << "StartHand Exit" << std::endl;
#endif
         }
         break;
         case Playing:
         {
#ifdef DEBUG_STATES
           std::cout << "Playing: Exit" << std::endl;
#endif
         }
         break;
         case EndHand:
         {
#ifdef DEBUG_STATES
           std::cout << "EndHand: Exit" << std::endl;
#endif
           std::cout << "set up all the variables to play another" << std::endl;
	   change_status("Waiting for next hand...");
         }
         break;
      }

      // on entrance
      switch (next_state)
      {
         case Init:
         {
#ifdef DEBUG_STATES
            std::cout << "Init: Entry" << std::endl;
#endif
            if (m_Dealer_recv)
            {
               m_dealer_list.push_back ( m_D ); //add the found dealer to list of dealers
            }
            // print the list to stdout, should change to a FLTK window
            if (m_dealer_list.size () > 0 )
            {
              string list = "";
              for (unsigned int i=0;i<m_dealer_list.size ();i++)
              {
                 std::cout << "Dealer # " << i 
                           << " name " << m_dealer_list[i].name << std::endl;

		 list +=std::to_string(i);
		 list +=") ";
		 list +=m_dealer_list[i].name;
		 list +="\n";
              }
              std::cout << "Enter the Dealer # to join.." << std::endl;

	      char copy[list.length()+1];
	      assert(copy);
	      strncpy(copy,list.c_str(),list.length());
	      copy[list.length()] = '\0';
	      Fl_Multiline_Output* o = (Fl_Multiline_Output*)this->start_window->child(0);
	      o->value(copy);
            }
         }
         break;
         case StartHand:
         {
#ifdef DEBUG_STATES
               std::cout << "Waiting: StartHand" << std::endl;
#endif
               memcpy ( m_P.game_uid, 
                        m_dealer_list[m_dealer_idx].game_uid,
                        sizeof ( m_P.game_uid ) );
               m_P.A = idle;
               // put the game_uuid in a member var a little 
               // easier to find
               memcpy ( &m_current_game_uuid,
                        m_dealer_list[m_dealer_idx].game_uid,
                        sizeof ( m_P.game_uid ) );
               p_io->publish  ( m_P );
         }
         break;
         case Playing:
         {
#ifdef DEBUG_STATES
            std::cout << "Playing: Entry " << std::endl;
#endif
            unsigned int value = Hand_Value ( m_G.p[m_G.active_player].cards ); //calculate the value of your hand
	    update_cards(m_G.p[m_G.active_player].cards,this);
	    update_dealer(m_G.dealer_cards,this);
	    update_total(value);
            std::cout << "The value of my hand is "<< value << std::endl;
	    assert(ante > 0);
	    assert(ante <= m_balance);

//if hand value is 21 or above
	if(value >= 21)
	{
		m_P.A = standing;
		decision = 0;
		change_status("Standing...");
		p_io->publish(m_P);
	}
//manual mode 
        else if(player_mode == 1)
	{
		cout << "1: Hit\n2:Stand\n3.Double Down\n";
		change_status("Please choose an action.");
		while(decision==0); //wait here while decision is still 0
		if(decision == 1)
		{
			std::cout << "I have decided to hit " << std::endl;
			change_status("I am hitting.");
              		m_P.A = hitting;
			decision = 0;
		} 
		else if (decision==2)
		{
			std::cout << "I have decided to stand" << std::endl;
			change_status("I have decided to stand.");
               		m_P.A = standing;
			decision = 0;
		}
		else if (decision == 3)
		{
			cout << "I am doubling down" << endl;
			change_status("Double Down!");
			m_P.A = doubling;
			ante*=2;
			decision = 0;
		}
		assert(player_mode == 1);
		p_io->publish(m_P);
	}
//conservative mode(automatic)
        else if(player_mode == 2)
	{
		cout << "Running in Conservative Mode\n";
	   	if (( value > 11)) //dont hit above 11 because you may lose
           	{
           		std::cout << "I have decided to stand " << std::endl;
			change_status("I might bust? Standing.");
           	    	m_P.A = standing;
            	}
            	else if ((value < 11))//only hit when you're below 11
            	{
               		std::cout << "I have decided to hit " << std::endl;
			change_status("I can't bust. Hit!");
               		m_P.A = hitting;
            	}
		assert(player_mode==2);
            	p_io->publish  ( m_P ); //publish to dealer
         }
//reckless mode (automatic)
	else if(player_mode == 3)
	{
		cout << "Running in Reckless Mode\n";
		if((value  == 10 || value == 11))
		{
			std::cout << "Double it!" <<std::endl;
			change_status("I can win twice! Double Down!");
			ante*=2;
			m_P.A = doubling;
	 	}
		if(value < 21)
	 	{
			std::cout << "I want to hit" <<std::endl;
			change_status("I'm not to 21 yet. Hit!");
	 	        m_P.A = hitting;
	 	}
	 	else
	   	{
			std::cout << "I want to stand" <<std::endl;
			change_status("Looks like a winner! Stand.");
			m_P.A = standing;
	   	}
		assert(player_mode==3);
		p_io->publish (m_P); //publish to dealer
	}
//basic strategy mode (automatic)
	else if(player_mode == 4)
	{
		cout << "Running in Basic Strategy mode\n";
		int hand_size = 0;
		string choice;
		for (unsigned int i=0; i< UberCasino::MAX_CARDS_PER_PLAYER;i++) //iterate through all of the possible cards
   		{
      	 		if ( m_G.p[m_G.active_player].cards[i].valid) //check only valid cards
      			{
				hand_size+=1;
			}
		}
		
		if(value == 21)
		{
			cout << "I want to stand\n";
			change_status("Can't get a better hand than this. Stand");
			m_P.A = standing;
		}
		else if(value > 21)
		{
			cout << "I have busted\n";
			change_status("Unlucky draw.");
			m_P.A = standing;
		}
		if(hand_size == 2)
		{
			if(m_G.p[m_G.active_player].cards[0].card == ace) //if either of the first two cards are an ace
			{
				choice = ace_table[m_G.p[m_G.active_player].cards[1].card%10][m_G.dealer_cards[0].card%10];
			}
			else if(m_G.p[m_G.active_player].cards[1].card == ace) //if either of the first two cards are an ace
			{
				choice = ace_table[m_G.p[m_G.active_player].cards[0].card%10][m_G.dealer_cards[0].card%10];
			}
		}
		else
		{
			choice = total_table[value%20][m_G.dealer_cards[0].card%10];
		}

		if(choice == "DoubleDown")
		{
			cout << "Doubling Down!\n";
			change_status("I have a good chance. Double Down!");
			ante*=2;
			m_P.A = doubling;
		}
		else if(choice == "Hit")
		{
			cout << "I want to hit.\n";
			change_status("Gotta do better than this. Hit!");
			m_P.A = hitting;
		}
		else
		{
			cout << "I want to stand\n";
			change_status("This is probably good enough. Stand.");
			m_P.A = standing;
		}
		assert(player_mode==4);
		p_io->publish (m_P); //publish to dealer
	 }
         }
         break;
         case EndHand:
         {
#ifdef DEBUG_STATES
            std::cout << "EndHand: Entry " << std::endl;
#endif
std::cout << "the state is " << (int)  m_G.gstate  << std::endl;
            if  ( m_G.gstate == end_hand ) 
            {
	      assert(m_balance >=10);
              std::cout << "The dealer says end of hand." << std::endl; 
              // calculate win or lose
	      cout<< "Dealer's Hand:\n";
	      update_dealer(m_G.dealer_cards,this);
              int dealer_points = Hand_Value ( m_G.dealer_cards );
              // calculate the hand value
              cout<< "Player's cards: \n";
              int player_points = Hand_Value ( m_G.p[m_G.active_player].cards );
              std::cout << "Dealer has " << dealer_points << " Player has " << player_points << std::endl;
              if(player_points > 21)//if player busts
	      {
		 cout << "Dealer Wins" << endl;
		 change_status("I busted. Dealer wins.");
		 m_balance = m_balance - ante;
		 update_bal();
	      }
              else if ( dealer_points > 21 || ( (player_points > dealer_points) && (player_points < 21) ) ) //if dealer has busted but player has not, player wins
              {
                 std::cout << "Player Wins" << std::endl;
		 change_status("Dealer busted! I win.");
                 m_balance = m_balance + ante;
		 update_bal();
              }
              else if ( player_points ==  dealer_points ) //if they have the same value, it is a push
              {
                 std::cout << "Push" << std::endl;
		 change_status("We have the same total. Push.");
              }
              else //else if dealer has a higher value than player but neither have busted
              {
                 std::cout << "Dealer Wins" << std::endl;
		 change_status("Dealer is closer. Dealer wins.");
                 m_balance = m_balance - ante;
	 	 update_bal();
              }

              if (m_balance > 10.0 )              
	      {
                 // this is just a way to start the next hand
                 TIMER(3);
              }
              else
              {
                 std::cout << "Down to " << m_balance << " exiting from game " << std::endl; //player has run out of money
                 exit(0);
              }
            }
         }
         break;
      }

      // make the transition
      m_player_state = next_state;
   }

   // clear all event flags
   m_timer_event   = false;
   m_user_event    = false;
   m_Player_recv   = false;
   m_Game_recv_idx = false;
   m_Game_recv     = false;
   m_Dealer_recv   = false;

}


void player::timer_expired ( )
{
   // this is called by the timer thread callback when the delay has expired
   // note: only one timer can be active at a time
   m_timer_event = true;
   std::cout << "Timer event has been received " << std::endl;
   manage_state ();
}

void player::external_data (Player P)
{
   // this is called when data is received
   m_P = P;
   m_Player_recv = true;
   std::cout << "Player data has been received" << std::endl;
   manage_state ();
}

void player::external_data (Dealer D)
{
   // this is called when data is received
   m_D = D;
   m_Dealer_recv = true;
   std::cout << "Dealer data has been received" << std::endl;
   manage_state ();
}

void player::external_data (Game G)
{
   // only care about games
   // we are in.
   //
   // this routine creates two possible eventsd
   // they are mutually exclusive
   //     m_Game_recv      when the game uids match
   //     m_Game_recv_idx  when the game uids and the idx match

   // first, the game needs to match   
   boost::uuids::uuid t;
   memcpy ( &t, G.game_uid, sizeof ( t ) );
   if (t == m_current_game_uuid)
   {
        m_G = G;  // it is the state of our game, so 
                  // store it
        m_Game_recv = true;
        // now to check if it is our turn
        unsigned int i = G.active_player;
        boost::uuids::uuid active_player_uuid;
        memcpy ( &active_player_uuid,
                 G.p[i].uid,
                 sizeof ( active_player_uuid ) );
        if ( m_my_uid == active_player_uuid )
        {
            // and last, need to be sure we are
            // 'playing' the game
            if ( G.gstate == playing )
            {
               m_Game_recv_idx = true;
               m_Game_recv = false;
            }
        }
   }
   if (m_Game_recv || m_Game_recv_idx )
   {
      if ( m_Game_recv )
      {
        std::cout << "Game (uid matched) received" << std::endl;
      }
      else if ( m_Game_recv_idx )
      {
        std::cout << "Game (uid matched and idx ) received" << std::endl;
      }
      manage_state ();
   }
}

void player::user_input (std::string I)
{
   // this is called when the user types in input
   // from the console.  any / all input is accepted
   m_user_event_string = I;
   m_user_event = true;
   std::cout << "User input received" << std::endl;
   manage_state ();
}

void player::setName (std::string Name )
{
   strncpy ( m_P.name, Name.c_str(), sizeof (m_P.name) - 1 );
}

player::player ()
{
   // member variables
   m_player_state = Init;
   m_balance = 200.0;
   m_P.balance = m_balance;
   m_dealer_list.clear ();
   m_timer_thread = NULL;

   //FLTK variables
   start_window = new Fl_Double_Window(700,500);
   start_window->box(FL_OFLAT_BOX);
   start_window->color((Fl_Color)100);
   start_window->labelfont(10);
   start_window->labelcolor((Fl_Color)33);

 //FLTK start window initialization
   start_window->begin();

   Fl_Multiline_Output* dealer = new Fl_Multiline_Output(305,325,100,100,"List of Dealers"); //child 0
   dealer->value("");

   Fl_Input* input = new Fl_Input(345,225,30,30,"Please enter a Dealer Number"); //child 1
   input->value("");
   input->align(FL_ALIGN_TOP);

   Fl_Button* start_button = new Fl_Button(325,265,70,20,"Start"); //child 2
   Fl_Text_Display* background = new Fl_Text_Display(335,135,50,30,"UberCasino"); //child 3
   Fl_Choice* choice = new Fl_Choice(275,180,225,15,"Select Game Type"); //child 4
   Fl_Button* ExitBut = new Fl_Button(5,400,70,25,"EXIT");//child 5
   Fl_Button* help1 = new Fl_Button(5,350,70,25,"HELP"); //child 6

   start_button->box(FL_PLASTIC_ROUND_DOWN_BOX);
   start_button->labelcolor((Fl_Color)1);
   start_button->callback(startCB,this);

   background->box(FL_NO_BOX);
   background->labeltype(FL_EMBOSSED_LABEL);
   background->labelfont(8);
   background->labelsize(100);
   background->labelcolor((Fl_Color)96);

   choice->down_box(FL_BORDER_BOX);
   choice->textfont(10);
   choice->textcolor((Fl_Color)128);
   choice->callback(choiceCB, 0);
   choice->add("Manual");
   choice->add("Conservative");
   choice->add("Reckless");
   choice->add("Basic Strategy");
   choice->value(0);
   

   ExitBut->callback(exitCB);
   help1->callback(helpCB);

   start_window->end();

  //Intialization of the play_window
   play_window = new Fl_Double_Window(1600, 900);
   play_window->box(FL_OSHADOW_BOX);
   play_window->color(FL_DARK_GREEN);
   play_window->align(Fl_Align(133));

   play_window->begin();

//PLAYER CARDS
   Fl_Box* card1 = new Fl_Box(600,100,50,75,"Card1"); //child 0
   card1->box(FL_PLASTIC_DOWN_BOX);
   card1->color(FL_WHITE);
   Fl_Box* card2 = new Fl_Box(675,100,50,75,"Card2"); //child 1
   card2->box(FL_PLASTIC_DOWN_BOX);
   card2->color(FL_WHITE);
   Fl_Box* card3 = new Fl_Box(750,100,50,75,"Card3"); //child 2
   card3->box(FL_PLASTIC_DOWN_BOX);
   card3->color(FL_WHITE);
   Fl_Box* card4 = new Fl_Box(825,100,50,75,"Card4"); //child 3
   card4->box(FL_PLASTIC_DOWN_BOX);
   card4->color(FL_WHITE);
   Fl_Box* card5 = new Fl_Box(900,100,50,75,"Card5"); //child 4
   card5->box(FL_PLASTIC_DOWN_BOX);
   card5->color(FL_WHITE);
   Fl_Box* card6 = new Fl_Box(600,200,50,75,"Card6"); //child 5
   card6->box(FL_PLASTIC_DOWN_BOX);
   card6->color(FL_WHITE);
   Fl_Box* card7 = new Fl_Box(675,200,50,75,"Card7"); //child 6
   card7->box(FL_PLASTIC_DOWN_BOX);
   card7->color(FL_WHITE);
   Fl_Box* card8 = new Fl_Box(750,200,50,75,"Card8"); //child 7
   card8->box(FL_PLASTIC_DOWN_BOX);
   card8->color(FL_WHITE);
   Fl_Box* card9 = new Fl_Box(825,200,50,75,"Card9"); //child 8
   card9->box(FL_PLASTIC_DOWN_BOX);
   card9->color(FL_WHITE);
   Fl_Box* card10 = new Fl_Box(900,200,50,75,"Card10"); //child 9
   card10->box(FL_PLASTIC_DOWN_BOX);
   card10->color(FL_WHITE);

//DEALER CARDS
   Fl_Box* card11 = new Fl_Box(600,500,50,75,"Card1"); //child 10
   card11->box(FL_PLASTIC_DOWN_BOX);
   card11->color(FL_WHITE);
   Fl_Box* card12 = new Fl_Box(675,500,50,75,"Card2"); //child 11
   card12->box(FL_PLASTIC_DOWN_BOX);
   card12->color(FL_WHITE);
   Fl_Box* card13 = new Fl_Box(750,500,50,75,"Card3"); //child 12
   card13->box(FL_PLASTIC_DOWN_BOX);
   card13->color(FL_WHITE);
   Fl_Box* card14 = new Fl_Box(825,500,50,75,"Card4"); //child 13
   card14->box(FL_PLASTIC_DOWN_BOX);
   card14->color(FL_WHITE);
   Fl_Box* card15 = new Fl_Box(900,500,50,75,"Card5"); //child 14
   card15->box(FL_PLASTIC_DOWN_BOX);
   card15->color(FL_WHITE);
   Fl_Box* card16 = new Fl_Box(600,600,50,75,"Card6"); //child 15
   card16->box(FL_PLASTIC_DOWN_BOX);
   card16->color(FL_WHITE);
   Fl_Box* card17 = new Fl_Box(675,600,50,75,"Card7"); //child 16
   card17->box(FL_PLASTIC_DOWN_BOX);
   card17->color(FL_WHITE);
   Fl_Box* card18 = new Fl_Box(750,600,50,75,"Card8"); //child 17
   card18->box(FL_PLASTIC_DOWN_BOX);
   card18->color(FL_WHITE);
   Fl_Box* card19 = new Fl_Box(825,600,50,75,"Card9"); //child 18
   card19->box(FL_PLASTIC_DOWN_BOX);
   card19->color(FL_WHITE);
   Fl_Box* card20 = new Fl_Box(900,600,50,75,"Card10"); //child 19
   card20->box(FL_PLASTIC_DOWN_BOX);
   card20->color(FL_WHITE);

//hide all of the cards at startup
   card1->hide();
   card2->hide();
   card3->hide();
   card4->hide();
   card5->hide();
   card6->hide();
   card7->hide();
   card8->hide();
   card9->hide();
   card10->hide();
   card11->hide();
   card12->hide();
   card13->hide();
   card14->hide();
   card15->hide();
   card16->hide();
   card17->hide();
   card18->hide();
   card19->hide();
   card20->hide();


//status message bar
   Fl_Output* status= new Fl_Output(600, 350, 400,50); //child 20
   status->box(FL_ENGRAVED_BOX);
   status->value("Waiting for Game to begin...");

//Dealer and Player name plates
   Fl_Box* player_name= new Fl_Box(725,25,100,25); //child 21
   player_name->box(FL_RSHADOW_BOX);
   player_name->label("Player");
   Fl_Box* dealer_name= new Fl_Box(725,700,100,25); //child 22
   dealer_name->box(FL_RSHADOW_BOX);
   dealer_name->label("Dealer");

//Player's Balance Widget
   Fl_Output* balance = new Fl_Output(1025,360,100,25,"Balance"); //child 23
   balance->align(FL_ALIGN_TOP);
   balance->box(FL_BORDER_BOX);

//Action buttons
   Fl_Button* hit_button = new Fl_Button(650,300,60,40,"Hit"); //child 24
   hit_button->box(FL_OSHADOW_BOX);
   hit_button->color((Fl_Color)160);
   hit_button->callback(hitCB);
   Fl_Button* stand_button = new Fl_Button(750,300,60,40,"Stand"); //child 25
   stand_button->box(FL_OSHADOW_BOX);
   stand_button->color((Fl_Color)160);
   stand_button->callback(standCB);
   Fl_Button*double_button = new Fl_Button(850,300,60,40,"Double"); //child 26
   double_button->box(FL_OSHADOW_BOX);
   double_button->color((Fl_Color)160);
   double_button->callback(doubleCB);

//Ante changing buttons and display
   plusbutton = new Fl_Button(1100,310,25,25,"+"); //child 27
   plusbutton->box(FL_THIN_UP_BOX);
   plusbutton->callback(plusCB,this);
   minusbutton = new Fl_Button(1025,310,25,25,"-"); //child 28
   minusbutton->box(FL_THIN_UP_BOX);
   minusbutton->callback(minusCB,this);
   Fl_Output* ante_display = new Fl_Output(1050,310,50,25,"Ante"); //child 29
   ante_display->align(FL_ALIGN_TOP);
   ante_display->box(FL_BORDER_BOX);

//Hand Total Widget
   Fl_Output* total = new Fl_Output(1050,260,50,25,"Hand Total"); //child 30
   total->box(FL_BORDER_BOX);
   total->align(FL_ALIGN_TOP);

//Exit and Help buttons
   Fl_Button* exitbut = new Fl_Button(0,0,75,25,"Exit");
   Fl_Button* howto = new Fl_Button(0,25,75,25,"HELP");
   exitbut->callback(exitCB);
   howto->callback(howtoCB);

//button to display basic strategy image
   Fl_Button* table = new Fl_Button(0,50,75,25,"Strategy");
   table->callback(strategyCB,this);

   play_window->end();
   play_window->resizable(play_window);

   
   // member objects
   p_io = new dds_io<Player,PlayerSeq,PlayerTypeSupport_var,PlayerTypeSupport,PlayerDataWriter_var,
                     PlayerDataWriter,PlayerDataReader_var,PlayerDataReader>
                ( (char*) "player", true, false );

   d_io = new dds_io<Dealer,DealerSeq,DealerTypeSupport_var,DealerTypeSupport,DealerDataWriter_var,
                     DealerDataWriter,DealerDataReader_var,DealerDataReader>
                ( (char*) "dealer", false, true );

   g_io = new dds_io<Game,GameSeq,GameTypeSupport_var,GameTypeSupport,GameDataWriter_var,
                     GameDataWriter,GameDataReader_var,GameDataReader>
                ( (char*) "game", false, true );

   // event flags
   m_timer_event    = false;
   m_user_event     = false;
   m_Player_recv    = false;
   m_Game_recv      = false;
   m_Game_recv_idx  = false;
   m_Dealer_recv    = false;
}

player::~player ()
{
}
