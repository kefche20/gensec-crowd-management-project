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
#include "heartbeat.hpp"

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

enum GeneralGatesState
{
  ALL_IN_DUTY,
  PARTY_DUTY,
  ONE_IN_DUTY,
  ALL_FREE
};

enum TrafficState
{
  VOID_T,
  IDLE_T,
  NORMAL,
  CROWD,
  UNOCCUPIED // different word use
};

// TODO - check how to prevent from using
struct Traffic
{
public:
  TrafficState state;
  TrafficState preState;

  Traffic() : state(IDLE_T), preState(VOID_T)
  {
  }

  bool IsNewState()
  {
    return state != preState;
  }

  void UpdateState(TrafficState newState)
  {
    state = newState;
  }

  void ClearEntryFlag()
  {
    preState = state;
  }
};

/*
  //TODO:
  1. check neccessary of letting AddNode and RemoveNode as interface

*/

class GateManager : public IGateListener, public INodeManager, public IDataCollector
{
private:
  int maxGateNum;

  int openThresholdRate;  // Threshold to open a new gate
  int closeThresholdRate; // Threshold to close an idle gate
  std::list<Gate> gates;

  GeneralGatesState generalState;
  Traffic traffic; // keep track on close/open gate logic
  hrtbt::MetaAliveTracker metaAliveTracker;

  ISender *sender;

public:
  // Constructor
  GateManager(int maxGateNum, int openThreshold, int closeThreshold);

  void SetSender(ISender *sender);

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
  float GetFreeSpaceRate();

  // get the number of open gate
  int GetActiveGate();

  // Open an idle gate
  int openAnIdleGate();

  // Close an idle gate
  int closeAnIdleGate();
  // Additional functionalities as needed
};

#endif // GATEMANAGER_H
