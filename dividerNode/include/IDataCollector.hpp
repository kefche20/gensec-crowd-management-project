#ifndef IDATACOLLECTOR_HPP
#define IDATACOLLECTOR_HPP


#include <utility>
#include "DividerDataProcessor.hpp"

//interface of getting the data from the local gate management
class IDataCollector
{
    public:
    virtual std::pair<int,int> GetLeastBusyGate() = 0;   //return the pair value of the gate ID and the least busy rate

};

class IRemoteDataCollector : public IDataCollector
{
   public: 
   virtual RoleMode GetRoleMode() = 0;
};

#endif