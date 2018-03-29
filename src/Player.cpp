#include <iostream>
#include "player.h"

static player* PPTR;
void _cb ( UberCasino::Player P )
{
  PPTR->external_data ( P );
}
void _cb ( UberCasino::Dealer D )
{
  PPTR->external_data ( D );
}
void _cb ( UberCasino::Game G )
{
  PPTR->external_data ( G );
}

int main ( int argc, char* argv[] )
{
   // create the dealer object
   player P = player ();
   PPTR = &P;
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

