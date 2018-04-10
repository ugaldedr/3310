#include <iostream>
#include <string>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Fl/Fl.H>		// FLTK headers
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>

#include "player.h"

static player* PTR;
#include "callback.h" // uses PTR as a global

string const ace_table[10][10] ={
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //A + 10
	{"Hit","Hit","Hit","Hit","Stand","Stand","Stand","Hit","Hit","Hit"},//A+A
	{"Hit","Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+2
	{"Hit","Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+3
	{"Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+4
	{"Hit","Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+5
	{"Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"},//A+6
	{"Hit","Hit","Stand","DoubleDown","DoubleDown","DoubleDown","DoubleDown","Stand","Stand","Hit"},//A+7
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"},//A+8
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}//A+9
	};
string const total_table[20][10] = {
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //20
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //1
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //2
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //3
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //4
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //5
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //6
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //7
	{"Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit","Hit"}, //8
	{"Hit","Hit","Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","Hit","Hit","Hit"}, //9
	{"Hit","Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown"}, //10
	{"Hit","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown","DoubleDown"}, //10
	{"Hit","Hit","Hit","Hit","Stand","Stand","Stand","Hit","Hit","Hit"}, //12
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //13
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //14
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //15
	{"Hit","Hit","Stand","Stand","Stand","Stand","Stand","Hit","Hit","Hit"}, //16
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //17
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"}, //18
	{"Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand","Stand"} //19
	};

//AUTOMATED PLAYER BEHAVIOR FUNCTIONS

string reckless_mode(int total)
{
	if(total == 10 || total == 11)
	{
		return "DoubleDown";
	}
	if(total < 21)
	{
		return "Hit";
	}
	else //total == 21
	{
		return "Stand";
	}
}

string conservative_mode(int total)
{
	if(total <= 11)
	{
		return "Hit";
	}
	else
	{
		return "Stand";
	}
}

string basic_strategy(int card1, int card2, int hand_size, int total, int dealer_card)
{
	if(hand_size == 2)
	{
		if(card1 == 1)
		{
			return ace_table[card2][dealer_card];
		}
		else if(card2 == 1)
		{
			return ace_table[card1][dealer_card];
		}
	}
	else
	{
		return total_table[total][dealer_card];
	}
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

//processing to turn UUID into a string so that it can be displayed
string tmp = boost::uuids::to_string(uuid);
const char* id = tmp.c_str();
string text = "The Player ID is: ";
text += id;
const char* text1 = text.c_str();

/*   //Main window for UberCasino
   Fl_Window win(1600,900,"UberCasino"); 
Fl_Text_Display display(150,150,450,125,text1);
   win.show();
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

