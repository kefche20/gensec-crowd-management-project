#ifndef GATEMANAGER_H
#define GATEMANAGER_H

#include <iostream>
#include <algorithm>

#include <map>
#include <list>
#include <queue>
#include <string>

#include "config.hpp"

#include "Gate.hpp"
#include "Heartbeat.hpp"
#include "Traffic.hpp"

#include "IGateListener.hpp"
#include "INodeManager.hpp"
#include "IDataCollector.hpp"
#include "ISender.hpp"

/*
  //TODO
  1. check move resource without uncessary cioy
  2. implement try catch
  3. template code for the mode of state machine
*/



/*
  //TODO:
  1. check neccessary of letting AddNode and RemoveNode as interface

*/

class GateManager : public IGateListener, public INodeManager, public ILocalCollector
{
private:
  int maxGateNum;
  int isActive;

  int openThresholdRate;  // Threshold to open a new gate
  int closeThresholdRate; // Threshold to close an idle gate
  std::list<Gate> gates;

  GeneralDutyState generalState;
  Traffic traffic; // keep track on close/open gate logic
  hrtbt::MetaAliveTracker metaAliveTracker;

  ISender *sender;

public:
  // Constructor
  GateManager(int maxGateNum, int openThreshold, int closeThreshold);
 

  void SetSender(ISender *sender);
   
  bool IsBusy() override;

 void  SetActivateState(bool sta) override;

  // Open the gate with the given ID
  void SetGateState(int id, bool sta);

  std::pair<int, int> GetLeastBusyGate();

  bool Add(int) override;

  bool Remove(int) override;

  // Add a person to the gate with the given ID
  void addPersonToGate(int id, int numOfPeople);

  // Allocate a person to the least busy gate
  void allocatePersonToGate();

  void HandleGateRegister(int id) override;

  void HandleGateDataBeats(int gateId, int numOfPeople) override;


  void GateChats();

private:
  // bool IsGateIdExist();

  // get the free space comparing to the total capacity
  float GetBusyRate();

  // get the number of open gate
  int GetActiveGate();

  // Open an idle gate
  int openAnIdleGate();

  // Close an idle gate
  int closeAnIdleGate();

  void CloseAllGate();
  // Additional functionalities as needed
};

#endif // GATEMANAGER_H
