#include <iostream>
#include <functional>
#include <vector>
#include <cstdlib>
#include <time.h>

#include "dealer.h"

#define TIMER(SECS) \
    if ( m_timer_thread )\
    {\
       m_timer_thread->interrupt ();\
       delete ( m_timer_thread );\
       m_timer_thread = NULL;\
    }\
    m_timer_thread = new boost::thread ( delay_thread , SECS , std::bind ( &dealer::timer_expired , this ) );\

int eight_deck[13] = {32,32,32,32,32,32,32,32,32,32,32,32,32}; //global variable of eight shoe deck, 32 of each card available

unsigned int Hand_Value ( UberCasino::card_t cards[] ) //function for determining hand value given an array of cards
{
   // given an array of cards, returns the point value
   unsigned int total=0;
   for (unsigned int i=0; i< UberCasino::MAX_CARDS_PER_PLAYER;i++) //check all of the card slots
   {
      if ( cards[i].valid ) //only do this if the card is valid
      {
         switch ( cards[i].card ) //switch statement to calculate total based on card value
         {
            case ace: total=total+1;break;
            case two: total=total+2;break;
            case three: total=total+3;break;
            case four: total=total+4;break;
            case five: total=total+5;break;
            case six: total=total+6;break;
            case seven: total=total+7;break;
            case eight: total=total+8;break;
            case nine: total=total+9;break;
            case ten: total=total+10;break;
            case jack: total=total+10;break;
            case queen: total=total+10;break;
            case king: total=total+10;break;
         }
      }
   }
   return total; //return total value of all the cards in the player's hand
}

UberCasino::card_t Next_Card () //this function deals the next card, can be to player's hand or dealer's hand
{
   static UberCasino::suite_t lut[] = { hearts,diamonds,clubs,spades }; //enum for card suit
   // this function returns the next card to be dealt
   UberCasino::card_t retval; //variable for card to return
   int value = 0; //initialize variable for card value
   int suit = rand() % 4; //generate a random number from 0-3
   bool found = false; //initialize boolean for a card found variable

//Loop and switch statment for eight shoe deck
do //do  while loop that executes until a valid card value is found
{   
   value = rand() %13; cout << "Random = " << value << endl; //generate a random number from 0-12
   switch(value) //switch to set correct card value, if card is available, decrement its slot in the eight_deck array and found == true
		// if deck is out of that card, break, found is still false so generate a random number and try again
   {
	case 0: if(eight_deck[value] != 0){retval.card = king; eight_deck[value]+=-1; found = true;} break;
	case 1: if(eight_deck[value] != 0){retval.card = ace; eight_deck[value]+=-1; found = true;} break;
	case 2: if(eight_deck[value] != 0){retval.card = two; eight_deck[value]+=-1; found = true;} break;
	case 3: if(eight_deck[value] != 0){retval.card = three; eight_deck[value]+=-1; found = true;} break;
	case 4: if(eight_deck[value] != 0){retval.card = four; eight_deck[value]+=-1; found = true;} break;
	case 5: if(eight_deck[value] != 0){retval.card = five; eight_deck[value]+=-1; found = true;} break;
	case 6: if(eight_deck[value] != 0){retval.card = six; eight_deck[value]+=-1; found = true;} break;
	case 7: if(eight_deck[value] != 0){retval.card = seven; eight_deck[value]+=-1; found = true;} break;
	case 8: if(eight_deck[value] != 0){retval.card = eight; eight_deck[value]+=-1; found = true;} break;
	case 9: if(eight_deck[value] != 0){retval.card = nine; eight_deck[value]+=-1; found = true;} break;
	case 10: if(eight_deck[value] != 0){retval.card = ten; eight_deck[value]+=-1; found = true;} break;
	case 11: if(eight_deck[value] != 0){retval.card = jack; eight_deck[value]+=-1; found = true;} break;
	case 12: if(eight_deck[value] != 0){retval.card = queen; eight_deck[value]+=-1; found = true;} break;
   }
}while(!found);

//switch statement infinite shoe
/* value = rand()%13; //generate a random number from 0-12
 switch(value) //switch statement that returns the card value from king to queen (0 to 12)
   {
	case 0: retval.card = king; break;
	case 1: retval.card = ace; break;
	case 2: retval.card = two; break;
	case 3: retval.card = three; break;
	case 4: retval.card = four; break;
	case 5: retval.card = five; break;
	case 6: retval.card = six; break;
	case 7: retval.card = seven; break;
	case 8: retval.card = eight; break;
	case 9: retval.card = nine; break;
	case 10: retval.card = ten; break;
	case 11: retval.card = jack; break;
	case 12: retval.card = queen; break;
   }*/
   retval.suite = lut[suit]; //set the suit of the card to a random value
   retval.valid = true; // set the card to be valid
   return retval; //return the card
}

