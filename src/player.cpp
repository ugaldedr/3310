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
//player mode, 1 = manual mode, 2 = conservative mode, 3 =Reckless mode
int player_mode = 2; 
//decision is a variable holding the player's action
int decision=0;


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

void player::lock ()
{
  // if this code is being used as part of an fltk program
  // a lock may be needed.  The fltk lock or something
  // like pthread_mutex() should work fine.
  std::cout << "****************************************" << std::endl;
}

void player::unlock ()
{
  // see comments under the lock () method
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
             if ( m_timer_event ) //if player times out, return to initial state
             {
                 transition = true;
                 next_state = Init;
             }
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
              for (unsigned int i=0;i<m_dealer_list.size ();i++)
              {
                 std::cout << "Dealer # " << i 
                           << " name " << m_dealer_list[i].name << std::endl;
              }
              std::cout << "Enter the Dealer # to join.." << std::endl;
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
            std::cout << "The value of my hand is "<< value << std::endl;
//manual mode 
       if(player_mode == 1)
	{
		cout << "1: Hit\n2:Stand\n";
		while(decision==0); //wait here while decision is still 0
		if(decision == 1){
			std::cout << "I have decided to hit " << std::endl;
              		 m_P.A = hitting;/*decision = 0;*/
					} 
		else if (decision==2){
			std::cout << "I have decided to stand" << std::endl;
               		m_P.A = standing;TIMER(1);
			}
			p_io->publish(m_P); //why is it publishing multiple times?
		       }
//conservative mode  
       else if(player_mode == 2){
	   cout << "1: Hit\n2:Stand\n";
	   while(decision==0); //wait here while decision is still 0
	   if (( value > 11) && decision == 2) //dont hit above 11 because you may lose
            {
               std::cout << "I have decided to stand " << std::endl;
               m_P.A = standing;
               TIMER(1);
            }
            else if ((value < 11) && decision == 1)//only hit when you're below 11
            {
               std::cout << "I have decided to hit " << std::endl;
               m_P.A = hitting;
            }
            p_io->publish  ( m_P ); //publish to dealer
            }
//reckless mode 
	else if(player_mode == 3)
	{
	  cout << "1: Hit\n2:Stand\n3:Double\n";
	  while(decision==0); //wait here while decision is still 0
	  if((value  == 10 || value == 11) && decision == 3)
	   {
		std::cout << "Double it!" <<std::endl;
		m_P.A = doubling;
	   }
	  if((value < 21) && decision == 1)
	   {
		std::cout << "I want to hit" <<std::endl;
	        m_P.A = hitting;
	   }
	   else if (decision == 2){
		std::cout << "I want to stand" <<std::endl;
		m_P.A = standing;
		TIMER(1);
	   }
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
              std::cout << "The dealer says end of hand." << std::endl; 
              decision =0; //change the decision to 0 so ensure that it will wait for the player decision when we go back to play
              // calculate win or lose
	      cout<< "Dealer's Hand:\n";
              int dealer_points = Hand_Value ( m_G.dealer_cards );
              // calculate the hand value
              cout<< "Player's cards: \n";
              int player_points = Hand_Value ( m_G.p[m_G.active_player].cards );
              std::cout << "Dealer has " << dealer_points << " Player has " << player_points << std::endl;
              if(player_points > 21)
	      {
		 cout << "Dealer Wins" << endl;
		 m_balance = m_balance - 10.0;
	      }
	      else if(player_points > 21) //if Player busts, Dealer wins no matter what
	      {
		 cout << "Dealer Wins" << endl;
		 m_balance = m_balance - 10.0;
              }
              else if ( dealer_points > 21 || ( (player_points > dealer_points) && (player_points < 21) ) ) //if dealer has busted but player has not, player wins
              {
                 std::cout << "Player Wins" << std::endl;
                 m_balance = m_balance + 10.0;
              }
              else if ( player_points ==  dealer_points ) //if they have the same value, it is a push
              {
                 std::cout << "Push" << std::endl;
              }
              else //else if dealer has a higher value than player but neither have busted
              {
                 std::cout << "Dealer Wins" << std::endl;
                 m_balance = m_balance - 10.0;
              }

              if (m_balance > 10.0 )              {
                 // this is just a way to start the next hand
                 TIMER(2);
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
   lock ();
   m_timer_event = true;
   std::cout << "Timer event has been received " << std::endl;
   manage_state ();
   unlock ();
}

void player::external_data (Player P)
{
   // this is called when data is received
   lock ();
   m_P = P;
   m_Player_recv = true;
   std::cout << "Player data has been received" << std::endl;
   manage_state ();
   unlock ();
}

void player::external_data (Dealer D)
{
   // this is called when data is received
   lock ();
   m_D = D;
   m_Dealer_recv = true;
   std::cout << "Dealer data has been received" << std::endl;
   manage_state ();
   unlock ();
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
   lock ();

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
   else
      // single player, this is a problem.  otherwise it is OK
      // std::cout << "Game was ignored " << std::endl;

   unlock ();
}

void player::user_input (std::string I)
{
   // this is called when the user types in input
   // from the console.  any / all input is accepted
   lock ();
   m_user_event_string = I;
   m_user_event = true;
   std::cout << "User input received" << std::endl;
   manage_state ();
   unlock ();
}

void player::setName (std::string Name )
{
   strncpy ( m_P.name, Name.c_str(), sizeof (m_P.name) - 1 );
}

player::player ()
{
   // member variables
   m_player_state = Init;
   m_balance = 1000.0;
   m_P.balance = m_balance;
   m_dealer_list.clear ();
   m_timer_thread = NULL;
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
