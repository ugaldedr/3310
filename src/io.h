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

// Interfaces to the pub/sub network
// this code is derived (copied!) from the opensplice examples

void _cb ( UberCasino::Player );
void _cb ( UberCasino::Dealer );
void _cb ( UberCasino::Game );



//void external_data (Player P);
template <typename DATA_READER_VAR,typename SEQ,typename MSG> 
class read_listener: public virtual DDS::DataReaderListener
{
  public:
     DATA_READER_VAR m_Reader;
     typedef void (*cb_func_type)( MSG );
     cb_func_type cb_func;
     read_listener ()
     {
       cb_func = NULL;
     }
     void set_callback ( cb_func_type CB )
     {
       cb_func = CB;        
     }

     /* Callback method implementation. */
      virtual void on_data_available(DDS::DataReader_ptr reader)
      {
         DDS::ReturnCode_t status;
         SEQ msgList;

         SampleInfoSeq infoSeq;
         status = m_Reader->take(msgList, infoSeq, LENGTH_UNLIMITED,
                  ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
         checkStatus(status, "MsgDataReader::read");
         for (DDS::ULong i = 0; i < msgList.length(); i++)
         {
            printf("\n    --- message received ---%d\n",msgList.length());
            if (1)
            {
               MSG D;
               D = msgList[i];
               _cb ( D );
            }
         }
         status = m_Reader->return_loan(msgList, infoSeq);
         checkStatus(status, "MsgDataReader::return_loan");
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


template <typename MSG, typename SEQ, typename TYPE_SUPPORT_VAR, class TYPE_SUPPORT,
          typename DATA_WRITER_VAR, class DATA_WRITER, typename DATA_READER_VAR, class DATA_READER>
class dds_io
{
  typedef void (*cb_func_type)( MSG );
   private:
      DDSEntityManager mgr;
      TYPE_SUPPORT_VAR mt;
      DataWriter_var dwriter;
      DATA_WRITER_VAR listenerWriter;
      DataReader_var dreader;
      cb_func_type CB;
      //DATA_READER_VAR listenerReader;
   public:
      dds_io ( int i)
      {
         dds_io ( (char*) "xxx" , false );
      }

      void set_callback ( cb_func_type cb)
      {
         CB = cb;
      }
      dds_io ( char *topicName, bool pub = false, bool sub = false ) 
      {
         CB = NULL;
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
       
           read_listener<DATA_READER_VAR,SEQ,MSG> *myListener = new read_listener<DATA_READER_VAR,SEQ,MSG> ();

           myListener->set_callback ( CB );

           myListener->m_Reader = DATA_READER::_narrow(dreader.in());
           checkHandle(myListener->m_Reader.in(), "MsgDataReader::_narrow");

           cout << "=== [ListenerDataSubscriber] set_listener" << endl;
              DDS::StatusMask mask =
                    DDS::DATA_AVAILABLE_STATUS;
           myListener->m_Reader->set_listener(myListener, mask);
           cout << "=== [ListenerDataSubscriber] Ready ..." << endl;
         }
      }

      void publish ( MSG D )
      {
         std::cout << "publishing" << std::endl;
         ReturnCode_t status = listenerWriter->write(D, DDS::HANDLE_NIL );
         checkStatus(status, "MsgDataWriter::write");
      }
};

#endif