void delay_thread ( int seconds, std::function <void(void)> callback)
{
  // this routine is created as a posix thread.
  boost::this_thread::sleep_for(boost::chrono::seconds(seconds));
  callback ();
}

void dealer::lock ()
{
  std::cout << "****************************************" << std::endl;
  // pthread mutex works well
  // suggest you put it in if needed
}

void dealer::unlock ()
{
  // see remarks under lock(), above
}

std::string dealer::to_string ( dealer_state_t d )//helper function to convert the enum of dealer state to the appropriate string
{
   std::string retval;
   switch ( d )
   {
    case Init:
         retval = "Init";
         break; 
    case Waiting:
         retval = "Waiting";
         break;
    case WaitingForOthers:
         retval = "WaitingForOthers";
         break;
    case StartHand:
         retval = "StartHand";
         break;
    case Deal:
         retval = "Deal";
         break;
    case EndHand:
         retval = "EndHand";
         break;
    case Done:
         retval = "Done";
   }
   return retval;
}

void dealer::manage_state () //function that will move dealer from state to state
{
   bool transition = false; //set transition variable to off
   dealer_state_t next_state = m_dealer_state; //determine what the next state is supposed to be, keep dealer in same state if nothing happens

   // determine if we have a state transition
   switch (m_dealer_state) //switch depending on what state the dealer is in
   {
      case Init:
         if ( m_user_event ) //receive the start command from the user
         {
            next_state = Waiting; //next state is waiting state
            transition = true;
         }
         break;
      case Waiting:
         if ( m_Player_recv ) //if you receive request from pplayer
         {
            next_state = WaitingForOthers; //next state wait for more players
            transition = true;
         }
         break;
      case WaitingForOthers: //while waiting for more players
         if ( m_Player_recv ) //if another player is received
         {
            next_state = WaitingForOthers; //loop back to this state
            transition = true;
         }
         if ( m_timer_event ) //if time runs out
         {
            next_state = StartHand; //go to the beginning of the game
            transition = true;
         }
         break;
      case StartHand:
         if ( m_timer_event ) //after time runs out
         {
            next_state = Deal; //go to state for dealing a hand
            transition = true;
         }
         break;
      case Deal:
         if ( m_timer_event ) //if player times out
         {
            next_state = EndHand; //end the hand
            transition = true;
         }
         if ( m_Player_recv ) //if you receive an action from the player
         {
            std::cout << "got a player event " << std::endl;
            next_state = Deal;
            transition = true;
         }
         break;
      case EndHand:
         if ( m_timer_event ) //timer runs out after the hand has ended
         {
            next_state = StartHand; //begin a new hand
            transition = true;
         }
         break;
      case Done: //if game is over
         {
           std::cout << "THE GAME IS OVER" << std::endl;
           exit(0);
         }
   }

   // if there is a transition, then we have to run the exit 
   // and entrance processing
   if (transition)
   {
#ifdef DEBUG_STATES
      std::cout << "State change from " << to_string ( m_dealer_state ) 
                << " to " << to_string ( next_state ) << std::endl;
#endif
      // on exit
      switch (m_dealer_state)
      {
       case Init:
          {
#ifdef DEBUG_STATES
             std::cout << "Init: Exit" << std::endl;
#endif
             // this tells everyone listening, "i am starting a game"
             d_io->publish ( m_D_pub );
          }
          break;
       case Waiting:
          {
#ifdef DEBUG_STATES
             std::cout << "Waiting: Exit" << std::endl;
#endif
             // start a 10 second timer
             TIMER(10);
          }
          break;
       case WaitingForOthers:
          {
#ifdef DEBUG_STATES
            std::cout << "WaitingForOthers: Exit" << std::endl;
#endif
            // send out the game as it is
            m_G_pub.gstate = waiting;
            g_io->publish ( m_G_pub );
          }
          break;
       case StartHand:
          {
#ifdef DEBUG_STATES
            std::cout << "StartHand: Exit" << std::endl;
#endif
          }
          break;
       case Deal:
          {
#ifdef DEBUG_STATES
            std::cout << "Deal: Exit" << std::endl;
#endif
          }
          break;
       case EndHand:
          {
#ifdef DEBUG_STATES
            std::cout << "EndHand: Exit" << std::endl;
#endif
          }
          break;
       case Done:
          {
#ifdef DEBUG_STATES
            std::cout << "Done: Exit" << std::endl;
#endif
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
          }
          break;
       case Waiting:
          {
#ifdef DEBUG_STATES
            std::cout << "Waiting: Entry" << std::endl;
#endif
          }
          break;
       case WaitingForOthers:
          {
#ifdef DEBUG_STATES
             std::cout << "WaitingForOther: Entry" << std::endl;
#endif
             // if there is room, need to accept the 
             // new player
             if (  ( m_Player_recv ) && 
                   ( m_P_sub.A == idle ) && 
                   ( m_number_of_players<UberCasino::MAX_PLAYERS_IN_A_GAME ) )
             {
                 m_G_pub.gstate = waiting_to_join;
                 // the UID's don't change, so they can be copied again
                 memcpy ( m_G_pub.game_uid,  
                          m_D_pub.game_uid, 
                          sizeof (m_G_pub.game_uid) );
                 memcpy ( m_G_pub.dealer_uid,  
                          m_D_pub.uid, 
                          sizeof (m_G_pub.dealer_uid) );
                 memcpy ( m_G_pub.p[m_number_of_players].uid, 
                          m_P_sub.uid, 
                          sizeof ( m_G_pub.p[m_number_of_players].uid ) );

		//for loops for setting up the table before the game begins
                 for (unsigned int i=0;i<UberCasino::MAX_CARDS_PER_PLAYER;i++) //make sure all of the cards for the players are invalid
                 {
                   m_G_pub.p[ m_number_of_players ].cards[ i ].valid  = false;
                 }
                 for (unsigned int i=0;i<UberCasino::MAX_CARDS_PER_PLAYER;i++) //make sure all of the cards for the dealer are invalid
                 {
                   m_G_pub.dealer_cards[i].valid = false;
                 }
                 m_number_of_players++; //increment number of players, NOTE: this should always be 1 higher than the index value of each player
                 std::cout << "There are " << m_number_of_players << " in the game." << std::endl;
                 g_io->publish ( m_G_pub ); //publish the game to the players
             }
          }
          break;
        case StartHand:
          {
#ifdef DEBUG_STATES
             std::cout << "StartHand: Entry" << std::endl;
#endif
             // start a 1 second timer
             TIMER(1);
             m_G_pub.gstate = playing;
             m_G_pub.dealer_cards[0] = Next_Card (); //Deal the dealer's hole card

		//for loop to deal the first two cards to each player
             for (unsigned int i=0;i<m_number_of_players;i++)
             {
               m_G_pub.p[ i ].cards[ 0 ] = Next_Card ();
               m_G_pub.p [ i ]. cards [1] = Next_Card ();
             }
             m_G_pub.active_player = 0; //set the first player as the active player
             g_io->publish ( m_G_pub ); //publish the game
          }
          break;
       case Deal:
          {
#ifdef DEBUG_STATES
             std::cout << "Deal: Entry" << std::endl;
#endif
	     if(m_P_sub.A == doubling) //if player return Doubling Down as his action
             {
                unsigned int i = 0; cout <<"THE PLAYER IS DOUBLING DOWN" <<endl;
                while (m_G_pub.p[ m_G_pub.active_player ].cards[i].valid) //iterate through the player's cards until you found a slot that is not valid
                {
                   i++;
                }
                m_G_pub.p[ m_G_pub.active_player ].cards[ i ] = Next_Card (); //deal a card to the available slot
                if ( (int) m_G_pub.active_player+1 < (int) m_number_of_players ) //if there is still a player left to act
                 {
                    std::cout << "Going to the next player" << std::endl;
                    m_G_pub.active_player++; //increase index for active player
                    g_io->publish ( m_G_pub ); //republish game
                 }
                 else //no more players need to take their turn
                 {
                     std::cout << "Next, the dealers cards." << std::endl;
                     TIMER(1); //1 sec timer which will transition dealer to the next state
                 }
             }
             else if ( m_P_sub.A == standing ) //if player is standing, see 2nd half "Doubling Down" above for explanations
             {
                 std::cout << "The player is standing with " 
                           << Hand_Value ( m_G_pub.p[ m_G_pub.active_player ].cards)
                           << std::endl;
                 // go to next player
                 if ( (int) m_G_pub.active_player+1 < (int) m_number_of_players )
                 {
                    std::cout << "Going to the next player" << std::endl;
                    m_G_pub.active_player++;
                    g_io->publish ( m_G_pub );
                 }
                 else
                 {
                     std::cout << "Next, the dealers cards." << std::endl;
                     TIMER(1);
                 }
             }

             else if ( m_P_sub.A == hitting ) //player indicates that they are hitting, see 1st half of "Doubling Down" for dealing another card 
             {
                unsigned int i=0;
                while (m_G_pub.p[ m_G_pub.active_player ].cards[i].valid)
                {
                   i++;
                }
                m_G_pub.p[ m_G_pub.active_player ].cards[ i ] = Next_Card ();
                g_io->publish ( m_G_pub );
             }
          }
          break;
       case EndHand:
          {
#ifdef DEBUG_STATES
            std::cout << "EndHand: Entry" << std::endl;
#endif
            // deal the dealers card
            // note: except for purists, dealing a card face down or
            // waiting to deal it now makes no difference.
            unsigned int i=1;
            while ( Hand_Value ( m_G_pub.dealer_cards ) < 17 ) //dealer should always hit until dealer has at least 17
            {
               m_G_pub.dealer_cards[i] = Next_Card ();
               i++;
            }
            m_G_pub.gstate = end_hand; //set the game state to end of hand
            g_io->publish ( m_G_pub ); //publish so that the players know it is the end of the hand

//**************** LOOP to clear last set of cards*************************
		 for (unsigned int i=0;i<UberCasino::MAX_CARDS_PER_PLAYER;i++) //iterate through all of the cards in one plyaers hands
                 { //this loop may need to be reverse, right now it is invalidating all of the 1st cards, then all of the 2nd cards etc, in reality should probably loop through per valid player
			for(int j = 0; j < UberCasino::MAX_PLAYERS_IN_A_GAME;j++) //iterate through all of the players in the game
			{
				m_G_pub.p[ j ].cards[ i ].valid  = false; //set the cards to false
			}
                 }
                 for (unsigned int i=0;i<UberCasino::MAX_CARDS_PER_PLAYER;i++)
                 {
                   m_G_pub.dealer_cards[i].valid = false; // set all of dealer cards to false
                 }
            std::cout << "The players now decide if they win or lose" << std::endl;
            TIMER(5); //give players 5 seconds before beginning next hand
          }
          break;
       case Done:
          {
#ifdef DEBUG_STATES
            std::cout << "Done: Entry" << std::endl;
#endif
          }
          break;
      }
      // make the transition
      m_dealer_state = next_state;
      sleep(1);
   }
   // clear all event flags
   m_timer_event = false;
   m_user_event = false;
   m_Player_recv = false;
   m_Game_recv = false;
   m_Dealer_recv = false;
}

