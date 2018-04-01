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
}

void dealer::unlock ()
{
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
   }
   return retval;
}

void dealer::manage_state ()
{
   // determine if we have a state transition
   bool transition = false;
   dealer_state_t next_state = m_dealer_state;

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
          break;
       case Dealing:
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
          break;
       case Dealing:
          break;
         // no default, we want to specify everything
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

}

dealer::~dealer ()
{
}
