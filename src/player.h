#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

#include "io.h"

class player
{
   private:
      enum player_state_t {Init,Waiting,Playing,Turn} m_player_state;
      std::string m_name;
      void manage_state ();

      std::vector<Dealer> m_dealer_list;
      std::string m_user_event_string;

      bool m_timer_event; // timer has expired
      bool m_user_event;  // user typed in something
      bool m_Player_recv; // data recved
      bool m_Game_recv;   // data recved
      bool m_Dealer_recv; // data recved

      dds_io<Player,PlayerSeq,PlayerTypeSupport_var,PlayerTypeSupport,PlayerDataWriter_var,
             PlayerDataWriter,PlayerDataReader_var,PlayerDataReader> *p_io;
      dds_io<Dealer,DealerSeq,DealerTypeSupport_var,DealerTypeSupport,DealerDataWriter_var,
             DealerDataWriter,DealerDataReader_var,DealerDataReader> *d_io;
      dds_io<Game,GameSeq,GameTypeSupport_var,GameTypeSupport,GameDataWriter_var,
             GameDataWriter,GameDataReader_var,GameDataReader> *g_io;

      std::string print_state ( player_state_t );

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
