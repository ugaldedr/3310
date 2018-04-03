#include <iostream>
#include <functional>
#include <cstdlib>


#include <boost/thread.hpp>


#include "player.h"

void delay_thread ( int seconds, std::function <void(void)> callback)
{
  // this routine is created as a posix thread.
  sleep (seconds);
  callback ();
}

void player::lock ()
{
}

void player::unlock ()
{
}

std::string player::print_state ( player_state_t p )
{
   std::string retval;
   switch ( p )
   {
      case Init:
         retval = "Init";
         break;
      case Waiting:
         retval = "Waiting";
         break;
      case Playing:
         retval = "Playing";
         break;
      case Turn:
         retval = "Turn";
         break;
   }
   return retval;
}

void player::manage_state ()
{
   // determine if we have a state transition
   bool transition = false;
   player_state_t next_state = m_player_state;
   switch ( m_player_state )
   {
     case Init:
         if ( m_Dealer_recv )
         {
            next_state = Init; // not really needed
            transition = true;
         }
         if ( m_user_event )
         {
            // the user has entered a number, we hope
            m_dealer_idx = std::stoi ( m_user_event_string );
            if (m_dealer_idx < m_dealer_list.size () )
            {
               // this is the dealer we want to play with
               next_state = Waiting;
               transition = true;
            }
         }
         break;
     case Waiting:
         if ( m_timer_event )
         {
               // the dealer did not 'accept' us
               next_state = Init;
               transition = true;
         }
         if ( m_Game_recv )
         {
               next_state = Playing;
               transition = true;
         }
         break;
     case Playing:
         break;
     case Turn:
         break;
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
         }
         break;
         case Waiting:
         {
         }
         break;
         case Playing:
         {
         }
         case Turn:
         {
         }
         break;
      }

      // on entrance
      switch (next_state)
      {
         case Init:
         {
         }
         break;
         case Waiting:
         {
               // Send a Player , telling the dealer we
               // would like to join
               memcpy ( m_P.game_uid, 
                        m_dealer_list[m_dealer_idx].game_uid,
                        sizeof ( m_P.game_uid ) );
               m_P.A = idle;
               p_io->publish  ( m_P );
               // Wait 30 seconds for the dealer to act
               boost::thread t( delay_thread , 30, std::bind ( &player::timer_expired , this ) );
         }
         break;
         case Playing:
         {
               // need to decide what to do and send it
               m_P.A = standing;
               p_io->publish  ( m_P );
         }
         break;
         case Turn:
         {
         }
         default:
            break;
      }

      // make the transition
      if ( m_player_state != next_state )
      {
          std::cout << "State change from " << print_state (m_player_state)
                    << " to " << print_state ( next_state ) << std::endl;
      }
      m_player_state = next_state;
   }

   // clear all event flags
   m_timer_event = false;
   m_user_event  = false;
   m_Player_recv = false;
   m_Game_recv   = false;
   m_Dealer_recv = false;
 
}


void player::timer_expired ()
{
   // this is called by the timer thread callback when the delay has expired
   // note: only one timer can be active at a time
   lock ();
   m_timer_event = true;
   manage_state ();
   unlock ();
}

void player::external_data (Player P)
{
   // this is called when data is received
   lock ();
   m_P = P;
   m_Player_recv = true;
   manage_state ();
   unlock ();
}

void player::external_data (Dealer D)
{
   // this is called when data is received
   lock ();
   m_D = D;
   m_Dealer_recv = true;

   m_dealer_list.push_back ( D );
   std::cout << "The available dealers has changed:" << std::endl;
   std::cout << "Enter the idx to join.." << std::endl;
   for (unsigned int i=0;i<m_dealer_list.size ();i++)
   {
      std::cout << "Dealer # " << i 
                << " name " << m_dealer_list[i].name << std::endl;
   }
   manage_state ();
   unlock ();
}

void player::external_data (Game G)
{
   // only care about games
   // we are in. m_dealer_idx is the
   // key to figuring this out
   lock ();

   boost::uuids::uuid t;
   memcpy ( &t, G.game_uid, sizeof ( t ) );
   boost::uuids::uuid current_game; 
   //std::cout << "the index is " << m_dealer_idx << std::endl;
   memcpy ( &current_game, 
             m_dealer_list[m_dealer_idx].game_uid, 
             sizeof ( current_game ) );
   //std::cout << "Comparing " << t << " " << current_game << std::endl;

   boost::uuids::uuid my_uid;
   memcpy ( &my_uid, m_P.uid, sizeof ( my_uid ) );
   boost::uuids::uuid this_move_id;
   memcpy ( &this_move_id, G.p[G.active_player].uid, sizeof ( this_move_id ) );

   //m_G_pub.gstate = waiting_to_join; 
   if ( 
        (t == current_game && G.gstate != playing ) ||
        (t == current_game && G.gstate == playing && my_uid==this_move_id  &&
        G.p[G.active_player].cards[0].valid   ) )
   {
     m_Game_recv = true;
     m_G = G;
     manage_state ();
   }
   unlock ();
}

void player::user_input (std::string I)
{
   // this is called when the user types in input
   // from the console.  any / all input is accepted
   lock ();
   m_user_event_string = I;
   m_user_event = true;
   std::cout << "the input string is " << I << std::endl;
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

   m_dealer_list.clear ();

   m_P.balance = 1000.0;

   // event flags
   m_timer_event = false;
   m_user_event  = false;
   m_Player_recv = false;
   m_Game_recv   = false;
   m_Dealer_recv = false;
}

player::~player ()
{
}
