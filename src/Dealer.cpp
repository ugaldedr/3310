#include <iostream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "dealer.h"

static dealer* DPTR;
void _cb ( UberCasino::Player P )
{
#ifdef DEBUG_PRINT
  std::cout << "RECEIVED" ;
#endif
  DPTR->external_data ( P );
}
void _cb ( UberCasino::Dealer D )
{
#ifdef DEBUG_PRINT
  std::cout << "\nRECEIVED -- Dealer" << std::endl;
  boost::uuids::uuid u;
  memcpy(&u, D.uuid, 16);
  std::cout << "   uuid " << boost::uuids::to_string( u ) << std::endl;
  std::cout << "   name " << D.name << std::endl;
  memcpy(&u, D.game_uuid, 16);
  std::cout << "   game_uuid " << boost::uuids::to_string( u ) << std::endl;
#endif
  DPTR->external_data ( D );
}
void _cb ( UberCasino::Game G )
{
#ifdef DEBUG_PRINT
#endif
  DPTR->external_data ( G );
}

int main ( int argc, char* argv[] )
{
   // create the dealer object
   dealer D = dealer ();
   DPTR = &D;
   boost::uuids::uuid uuid = boost::uuids::random_generator()();
   std::cout << uuid << std::endl;
   D.setuuid ( uuid );
   D.setName ("Bud");
   // a buffer to accept input
   char line[100];
   // the main loop
   std::cout << "Welcome to UberCasino.  The fast paced, command line " << std::endl;
   std::cout << "BlackJack system. " << std::endl << std::endl;
   std::cout << "The dealers name is \"Bud\" and his UUID is " << uuid << std::endl;
   std::cout << "Enter 'start' to begin the game" << std::endl;
   std::cout << "Enter 'q' to exit" << std::endl;
   std::cout << "-------------------------------------------" << std::endl;
   while (std::cin.getline(line, 100))
   {
     if (line[0] == 'q' ) break;
     D.user_input ( std::string (line) );
   }


   return 0;
}

