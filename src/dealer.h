#ifndef DEALER_H
#define DEALER_H

#include <string>

#include "io.h"

class dealer
{
   private:
      enum dealer_state_t {init,waiting,waiting_for_more,dealing} dealer_state;
      std::string name;
      void manage_state ();
      bool timer_event;
      bool user_event;
      std::string user_event_string;
      bool external_event;
      dds_io<Player,PlayerTypeSupport_var,PlayerTypeSupport,PlayerDataWriter_var,
             PlayerDataWriter> *p_io;
      dds_io<Dealer,DealerTypeSupport_var,DealerTypeSupport,DealerDataWriter_var,
             DealerDataWriter> *d_io;
   public:
      void setName (std::string);
      // There are 3 possible inputs to the dealer
      //    1. timer expiration
      //    2. external data recieved
      //    3. user input
      void timer_expired ();
      void external_data ();
      void user_input (std::string);
      dealer ();
      ~dealer ();
};
#endif
