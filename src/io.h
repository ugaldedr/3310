#ifndef IO_H
#define IO_H
#include <string>
#include <sstream>
#include <iostream>
#include <functional>

#include "DDSEntityManager.h"
#include "ccpp_UberCasino.h"
#include "vortex_os.h"

using namespace DDS;
using namespace UberCasino;

template <typename DATA_READER_VAR> 
class read_listener: public virtual DDS::DataReaderListener
{
  public:
     DATA_READER_VAR m_Reader;
     read_listener ()
     {
        
     }
     /* Callback method implementation. */
      virtual void on_data_available(DDS::DataReader_ptr reader)
      {
std::cout << "on data available" << std::endl;
      }

      virtual void on_requested_deadline_missed(DDS::DataReader_ptr reader,
        const DDS::RequestedDeadlineMissedStatus &status)
      {
      }

      virtual void on_requested_incompatible_qos(DDS::DataReader_ptr reader,
        const DDS::RequestedIncompatibleQosStatus &status)
      {
      }

      virtual void on_sample_rejected(DDS::DataReader_ptr reader, const DDS
        ::SampleRejectedStatus &status)
      {
      }

      virtual void on_liveliness_changed(DDS::DataReader_ptr reader, const DDS
        ::LivelinessChangedStatus &status)
      {
      }

      virtual void on_subscription_matched(DDS::DataReader_ptr reader, const
        DDS::SubscriptionMatchedStatus &status)
      {
      }

      virtual void on_sample_lost(DDS::DataReader_ptr reader, const DDS
        ::SampleLostStatus &status)
      {
      }

};

// Interfaces to the pub/sub network
// this code is derived (copied!) from the opensplice examples

template <typename MSG, typename TYPE_SUPPORT_VAR, class TYPE_SUPPORT,
          typename DATA_WRITER_VAR, class DATA_WRITER, typename DATA_READER_VAR, class DATA_READER>
class dds_io
{
   private:
      DDSEntityManager mgr;
      TYPE_SUPPORT_VAR mt;
      DataWriter_var dwriter;
      DATA_WRITER_VAR listenerWriter;
      DataReader_var dreader;
      //DATA_READER_VAR listenerReader;
   public:
      dds_io ( int i)
      {
         dds_io ( (char*) "xxx" , false );
      }

      dds_io ( char *topicName, bool pub = false, bool sub = false ) 
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

         if (pub) // meaning we intend to publish from this object
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

         if (sub)
         {
           // this is a subscription
           //create Subscriber
           mgr.createSubscriber();

           // create DataReader
           mgr.createReader();

           DataReader_var dreader = mgr.getReader();
       
           read_listener<DATA_READER_VAR> *myListener = new read_listener<DATA_READER_VAR> ();

           myListener->m_Reader = DATA_READER::_narrow(dreader.in());
           checkHandle(myListener->m_Reader.in(), "MsgDataReader::_narrow");

           cout << "=== [ListenerDataSubscriber] set_listener" << endl;
              DDS::StatusMask mask =
                    DDS::DATA_AVAILABLE_STATUS;
           myListener->m_Reader->set_listener(myListener, mask);
           cout << "=== [ListenerDataSubscriber] Ready ..." << endl;


         }
      }

      void subscribe ( std::function <void (void)> callback )
      {
      }

      void publish ( MSG D )
      {
         std::cout << "publishing" << std::endl;
         ReturnCode_t status = listenerWriter->write(D, DDS::HANDLE_NIL );
         checkStatus(status, "MsgDataWriter::write");
      }
};

#endif
