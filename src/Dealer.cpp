#include <iostream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "dealer.h"

static dealer* PTR;

#include "callback.h" // uses the PTR var

int main ( int argc, char* argv[] )
{
   // create the dealer object
   dealer D = dealer ();
   PTR = &D;

   boost::uuids::uuid uuid = boost::uuids::random_generator()();
   std::cout << uuid << std::endl;
   D.setuuid ( uuid );
   D.setName ("Bud"); // should this be entered by the user. probably!

   std::cout << "Welcome to UberCasino.  The fast paced, command line BlackJack system." << std::endl;
   std::cout << "-------------------------------------------" << std::endl;

   std::cout << "The dealers name is \"Bud\"." << std::endl;
   std::cout <<  "With a UUID of " << uuid << std::endl;
   std::cout << "-------------------------------------------" << std::endl;

   std::cout << "Enter 'start' to begin the game" << std::endl;
   std::cout << "Enter 'q' to exit" << std::endl;
   std::cout << "-------------------------------------------" << std::endl;

   char line[100]; // how large to make it? who knows.
   while (std::cin.getline(line, sizeof(line)))
   {
     if (line[0] == 'q' ) break;
     D.user_input ( std::string (line) );
   }


   return 0;
}

