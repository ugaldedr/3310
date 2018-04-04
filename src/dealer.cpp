#include <iostream>
#include <functional>
#include <vector>


#include <boost/thread.hpp>

#include "dealer.h"

void delay_thread ( int seconds, std::function <void(void)> callback)
{
  // this routine is created as a posix thread.
  sleep (seconds);
  callback ();
}

void dealer::lock ()
{
   // pthread mutex works well
   // suggest you put it in if needed
}

void dealer::unlock ()
{
   // see remarks under lock(), above
}

std::string dealer::print_state ( dealer_state_t d )
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
    case Dealing:
         retval = "Dealing";
         break;
    case WaitingForPlayer:
         retval = "WaitingForPlayer";
         break;
    case Done:
         retval = "Done";
         break;
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
            transition = true;
            next_state = Waiting;
         }
         break;
      case Waiting:
         if ( m_Player_recv )
         {
            transition = true;
            next_state = WaitingForOthers;
         }
         break;
      case WaitingForOthers:
         if ( m_Player_recv )
         {
            transition = true;
            next_state = WaitingForOthers;
         }
         if ( m_timer_event )
         {
            next_state = Dealing;
            transition = true;
         }

         break;
      case Dealing:
         if ( m_Player_recv )
         {
            transition = true;
            next_state = Dealing;
         }
         // need to check for exit here
         if ( m_hands_dealt > 5 )
         {
            transition = true;
            next_state = Done;
         }
         break;
      case WaitingForPlayer:
         break;
      case Done:
         break;

   }

   // if there is a transition, then we have to run the exit 
   // and entrance processing
   if (transition)
   {
      // on exit
      switch (m_dealer_state)
      {
       case Init:
          {
             // this tells everyone listening, "i am starting a game"
             d_io->publish ( m_D_pub );
          }
          break;
       case Waiting:
          {
             boost::thread t( delay_thread , 10, std::bind ( &dealer::timer_expired , this ) );
          }
          break;
       case WaitingForOthers:
#ifdef XXX
          // time to start the game and deal some cards
          m_G_pub.gstate = waiting;
          //  two for the dealer
          m_G_pub.dealer_cards[0].valid = true;
          m_G_pub.dealer_cards[0].card = four;
          m_G_pub.dealer_cards[0].suite = clubs;
          m_G_pub.dealer_cards[1].valid = true;
          m_G_pub.dealer_cards[1].card = six;
          m_G_pub.dealer_cards[1].suite = spades;
         
          //  one for each player
          for (unsigned int i=0;i<m_number_of_players;i++)
          {
             m_G_pub.p[ i ].cards[ 0 ].valid  = true;
             m_G_pub.p[ i ].cards[ 0 ].card   = jack;
             m_G_pub.p[ i ].cards[ 0 ].suite  = diamonds;
          }
          g_io->publish ( m_G_pub );
#endif
          break;
       case Dealing:
          break;
       case WaitingForPlayer:
          break;
       case Done:
          break;
      }

      // on entrance
      switch (next_state)
      {
       case Init:
          break;
       case Waiting:
          break;
       case WaitingForOthers:
          {
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
       case Dealing:
#ifdef XXX
          if (m_Player_recv)
          {
std::cout << "got something from a player " << std::endl;
             if (m_P_sub.A == standing)
             {
                 // go to next player
                 m_G_pub.active_player++;
                 if ( (int) m_number_of_players >= (int) m_G_pub.active_player )
                 {
                     // do something
                 }
             }
          }
          // 
          m_G_pub.gstate = playing;
          m_G_pub.active_player = 0;
          m_G_pub.p[ 0 ].cards[ 1 ].valid  = true;
          m_G_pub.p[ 0 ].cards[ 1 ].card   = ten;
          m_G_pub.p[ 0 ].cards[ 1 ].suite  = hearts;
          g_io->publish ( m_G_pub );
#endif
          break;
       case WaitingForPlayer:
          break;
       case Done:
          std::cout << "Done." << std::endl;
          exit( 0 );
          break;
      }
      // make the transition
      if ( m_dealer_state != next_state )
      {
          std::cout << "State change from " << print_state ( m_dealer_state ) 
                    << " to " << print_state ( next_state ) << std::endl;
      }
      m_dealer_state = next_state;
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
   // this is called by the timer thread callback when the delay has expired
   // note: only one timer can be active at a time
   lock ();
   m_timer_event = true;
   manage_state ();
   unlock ();
}

void dealer::external_data (Player P)
{
   lock ();
   // this is called when data is received
   m_P_sub = P;
   m_Player_recv = true;
   manage_state ();
   unlock ();
}

void dealer::external_data (Dealer D)
{
   lock ();
   // this is called when data is received
   m_D_sub = D;
   m_Dealer_recv = true;
   manage_state ();
   unlock ();
}

void dealer::external_data (Game G)
{
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
