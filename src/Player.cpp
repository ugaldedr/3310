#include <iostream>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.



#include "player.h"

static player* PTR;
#include "callback.h" // uses PTR as a global

int main ( int argc, char* argv[] )
{
   // create the dealer object
   player P = player ();
   PTR = &P;
   P.setName ("jim");
   // a buffer to accept input
   char line[100];
   // the main loop
   std::cout << "Enter 'start' to begin the game" << std::endl;
   std::cout << "Enter 'q' to exit" << std::endl;
   std::cout << "-------------------------------------------" << std::endl;
   while (std::cin.getline(line, 100))
   {
     if (line[0] == 'q' ) break;
     P.user_input ( std::string (line) );
   }


   return 0;
}

