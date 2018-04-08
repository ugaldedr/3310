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


   return 0;
}

