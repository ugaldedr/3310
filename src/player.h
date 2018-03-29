#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "io.h"

class player
{
   private:
      enum dealer_state_t {init,waiting,waiting_for_more,dealing} dealer_state;
      std::string name;
      void manage_state ();
      bool timer_event;
      bool user_event;
      std::string user_event_string;
      bool external_event;
      dds_io<Player,PlayerSeq,PlayerTypeSupport_var,PlayerTypeSupport,PlayerDataWriter_var,
             PlayerDataWriter,PlayerDataReader_var,PlayerDataReader> *p_io;
      dds_io<Dealer,DealerSeq,DealerTypeSupport_var,DealerTypeSupport,DealerDataWriter_var,
             DealerDataWriter,DealerDataReader_var,DealerDataReader> *d_io;
      dds_io<Game,GameSeq,GameTypeSupport_var,GameTypeSupport,GameDataWriter_var,
             GameDataWriter,GameDataReader_var,GameDataReader> *g_io;
      Player m_P; // stores the last recieved data
      Dealer m_D; 
      Game   m_G;
   public:
      void setName (std::string);
      // There are 3 possible inputs to the dealer
      //    1. timer expiration
      //    2. external data recieved
      //    3. user input
      void timer_expired ();
      void external_data (Player P);
      void external_data (Dealer D);
      void external_data (Game G);
      void user_input (std::string);
      player ();
      ~player ();
};
#endif
