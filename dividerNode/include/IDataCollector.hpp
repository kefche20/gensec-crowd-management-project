#ifndef IDATACOLLECTOR_HPP
#define IDATACOLLECTOR_HPP


#include <utility>
 #include "DividerDataProcessor.hpp"

//interface of getting the data from the local gate managment
class IDataCollector
{
    public:
    virtual std::pair<int,int> GetLeastBusyGate() = 0;   //return the information of gate id and number of people

};

class IRemoteDataCollector : public IDataCollector
{
   public: 
   virtual RoleMode GetRoleMode() = 0;
};

#endif