#include <iostream>
#include "cb.h"
#include "dealer.h"

static dealer* DPTR;
void _cb ( UberCasino::Player P )
{
  std::cout << "i am calling the callback routine Player " << std::endl;
  DPTR->external_data ( P );
}
void _cb ( UberCasino::Dealer D )
{
  DPTR->external_data ( D );
  std::cout << "i am calling the callback routine Dealer " << std::endl;
}
void _cb ( UberCasino::Game G )
{
  DPTR->external_data ( G );
  std::cout << "i am calling the callback routine Game " << std::endl;
}

int main ( int argc, char* argv[] )
{
   // create the dealer object
   dealer D = dealer ();
   DPTR = &D;
   D.setName ("Bud");
   // a buffer to accept input
   char line[100];
   // the main loop
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

