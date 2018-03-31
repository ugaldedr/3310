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
#ifdef XXX
//{Init,Waiting,WaitingForOthers,Dealing}
switch ()
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
#endif

void dealer::manage_state ()
{
   // determine if we have a state transition
   bool transition = false;
   dealer_state_t next_state;
   switch (dealer_state)
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
   // if there is a transition, then we have to run the exit 
   // and entrance processing
   if (transition)
   {
      // on exit
      switch (dealer_state)
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
#ifdef OLD
      switch (dealer_state)
      {
         case init:
         {
           // publish out the dealer structure
           std::cout << "i am publishing the dealer structure" << std::endl;
           d_io->publish ( m_D );
         }
         break;
         case waiting:
         {
         }
         break;
         case waiting_for_more:
         {
         }
         break;
         default:
            break;
      }
#endif

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
#ifdef OLD
      switch (next_state)
      {
         case init:
         {
         }
         break;
         case waiting:
         {
               // A sloppy way to delay a callback 
               boost::thread t( delay_thread , 5, std::bind ( &dealer::timer_expired , this ) );
         }
         break;
         case waiting_for_more:
         {
               // A sloppy way to delay a callback 
               boost::thread t( delay_thread , 10, std::bind ( &dealer::timer_expired , this ) );
         }
         break;
         default:
            break;
      }
#endif
      // make the transition
      if ( dealer_state != next_state )
      {
          std::cout << "State change from " << dealer_state << " to " << next_state << std::endl;
      }
      dealer_state = next_state;
   }
   // clear all event flags
   timer_event = false;
   user_event = false;
   external_event = false;
}


void dealer::timer_expired ()
{
   // this is called by the timer thread callback when the delay has expired
   // note: only one timer can be active at a time
   timer_event = true;
   manage_state ();
}

void dealer::external_data (Player P)
{
   // this is called when data is received
   m_P = P;
   external_event = true;
   manage_state ();
}

void dealer::external_data (Dealer D)
{
   // this is called when data is received
   m_D = D;
   external_event = true;
   manage_state ();
}

void dealer::external_data (Game G)
{
   // this is called when data is received
   m_G = G;
   external_event = true;
   manage_state ();
}

void dealer::user_input (std::string I)
{
   // this is called when the user types in input
   // from the console.  any / all input is accepted
   user_event_string = I;
   user_event = true;
   manage_state ();
}

void dealer::setName (std::string Name )
{
   strncpy ( m_D.name, Name.c_str(), sizeof (m_D.name) -1 );

}

void dealer::setuuid (boost::uuids::uuid uuid )
{
   boost::uuids::uuid u;
   std::copy(u.begin(), u.end(), m_D.uuid);
}
dealer::dealer ()
{
   // member variables
   dealer_state = Init;

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
   timer_event = false;
   user_event = false;
   external_event = false;
}

dealer::~dealer ()
{
}