void dealer::new_game ()
{
}

void dealer::next_player ()
{

}

void dealer::end_game ()
{
}

void dealer::deal_to_dealer ()
{
}


void dealer::timer_expired ()
{
std::cout << "TIMER EXPIRED" << std::endl;
   // this is called by the timer thread callback when the delay has expired
   // note: only one timer can be active at a time
   lock ();
   m_timer_event = true;
   manage_state ();
   unlock ();
}

void dealer::external_data (Player P)
{
std::cout << "PLAYER" << std::endl;
   lock ();
   // this is called when data is received
   m_P_sub = P;
   m_Player_recv = true;
   manage_state ();
   unlock ();
}

void dealer::external_data (Dealer D)
{
std::cout << "DEALER" << std::endl;
   lock ();
   // this is called when data is received
   m_D_sub = D;
   m_Dealer_recv = true;
   manage_state ();
   unlock ();
}

void dealer::external_data (Game G)
{
std::cout << "GAME" << std::endl;
   lock ();
   // this is called when data is received
   m_G_sub = G;
   m_Game_recv = true;
   manage_state ();
   unlock ();
}

void dealer::user_input (std::string I)
{
   // this is called when the user types in input
   // from the console.  any / all input is accepted
   lock ();
   if (m_user_event_mask == I )
   {
      m_user_event = true;
      manage_state ();
   }
   unlock ();
}

