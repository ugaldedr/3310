#include <iostream>
#include <string>

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
#include "player.h"

static player* PTR;
#include "callback.h" // uses PTR as a global


Fl_Double_Window* main_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(700, 340);
    w = o; if (w) {/* empty */}
    o->box(FL_OFLAT_BOX);
    o->color((Fl_Color)100);
    o->labelfont(10);
    o->labelcolor((Fl_Color)33);
    { Fl_Button* o = new Fl_Button(325, 265, 70, 20, "Start");
      o->box(FL_PLASTIC_ROUND_DOWN_BOX);
      o->labelcolor((Fl_Color)1);
    } // Fl_Button* o
    { Fl_Text_Display* o = new Fl_Text_Display(335, 135, 50, 30, "UberCasino");
      o->box(FL_NO_BOX);
      o->labeltype(FL_EMBOSSED_LABEL);
      o->labelfont(8);
      o->labelsize(91);
      o->labelcolor((Fl_Color)96);
    } // Fl_Text_Display* o
    { Fl_Choice* o = new Fl_Choice(275, 180, 225, 15, "Select Game Type");
      o->down_box(FL_BORDER_BOX);
      o->textfont(10);
      o->textcolor((Fl_Color)128);
    } // Fl_Choice* o
    { new Fl_Button(5, 305, 70, 25, "EXIT");
    } // Fl_Button* o
    o->end();
  } // Fl_Double_Window* o
  { Fl_Double_Window* o = new Fl_Double_Window(1295, 635);
    w = o; if (w) {/* empty */}
    o->box(FL_OSHADOW_BOX);
    o->color(FL_DARK_GREEN);
    o->align(Fl_Align(133));
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 1295, 20, "This is the menu");
      o->labeltype(FL_SHADOW_LABEL);
      o->labelfont(10);
      o->labelcolor(FL_GRAY0);
      o->textfont(8);
      o->textcolor(FL_GRAY0);
    } // Fl_Menu_Bar* o
    { Fl_Box* o = new Fl_Box(0, -4, 75, 24, "Exit");
      o->labelfont(5);
    } // Fl_Box* o
    { Fl_Value_Output* o = new Fl_Value_Output(140, 353, 30, 22, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { Fl_Value_Output* o = new Fl_Value_Output(275, 256, 30, 24, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { Fl_Value_Output* o = new Fl_Value_Output(455, 201, 25, 24, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { Fl_Value_Output* o = new Fl_Value_Output(645, 196, 30, 24, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { new Fl_Counter(1095, 455, 64, 20, "counter:");
    } // Fl_Counter* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(5, 260, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)160);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(30, 215, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)160);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(80, 180, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)160);
    } // Fl_Repeat_Button* o
    { Fl_Box* o = new Fl_Box(410, 276, 440, 74, "BlackJack");
      o->box(FL_GLEAM_UP_BOX);
      o->color((Fl_Color)108);
      o->labeltype(FL_EMBOSSED_LABEL);
      o->labelfont(10);
      o->labelsize(84);
      o->labelcolor((Fl_Color)57);
    } // Fl_Box* o
    { Fl_Slider* o = new Fl_Slider(55, 245, 135, 95, "P1 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Slider* o = new Fl_Slider(185, 140, 140, 105, "P2 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Slider* o = new Fl_Slider(350, 80, 145, 105, "P3 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Slider* o = new Fl_Slider(560, 75, 155, 105, "P4 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Box* o = new Fl_Box(870, 419, 120, 61, "Discard Tray");
      o->box(FL_DIAMOND_DOWN_BOX);
      o->color((Fl_Color)44);
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(250, 428, 120, 67, "Dealing Shoe");
      o->box(FL_GLEAM_ROUND_DOWN_BOX);
      o->color((Fl_Color)25);
    } // Fl_Box* o
    { Fl_Slider* o = new Fl_Slider(535, 415, 185, 120, "Dealer\'s Cards");
      o->box(FL_ROUND_DOWN_BOX);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(150, 120, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)158);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(335, 45, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)174);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(530, 35, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)145);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(200, 95, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)158);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(390, 30, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)174);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(590, 30, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)145);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(255, 80, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)158);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(440, 25, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)174);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(655, 30, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)145);
    } // Fl_Repeat_Button* o
    { Fl_Slider* o = new Fl_Slider(780, 80, 145, 110, "P5 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Slider* o = new Fl_Slider(970, 130, 140, 100, "P6 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Slider* o = new Fl_Slider(1090, 255, 140, 95, "P7 Cards");
      o->box(FL_ROUND_UP_BOX);
      o->color((Fl_Color)24);
      o->labelfont(5);
      o->align(Fl_Align(514));
    } // Fl_Slider* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(1200, 215, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)114);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(1000, 80, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)243);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(780, 35, 50, 40, "Hit");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)3);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(1235, 250, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)114);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(1055, 90, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)243);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(840, 35, 50, 40, "Double");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)3);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(1235, 300, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)114);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(1100, 115, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)243);
    } // Fl_Repeat_Button* o
    { Fl_Repeat_Button* o = new Fl_Repeat_Button(895, 45, 50, 40, "Stand");
      o->box(FL_OSHADOW_BOX);
      o->color((Fl_Color)3);
    } // Fl_Repeat_Button* o
    { Fl_Value_Output* o = new Fl_Value_Output(1180, 361, 30, 24, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { Fl_Value_Output* o = new Fl_Value_Output(1050, 246, 30, 24, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { Fl_Value_Output* o = new Fl_Value_Output(865, 206, 30, 24, "PointVal:");
      o->box(FL_OVAL_BOX);
    } // Fl_Value_Output* o
    { Fl_Counter* o = new Fl_Counter(1175, 0, 87, 20, "No of Players Playing:");
      o->color((Fl_Color)128);
      o->labelfont(5);
      o->align(Fl_Align(FL_ALIGN_LEFT));
    } // Fl_Counter* o
    o->end();
    o->resizable(o);
  } // Fl_Double_Window* o
  return w;
}

int main ( int argc, char* argv[] )
{
   // create the dealer object
   player P = player ();
   PTR = &P; // see callback.h to understand this
   // set the name
   if ( argc == 1 ) // meaning there are none
   {
      P.setName ("None");
   }
   else
   {
      P.setName (argv[1]);
   }

   // player unique ID
   boost::uuids::uuid uuid = boost::uuids::random_generator()();
   std::cout << "The players id is " << uuid << std::endl;
   memcpy ( P.m_P.uid, &uuid, sizeof ( P.m_P.uid ) );
   P.m_my_uid = uuid;

/*processing to turn UUID into a string so that it can be displayed
string tmp = boost::uuids::to_string(uuid);
const char* id = tmp.c_str();
string text = "The Player ID is: ";
text += id;
const char* text1 = text.c_str();*/

   //Main window for UberCasino
/*
   Fl_Double_Window* win = main_window();
   win->show();
   return Fl::run();
*/
   // a buffer to accept input
   char line[100];
   // the main loop
   std::cout << "Enter 'q' to exit" << std::endl;
   std::cout << "-------------------------------------------" << std::endl;
   while (std::cin.getline(line, 100))
   {
     if (line[0] == 'q' ) break;
     P.user_input ( std::string (line) );
   }


}

