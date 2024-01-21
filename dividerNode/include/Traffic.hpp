#ifndef TRAFFIC_HPP
#define TRAFFIC_HPP


enum GeneralDutyState
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
#endif