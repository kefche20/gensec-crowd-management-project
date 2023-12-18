#ifndef MESSAGECONTENT_HPP
#define MESSAGECONTENT_HPP

enum DividerBoardcastMessage
{
   DISCOVER,     //0
   NEW_LEADER,   //1
   NEW_MEMBER,   //2
   LEADER_ALIVE, //3
   LEADER_DEAD,  //4
};

enum DividerDirectMessage
{
  FELLOW_MEMBER,
  FELLOW_LEADER,
  FELLOW_NEUTRAL
};

enum GateMessage
{
   OPENGATE,
   CLOSEGATE,
   NUMOFPEOPLE,
   EMERGENCY,
   ALLOC,
   REGISTER,
   SCANNED
  
};



#endif 
