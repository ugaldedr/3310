/************************************************************************
 *
 * Copyright (c) 2010
 * PrismTech Ltd.
 * All rights Reserved.
 *
 * LOGICAL_NAME:    PlayerDataListener.cpp
 * FUNCTION:        .
 * MODULE:          .
 * DATE             August 2010.
 ************************************************************************
 *
 * This file contains the implementation of PlayerDataListener
 *
 ***/

#include "PlayerDataListener.h"
#include "CheckStatus.h"
#include <sstream>

using namespace DDS;
using namespace UberCasino;

// --------------------------------------------------------------------------------------------------------
//                                              PlayerDataListener                                     --
// --------------------------------------------------------------------------------------------------------

void PlayerDataListener::on_data_available(DDS::DataReader_ptr reader)
  THROW_ORB_EXCEPTIONS
{
  DDS::ReturnCode_t status;
  PlayerSeq msgList;
  SampleInfoSeq infoSeq;

  status = m_MsgReader->read(msgList, infoSeq, LENGTH_UNLIMITED,
    ANY_SAMPLE_STATE, NEW_VIEW_STATE, ANY_INSTANCE_STATE);
  checkStatus(status, "MsgDataReader::read");
  printf("=== [PlayerDataListener::on_data_available] - msgList.length : %d\n", msgList.length());
  for (DDS::ULong j = 0; j < msgList.length(); j++)
    {
      printf("\n    --- message received ---\n");
      printf("\n    userID  : %d\n", msgList[j].count);
      //printf("\n    Message : %s\n", msgList[j].message.m_ptr);
    }
  status = m_MsgReader->return_loan(msgList, infoSeq);
  checkStatus(status, "MsgDataReader::return_loan");
  // unblock the waitset in Subscriber main loop
  m_guardCond->set_trigger_value(true);
};

void PlayerDataListener::on_requested_deadline_missed(DDS::DataReader_ptr
  reader, const DDS::RequestedDeadlineMissedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [PlayerDataListener::on_requested_deadline_missed] : triggered\n");
  printf("\n=== [PlayerDataListener::on_requested_deadline_missed] : stopping\n");
  m_closed = true;
  // unblock the waitset in Subscriber main loop
  m_guardCond->set_trigger_value(true);
};

void PlayerDataListener::on_requested_incompatible_qos(DDS::DataReader_ptr
  reader, const DDS::RequestedIncompatibleQosStatus &status)
  THROW_ORB_EXCEPTIONS
{
  printf("\n=== [PlayerDataListener::on_requested_incompatible_qos] : triggered\n");
};

void PlayerDataListener::on_sample_rejected(DDS::DataReader_ptr reader, const
  DDS::SampleRejectedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [PlayerDataListener::on_sample_rejected] : triggered\n");
};

void PlayerDataListener::on_liveliness_changed(DDS::DataReader_ptr reader,
  const DDS::LivelinessChangedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [PlayerDataListener::on_liveliness_changed] : triggered\n");
};

void PlayerDataListener::on_subscription_matched(DDS::DataReader_ptr reader,
  const DDS::SubscriptionMatchedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [PlayerDataListener::on_subscription_matched] : triggered\n");
};

void PlayerDataListener::on_sample_lost(DDS::DataReader_ptr reader, const DDS
  ::SampleLostStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [PlayerDataListener::on_sample_lost] : triggered\n");
};
