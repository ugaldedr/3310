#ifndef IO_H
#define IO_H
#include <string>
#include <sstream>
#include <iostream>
#include "DDSEntityManager.h"
#include "ccpp_UberCasino.h"
#include "vortex_os.h"
#include "PlayerDataListener.h"

using namespace DDS;
using namespace UberCasino;



// Interfaces to the pub/sub network
class player_publisher
{
   private:
      DDSEntityManager mgr;
   public:
     player_publisher ();
     ~player_publisher ();
};
class player_subscriber
{
   private:
      DDSEntityManager mgr;
   public:
      player_subscriber ();
      ~player_subscriber ();
};

class player_io
{
   private:
      DDSEntityManager mgr;
      PlayerDataWriter_var PlayerWriter;
      PlayerTypeSupport_var mt;
      PlayerDataListener *myListener;
   public:
      bool publish ( Player D);
      bool subscribe (void *S); // add the callback )
      player_io (bool);  // true if you subscribe
      ~player_io ();
};

class dealer_io
{
   private:
      DDSEntityManager mgr;
      DealerDataWriter_var DealerWriter;
   public:
      bool publish ( Dealer D);
      bool subscribe (void *S); // add the callback )
      dealer_io ();
      ~dealer_io ();
};
#endif
