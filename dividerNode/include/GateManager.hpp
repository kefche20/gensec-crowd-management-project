#ifndef GATEMANAGER_H
#define GATEMANAGER_H



#include <iostream>
#include <algorithm>

#include <map>
#include <list>
#include <queue>
#include <string>

#include "heartbeat.hpp"
#include "INodeShifter.hpp"
#include "Gate.hpp"
#include "config.hpp"

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



class GateManager : INodeShifter
{
private:
  int maxGateNum;

  int openThreshold;  // Threshold to open a new gate
  int closeThreshold; // Threshold to close an idle gate

  GeneralGatesState generalState;
  Traffic traffic; // keep track on close/open gate logic
   
  std::list<Gate> gates;

public:
  // Constructor
  GateManager(int maxGateNum, int openThreshold, int closeThreshold);

  // Add a new gate with the given ID

  void AddNode(int id) override
  {
    if (gates.size() >= maxGateNum)
    {
      // TODO:: Throw error when too many gates are registered
      return;
    }

    auto result = std::find(gates.begin(), gates.end(), id);

    if (result != gates.end())
    {
      // throw id is already existed
      return;
    }

    gates.push_back(Gate(id));
  }

  void RemoveNode(int id) override
  {
     auto gate = std::find(gates.begin(),gates.end(),id);
     
     //remove if the gate exist
     if(gate != gates.end())
     {
      gates.remove(*gate);
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

  void HandleGateALive()
  {
  }

private:
  //get the free space comparing to the total capacity
  float GetFreeSpaceRate();
   
  //get the number of open gate
  int GetActiveGate();

  // Open an idle gate
  int openAnIdleGate();

  // Close an idle gate
  int closeAnIdleGate();
  // Additional functionalities as needed
};

#endif // GATEMANAGER_H