dealer::dealer () //initialize the dealer class
{
   // member variables
   m_timer_thread = NULL;
   m_dealer_state = Init;
   m_user_event_mask = "start";  // this is the first event we will be looking for
   m_number_of_players = 0;
   m_G_pub.active_player = 0;
   // member objects
   p_io = new dds_io<Player,PlayerSeq,PlayerTypeSupport_var,PlayerTypeSupport,PlayerDataWriter_var,
                     PlayerDataWriter,PlayerDataReader_var,PlayerDataReader>
                ( (char*) "player", false, true );
                       // topic name, publish, subscribe

   d_io = new dds_io<Dealer,DealerSeq,DealerTypeSupport_var,DealerTypeSupport,DealerDataWriter_var,
                     DealerDataWriter,DealerDataReader_var,DealerDataReader>
                ( (char*) "dealer", true, false );

   g_io = new dds_io<Game,GameSeq,GameTypeSupport_var,GameTypeSupport,GameDataWriter_var,
                     GameDataWriter,GameDataReader_var,GameDataReader>
                ( (char*) "game", true, false );
   // event flags
   m_timer_event = false;
   m_user_event = false;
   m_Dealer_recv = false;
   m_Game_recv = false;
   m_Player_recv = false;

   m_hands_dealt = 0;
}

dealer::~dealer ()
{
}
