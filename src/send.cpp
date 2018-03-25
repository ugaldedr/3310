#include <string>
#include <sstream>
#include <iostream>
#include "DDSEntityManager.h"
#include "ccpp_UberCasino.h"


using namespace DDS;
using namespace UberCasino;

int main(int argc, char* argv[])
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

  //create Publisher
  mgr.createPublisher();

  // create DataWriter
  mgr.createWriter();

  // Publish Events
  DataWriter_var dwriter = mgr.getWriter();
  PlayerDataWriter_var listenerWriter = PlayerDataWriter::_narrow(dwriter.in());

  Player msgInstance; /* Example on Stack */
  msgInstance.count = 1;
  msgInstance.uuid[0]='a';
  cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  cout << "    count  : " << msgInstance.count << endl;

  DDS::InstanceHandle_t handle = listenerWriter->register_instance(msgInstance);
  //DDS::InstanceHandle_t handle = DDS::HANDLE_NIL;
  ReturnCode_t status = listenerWriter->write(msgInstance, handle );
  checkStatus(status, "MsgDataWriter::write");

  Player msgInstance2; /* Example on Stack */
  msgInstance2.count = 2;
  msgInstance2.uuid[0]='b';
  cout << "=== [ListenerDataPublisher] writing a message containing :" << endl;
  cout << "    count  : " << msgInstance2.count << endl;

  handle = listenerWriter->register_instance(msgInstance2);
  status = listenerWriter->write(msgInstance2, handle );
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

  return 0;
}
