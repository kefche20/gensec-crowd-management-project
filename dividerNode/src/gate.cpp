#include "gate.h"
Gate::Gate(std::string id) : id(std::move(id)), isOpen(false), peopleCount(0)
{
}
std::string Gate::getId()
{
  return id;
}

void Gate::open()
{
  isOpen = true;
}

void Gate::close()
{
  isOpen = false;
}

bool Gate::isOpened()
{
  return isOpen;
}

void Gate::addPerson()
{
  if (isOpen)
  {
    ++peopleCount;
  }
  // TODO: Handle case when gate is closed
  // TODO: Handle case when gate is at maximum
}

void Gate::refreshCount(int count)
{
  peopleCount = count;
}

int Gate::getLineCount()
{
  return peopleCount;
}
