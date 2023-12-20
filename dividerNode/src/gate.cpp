#include "Gate.hpp"
Gate::Gate(int id) : id(id), isOpen(false), peopleCount(0)
{
}

bool Gate::operator==(int id)
{
  return this->id == id;
}

bool Gate::operator==(const Gate& gate)
{
  return this->id == gate.id;
}

int Gate::GetId()
{
  return id;
}

bool Gate::GetOpenSta()
{
  return isOpen;
}

int Gate::GetMaxCapacity()
{
  return maxCapacity;
}

void Gate::SetOpenSta(bool sta)
{
  isOpen = sta;
}

void Gate::addPerson(int numOfPeople)
{
  if (isOpen)
  {
    peopleCount += numOfPeople;
  }

  // TODO Handle case when gate is closed
  // TODO Handle case when gate is at maximum
}

void Gate::refreshCount(int count)
{
  peopleCount = count;
}

int Gate::getLineCount()
{
  return peopleCount;
}
