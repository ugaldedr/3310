#include <string>
#include <iostream>
#include "DDSEntityManager.h"
#include "ccpp_UberCasino.h"
#include "PlayerDataListener.h"

using namespace DDS;
using namespace UberCasino;

int main(int argc, char* argv[])
{
  PlayerSeq msgList;
  SampleInfoSeq infoSeq;
  int count = 0;
  Duration_t timeout = { 0, 200000000 };
  DDSEntityManager mgr;

   // create domain participant
  char partition_name[] = "Listener example";
  mgr.createParticipant(partition_name);

  //create type
  PlayerTypeSupport_var st = new PlayerTypeSupport();
  mgr.registerType(st.in());

  //create Topic
  char topic_name[] = "ListenerData_Msg";
  mgr.createTopic(topic_name);

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
  myListener->m_closed = false;

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

  cout << "===[ListenerDataSubscriber] Market Closed" << endl;

  //cleanup
  mgr.deleteReader(myListener->m_MsgReader.in ());
  mgr.deleteSubscriber();
  mgr.deleteTopic();
  mgr.deleteParticipant();
  delete myListener;
  cout << "Completed Listener example." << endl;
  return 0;
}
