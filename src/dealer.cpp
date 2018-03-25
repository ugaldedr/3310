#include <iostream>
#include <functional>

#include <boost/thread.hpp>

#include "dealer.h"

static void delay_thread ( int seconds, std::function <void(void)> callback)
{
  // this routine is created as a posix thread.
  sleep (seconds);
  callback ();
}

void dealer::manage_state ()
{
   // determine if we have a state transition
   bool transition = false;
   dealer_state_t next_state;
   switch (dealer_state)
   {
      case init:
            if (user_event_string == "start")
            {
               transition = true;
               next_state = waiting;
               // A sloppy way to delay a callback
               boost::thread t( delay_thread , 10, std::bind ( &dealer::timer_expired , this ) );
               //Dealer D;
               //D_IO->publish  ( D );
               Player P;
               P.count = 1;
               P_IO->publish  ( P );
               P.count = 4;
               //P_IO->publish  ( P );
            }
            break;
      case waiting: 
            if ( timer_event )
            {
               player_publisher* Z = new player_publisher ();
               delete (Z);
               Player P;
               P.count = 2;
               //P_IO->publish  ( P );
               P.count = 3;
               //P_IO->publish  ( P );
               transition = true;
               next_state = waiting_for_more;
            }
            break;
      case waiting_for_more: break;
      case dealing: break;
   }
   // if there is a transition, then we have to run the exit 
   // and entrance processing
   if (transition)
   {
      // on exit
      switch (dealer_state)
      {
         default:
            break;
      }

      // on entrance
      switch (next_state)
      {
         default:
            break;
      }
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
std::cout << "DEALER TIMER EXPIRED " << std::endl;
   timer_event = true;
   manage_state ();
}

void dealer::external_data ()
{
std::cout << "dealer external data" << std::endl;
}

void dealer::user_input (std::string I)
{
   user_event_string = I;
   user_event = true;
   manage_state ();
}

void dealer::setName (std::string Name )
{
   name = Name;

}
dealer::dealer ()
{
   // member variables
   dealer_state = init;

   // member objects
   //D_IO = new dealer_io ();
   P_IO = new player_io (false);

   // event flags
   timer_event = false;
   user_event = false;
   external_event = false;
}

dealer::~dealer ()
{
#ifdef XX
   if (D_IO)
   {
     delete D_IO;
   }

   if (P_IO)
   {
     delete P_IO;
   }
#endif
}
