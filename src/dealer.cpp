#include <iostream>
#include <functional>
#include <vector>


#include <boost/thread.hpp>

#include "dealer.h"

#define TIMER(SECS) \
    if ( m_timer_thread )\
    {\
       m_timer_thread->interrupt ();\
       delete ( m_timer_thread );\
       m_timer_thread = NULL;\
    }\
    m_timer_thread = new boost::thread ( delay_thread , SECS , std::bind ( &dealer::timer_expired , this ) );\



unsigned int Hand_Value ( UberCasino::card_t cards[] )
{
   // given an array of cards, returns the point value
   unsigned int total=0;
   for (unsigned int i=0; i< UberCasino::MAX_CARDS_PER_PLAYER;i++)
   {
      if ( cards[i].valid )
      {
         switch ( cards[i].card )
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
   return total;
}

UberCasino::card_t Next_Card ()
{
   static int count=0;
   static UberCasino::suite_t lut[] = { hearts,diamonds,clubs,spades };
   // this function returns the next card to be dealt
   UberCasino::card_t retval;
   retval.card = ten;
   count++;
   if (count>3)
     count = 0;
   retval.suite = lut[count];
   retval.valid = true;
   return retval;
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

std::string dealer::to_string ( dealer_state_t d )
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

void dealer::manage_state ()
{
   bool transition = false;
   dealer_state_t next_state = m_dealer_state;

   // determine if we have a state transition
   switch (m_dealer_state)
   {
      case Init:
         if ( m_user_event ) 
         {
            next_state = Waiting;
            transition = true;
         }
         break;
      case Waiting:
         if ( m_Player_recv )
         {
            next_state = WaitingForOthers;
            transition = true;
         }
         break;
      case WaitingForOthers:
         if ( m_Player_recv )
         {
            next_state = WaitingForOthers;
            transition = true;
         }
         if ( m_timer_event )
         {
            next_state = StartHand;
            transition = true;
         }
         break;
      case StartHand:
         if ( m_timer_event )
         {
            next_state = Deal;
            transition = true;
         }
         break;
      case Deal:
         if ( m_timer_event )
         {
            next_state = EndHand;
            transition = true;
         }
         if ( m_Player_recv )
         {
std::cout << "got a player event " << std::endl;
            next_state = Deal;
            transition = true;
         }
         break;
      case EndHand:
         if ( m_timer_event )
         {
            next_state = Done;
            transition = true;
         }
         break;
      case Done:
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

                 for (unsigned int i=0;i<UberCasino::MAX_CARDS_PER_PLAYER;i++)
                 {
                   m_G_pub.p[ m_number_of_players ].cards[ i ].valid  = false;
                 }
                 for (unsigned int i=0;i<UberCasino::MAX_CARDS_PER_PLAYER;i++)
                 {
                   m_G_pub.dealer_cards[i].valid = false;
                 }
                 m_number_of_players++;
                 std::cout << "There are " << m_number_of_players << " in the game." << std::endl;
                 g_io->publish ( m_G_pub );
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
             //  one for the dealer
             m_G_pub.dealer_cards[0] = Next_Card ();
             //  one for each player
             for (unsigned int i=0;i<m_number_of_players;i++)
             {
               m_G_pub.p[ i ].cards[ 0 ] = Next_Card ();
             }
             // and the second card to the first player
             m_G_pub.p [ 0 ]. cards [1] = Next_Card ();
             // set to the player
             m_G_pub.active_player = 0;
             g_io->publish ( m_G_pub );
          }
          break;
       case Deal:
          {
#ifdef DEBUG_STATES
             std::cout << "Deal: Entry" << std::endl;
#endif
             if ( m_P_sub.A == standing )
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
             else if ( m_P_sub.A == hitting )
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
            while ( Hand_Value ( m_G_pub.dealer_cards ) < 17 )
            {
               m_G_pub.dealer_cards[i] = Next_Card ();
               i++;
            }
            m_G_pub.gstate = end_hand;
            g_io->publish ( m_G_pub );
            // if you wanted to, the dealer could decide who wins
            // or loses here.
            std::cout << "The players now decide if they win or lose" << std::endl;
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

dealer::dealer ()
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
