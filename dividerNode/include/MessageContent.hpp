#ifndef MESSAGECONTENT_HPP
#define MESSAGECONTENT_HPP

#define BOARDCAST_ID 000

#define ID_LENGTH 3
#define MSG_LENGTH 1
#define DATA_LENGTH 2

#define SRC_ID_POS 1
#define DES_ID_POS 5
#define MSG_POS 9
#define DATA_POS 11

enum Topic
{
   UI,
   DIVIDER_ROLE,
   DIVIDER_ALIVE,
   GATE,
};

enum CONTENT_TYPE
{
   SRC_ID,
   DES_ID,
   MSG,
   DATA1,
   DATA2
};

enum DividerBoardcastMessage
{
   DISCOVER,
   NEW_LEADER,
   NEW_MEMBER,
   LEADER_ALIVE,
   LEADER_DEAD,
   MEMBER_ALIVE,
};

enum DividerDirectMessage
{
   FELLOW_MEMBER,
   FELLOW_LEADER,
   FELLOW_NEUTRAL,
   ACTIVATE,
   DEACTIVATE
};

enum GateMessage
{
   ACK = 0,
   OPENGATE = 1,
   CLOSEGATE = 2,
   DATA = 3, // REVIEW - chaning message name from NUMOFPEOPLE to DATA
   EMERGENCY = 4,
   ALLOC = 5,
   REGISTER = 6,
   SCANNED = 7,

};

enum UIMessage
{
   CHECK_IN,
   // ACK,
};

#endif
