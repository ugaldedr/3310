#include <iostream>
#include <string>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "player.h"

static player* PTR;
#include "callback.h" // uses PTR as a global
int player_mode;

void HitCB(Fl_Widget* w, void* p)
{
	
    
}

void StandCB(Fl_Widget*w, void* p)
{

}

void DoubleCB(Fl_Widget *w, void* p)
{

}

void PlayerCB(Fl_Widget *w, void* p)
{
	
}

void GameTypeCB(Fl_Widget* w, void* p)
{

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
   P.start_window->show();
   return Fl::run();
   //create player group
 /*  
    Players = new FL_Group(150, 50, 800, 450);
    {
	Players->begin();
	Fl_Pack *inputBoxes = new Fl_Pack(150, 50, 200, 400);
	{
		inputBoxes->begin();
		Fl_Input *name = new Fl_Input(200, 200, 250, 25, "Player Name");

		inputBoxes->end();
	}
	Fl_Button *NewPlayer = new Fl_Button(725, 400, 60, 30, "Join");
	NewPlayer->callback(PlayerCB);


    }
    
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

