// This code is derived from the PrismTech example listener
//
#include <string>
#include <sstream>
#include <iostream>
#include "DDSEntityManager.h"
#include "ccpp_UberCasino.h"
#include "vortex_os.h"
#include "PlayerDataListener.h"

#include <iostream>


#include "io.h"

using namespace DDS;
using namespace UberCasino;

player_publisher::player_publisher ()
{

  DDSEntityManager mgr;

  // create domain participant
  char partition_name[] = "Listener example";
  mgr.createParticipant(partition_name);

  //create type
  PlayerTypeSupport_var mt = new PlayerTypeSupport();
  mgr.registerType(mt.in());

  //create Topic
  char topic_name[] = "ListenerData_Msg";
  mgr.createTopic(topic_name);

////
  //create Subscriber
  mgr.createSubscriber();

  // create DataReader
  mgr.createReader();

  DataReader_var dreader = mgr.getReader();
  PlayerDataListener *myListener = new PlayerDataListener();
  myListener->m_MsgReader = PlayerDataReader::_narrow(dreader.in());
  checkHandle(myListener->m_MsgReader.in(), "MsgDataReader::_narrow");

  cout << "=== [ListenerDataSubscriber] set_listener" << endl;
  DDS::StatusMask mask =
           DDS::DATA_AVAILABLE_STATUS | DDS::REQUESTED_DEADLINE_MISSED_STATUS;
  myListener->m_MsgReader->set_listener(myListener, mask);
  cout << "=== [ListenerDataSubscriber] Ready ..." << endl;
  //myListener->m_closed = false;

////
  //create Publisher
  mgr.createPublisher();

  // create DataWriter
  mgr.createWriter();

  // Publish Events
  DataWriter_var dwriter = mgr.getWriter();
  PlayerDataWriter_var listenerWriter = PlayerDataWriter::_narrow(dwriter.in());

  Player msgInstance; /* Example on Stack */
  msgInstance.count = 1;
  cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  cout << "    count  : " << msgInstance.count << endl;

  DDS::InstanceHandle_t handle = listenerWriter->register_instance(msgInstance);
  ReturnCode_t status = listenerWriter->write(msgInstance, handle);
  checkStatus(status, "MsgDataWriter::write");

  sleep(5);

  msgInstance.count = 2;
  cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  cout << "    count  : " << msgInstance.count << endl;

  status = listenerWriter->write(msgInstance, handle);
  checkStatus(status, "MsgDataWriter::write");
  sleep(100);
  /* Remove the DataWriters */
  mgr.deleteWriter(listenerWriter.in ());

  /* Remove the Publisher. */
  mgr.deletePublisher();

  /* Remove the Topics. */
  mgr.deleteTopic();

  /* Remove Participant. */
  mgr.deleteParticipant();

}

player_publisher::~player_publisher ()
{
}

player_subscriber::player_subscriber ()
{
}

player_subscriber::~player_subscriber ()
{
}



bool player_io::publish ( Player D)
{
  //Player msgInstance; /* Example on Stack */
  //msgInstance.userID = 1;
  //msgInstance.message = DDS::string_dup("Hello World");
  //cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  //cout << "    userID  : " << msgInstance.userID << endl;
  //cout << "    Message : \"" << msgInstance.message << "\"" << endl;
  static Player D1;
  D1 = D;
  std::cout << "i am sending a player " << std::endl;
  DDS::InstanceHandle_t handle = PlayerWriter->register_instance( D1 );
  ReturnCode_t status = PlayerWriter->write(D1, handle);
  checkStatus(status, "MsgDataWriter::write");

  return true;
}
bool player_io::subscribe (void *S)
{
   return true;
}
player_io::player_io ( bool s )
{
  // create domain participant
  char partition_name[] = "Listener example";
  mgr.createParticipant(partition_name);

  //create type
  PlayerTypeSupport_var mt = new PlayerTypeSupport();
  mgr.registerType(mt.in());

  //create Topic
  char topic_name[] = "ListenerData_Msg";
  mgr.createTopic(topic_name);

  if (!s)
  {
  //create Publisher
  mgr.createPublisher ( );

  // create DataWriter
  mgr.createWriter ( );

  // Publish Events
  DataWriter_var dwriter = mgr.getWriter();
  PlayerWriter = PlayerDataWriter::_narrow(dwriter.in());
  }
  else
  {
  //create Subscriber
  mgr.createSubscriber();

  // create DataReader
  mgr.createReader();

  DataReader_var dreader = mgr.getReader();
  //PlayerDataListener *myListener = new PlayerDataListener();
  myListener = new PlayerDataListener();
  myListener->m_MsgReader = PlayerDataReader::_narrow(dreader.in());
  checkHandle(myListener->m_MsgReader.in(), "PlayerDataReader::_narrow");

  cout << "=== [ListenerDataSubscriber] set_listener" << endl;
  DDS::StatusMask mask = DDS::DATA_AVAILABLE_STATUS | 
                         DDS::LIVELINESS_CHANGED_STATUS | 
                         DDS::REQUESTED_DEADLINE_MISSED_STATUS |
                         DDS::SAMPLE_REJECTED_STATUS |
                         DDS::SUBSCRIPTION_MATCHED_STATUS |
                         DDS::OFFERED_INCOMPATIBLE_QOS_STATUS;
           //DDS::DATA_AVAILABLE_STATUS | DDS::REQUESTED_DEADLINE_MISSED_STATUS;
  myListener->m_MsgReader->set_listener(myListener, mask);
  cout << "=== [ListenerDataSubscriber] Ready ..." << endl;
  //myListener->m_closed = false;
  }
  if (!s)
  {
  Player msgInstance; /* Example on Stack */
  msgInstance.count = 1;
  cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  cout << "    userID  : " << msgInstance.count << endl;

  ReturnCode_t status = PlayerWriter->write(msgInstance, 0);
  checkStatus(status, "MsgDataWriter::write");

  msgInstance.count = 2;
  cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  cout << "    userID  : " << msgInstance.count << endl;

  status = PlayerWriter->write(msgInstance, 0);
  checkStatus(status, "MsgDataWriter::write");

  //myListener->m_guardCond->set_trigger_value(false);
  }
#ifdef XXX
  // waitset used to avoid spinning in the loop below
  DDS::WaitSet_var ws = new DDS::WaitSet();
  ws->attach_condition(myListener->m_guardCond);
  DDS::ConditionSeq condSeq;
  while (!myListener->m_closed && count < 1500 ){
    // To avoid spinning here. We can either use a sleep or better a WaitSet.
    ws->wait(condSeq, timeout);
    myListener->m_guardCond->set_trigger_value(false);
    ++count;
 }
#endif



}

player_io::~player_io ()
{
   std::cout << "player io destructor " << std::endl;
   /* Remove the DataWriters */
   mgr.deleteWriter( PlayerWriter.in () );

   /* Remove the Publisher. */
   mgr.deletePublisher();

   /* Remove the Topics. */
   mgr.deleteTopic();

   /* Remove Participant. */
   mgr.deleteParticipant();
}

bool dealer_io::publish ( Dealer D)
{
   std::cout << "i am sending a dealer " << std::endl;
   ReturnCode_t status = DealerWriter->write(D, 0);
   checkStatus(status, "MsgDataWriter::write");
   return true;
}
bool dealer_io::subscribe (void *S)
{
   return true;
}
dealer_io::dealer_io ()
{
   std::cout << "dealer io constructor" << std::endl;
}

dealer_io::~dealer_io ()
{
   std::cout << "in dealer io destructor " << std::endl;
}
