#include <iostream>

#include "dealer.h"

int main ( int argc, char* argv[] )
{
   // create the dealer object
   dealer D = dealer ();
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

