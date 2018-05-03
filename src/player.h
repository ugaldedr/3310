#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>


#include <boost/thread.hpp>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

//FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Input_Choice.H>
#include <Fl/Fl_Int_Input.H>
#include <Fl/Fl_Multiline_Output.H>
#include <Fl/fl_ask.H>
#include <FL/Fl_JPEG_Image.H>

#include "io.h"
extern int player_mode;

Fl_Double_Window* main_window();

class player
{
   private:
      enum player_state_t {Init,StartHand,Playing,EndHand} m_player_state;
      std::string m_name;
      void manage_state ();
      void lock ();
      void unlock ();
      unsigned int m_dealer_idx;
      std::vector<Dealer> m_dealer_list;
      std::string m_user_event_string;
      boost::uuids::uuid m_current_game_uuid;
      boost::thread *m_timer_thread;
      float m_balance;
      bool m_timer_event;   // timer has expired
      bool m_user_event;    // user typed in something
      bool m_Player_recv;   // data recved
      bool m_Game_recv;     // data recved, the game UID matches
      bool m_Game_recv_idx; // data recved, the player index matches
      bool m_Dealer_recv;   // data recved
      //unsigned int player_mode;

      dds_io<Player,PlayerSeq,PlayerTypeSupport_var,PlayerTypeSupport,PlayerDataWriter_var,
             PlayerDataWriter,PlayerDataReader_var,PlayerDataReader> *p_io;
      dds_io<Dealer,DealerSeq,DealerTypeSupport_var,DealerTypeSupport,DealerDataWriter_var,
             DealerDataWriter,DealerDataReader_var,DealerDataReader> *d_io;
      dds_io<Game,GameSeq,GameTypeSupport_var,GameTypeSupport,GameDataWriter_var,
             GameDataWriter,GameDataReader_var,GameDataReader> *g_io;

      std::string to_string ( player_state_t );


   public:
      boost::uuids::uuid m_my_uid;
      Player m_P; // stores the last data
      Dealer m_D; 
      Game   m_G;
      Fl_Window* start_window;
      Fl_Window* play_window;

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


      static void startCB(Fl_Widget* w, void* p);
      void startCB2(Fl_Widget* w);
      void update_cards(UberCasino::card_t cards[], void* o);
      void update_dealer(UberCasino::card_t cards[], void* o);
      void change_status(string s);
      static void plusCB(Fl_Widget* w, void* p);
      void plusCB2(Fl_Widget* w);
      static void minusCB(Fl_Widget* w, void* p);
      void minusCB2(Fl_Widget* w);
      void update_bal();
      void update_total(int i);
      static void strategyCB(Fl_Widget* w, void* p);



      player ();
      ~player ();

};
#endif
