#ifndef IO_H
#define IO_H
#include <string>
#include <sstream>
#include <iostream>
#include <functional>

#include "DDSEntityManager.h"
#include "ccpp_UberCasino.h"
#include "vortex_os.h"
#include "PlayerDataListener.h"

using namespace DDS;
using namespace UberCasino;


// Interfaces to the pub/sub network
// this code is derived (copied!) from the opensplice examples

template <typename MSG, typename TYPE_SUPPORT_VAR, class TYPE_SUPPORT,
          typename DATA_WRITER_VAR, class DATA_WRITER>
class dds_io
{
   private:
      DDSEntityManager mgr;
      TYPE_SUPPORT_VAR mt;
      DataWriter_var dwriter;
      DATA_WRITER_VAR listenerWriter;
   public:
      dds_io ( int i)
      {
         dds_io ( (char*) "xxx" , false );
      }

      dds_io ( char *topicName, bool pub_flag = false ) 
      {
         DDSEntityManager mgr;

         // create domain participant
         char partition_name[] = "Listener example";
         mgr.createParticipant(partition_name);

         //create type
         mt = new TYPE_SUPPORT();
         mgr.registerType(mt.in());

         //create Topic
         mgr.createTopic( topicName );

         if (pub_flag) // meaning we intend to publish from this object
         {
std::cout << "the pub flag is true" << std::endl;
            //create Publisher
            mgr.createPublisher();

            // create DataWriter
            mgr.createWriter();

           // Publish Events
           dwriter = mgr.getWriter();
           listenerWriter = DATA_WRITER::_narrow(dwriter.in());
         }
      }

      void subscribe ( std::function <void (void)> callback )
      {
      }

      void publish ( MSG D )
      {
         MSG msgInstance; 
         msgInstance =  D;
         std::cout << "publishing " << std::endl;
         DDS::InstanceHandle_t handle = listenerWriter->register_instance(msgInstance);
         ReturnCode_t status = listenerWriter->write(msgInstance, handle );
         checkStatus(status, "MsgDataWriter::write");
      }
};

#endif
