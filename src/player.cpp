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
std::cout << "m_user_event is true" << std::endl;
            // the user has entered a number, we hope
            m_dealer_idx = std::stoi ( m_user_event_string );
std::cout << "the number entered is " << m_dealer_idx << std::endl;
            if (m_dealer_idx < m_dealer_list.size () )
            {
               // this is the dealer we want to play with
               next_state = Waiting;
               transition = true;
            }
         }
         break;
     case Waiting:
         break;
     case Playing:
         break;
     case Turn:
         break;
   }
// enum player_state_t {Init,Waiting,Playing,Turn} m_player_state;
   // if there is a transition, then we have to run the exit 
   // and entrance processing
   if (transition)
   {
#ifdef XXX
      // on exit
      switch (m_player_state)
      {
         case Init:
         {
         }
         break;
         case Waiting:
         {
               static Player P;
               P.count++;
               p_io->publish  ( P );
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
         case init:
         {
         }
         break;
         case waiting:
         {
               // A sloppy way to delay a callback 
               boost::thread t( delay_thread , 5, std::bind ( &player::timer_expired , this ) );
         }
         break;
         case waiting_for_more:
         {
               // A sloppy way to delay a callback 
               boost::thread t( delay_thread , 10, std::bind ( &player::timer_expired , this ) );
         }
         break;
         default:
            break;
      }
#endif
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
   m_timer_event = true;
   manage_state ();
}

void player::external_data (Player P)
{
   // this is called when data is received
   m_P = P;
   m_Player_recv = true;
   manage_state ();
}

void player::external_data (Dealer D)
{
   // this is called when data is received
   m_D = D;
   m_Dealer_recv = true;

   m_dealer_list.push_back ( D );
   std::cout << "The available dealers has changed:" << std::endl;
   for (unsigned int i=0;i<m_dealer_list.size ();i++)
   {
      std::cout << "Dealer # " << i 
                << " name " << m_dealer_list[i].name << std::endl;
   }
   manage_state ();
}

void player::external_data (Game G)
{
   // this is called when data is received
   m_G = G;
   m_Game_recv = true;
   manage_state ();
}

void player::user_input (std::string I)
{
   // this is called when the user types in input
   // from the console.  any / all input is accepted
   m_user_event_string = I;
   m_user_event = true;
   std::cout << "the input string is " << I << std::endl;
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
