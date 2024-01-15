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
  2. what the fuck meaning for storing information at two place
*/

class GateManager : public IGateListener, public INodeManager
{
private:
  int id; // temporary test - id should be in the customer guider
  int maxGateNum;

  int openThreshold;  // Threshold to open a new gate
  int closeThreshold; // Threshold to close an idle gate
  std::list<Gate> gates;

  GeneralGatesState generalState;
  Traffic traffic; // keep track on close/open gate logic
  hrtbt::MetaAliveTracker *gateMetaTracker;

  ISender *sender;

public:
  // Constructor
  GateManager(int maxGateNum, int openThreshold, int closeThreshold, ISender *sender);

  // Add a new gate with the given ID

  bool Add(int id) override
  {
    if (gates.size() >= maxGateNum )
    {
      // TODO:: Throw error when too many gates are registered
      return false;
    }

    auto result = std::find(gates.begin(), gates.end(), id);

    if (result != gates.end())
    {
      // throw id is already existed
      return false;
    }

    gates.push_back(Gate(id));
    return true;
  }

  bool Remove(int id) override
  {
    auto gate = std::find(gates.begin(), gates.end(), id);

    // remove if the gate exist
    if (gate != gates.end())
    {
      gates.remove(*gate);
    }
  }

  void HandleGateRegister(int id) override
  {
    // add new gate to list
    Add(id);
    // creat alive tracker for the new gate
    // FIXME - check add gate heart
    // gateMetaTracker->AddGateHeart(id);
  }

  void HandleGateDataBeats(int gateId, int numOfPeople) override
  {
    auto gate = std::find(gates.begin(), gates.end(), gateId);

    if (gate != gates.end())
    // update the gat data and newest beats
    {
      gate->refreshCount(numOfPeople);
      // FIXME - check add gate heart
      //    gateMetaTracker->AddGateBeat(gateId);
    }
    else
    {
      // handle error of unfound id
    }
  }

  // Open the gate with the given ID
  void SetGateState(int id, bool sta);

  // Add a person to the gate with the given ID
  void addPersonToGate(int id, int numOfPeople);

  // Allocate a person to the least busy gate
  void allocatePersonToGate();

  // TODO -  find a better name for me
  void GateOpenCloseLogic();

private:
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
