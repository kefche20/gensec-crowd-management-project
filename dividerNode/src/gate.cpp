#include <string>
class Gate
{
private:
  std::string id;
  bool isOpen;
  int peopleCount;

public:
  Gate(const std::string &id) : isOpen(false), peopleCount(0) {}

  const std::string &getId() const
  {
    return id;
  }

  void open()
  {
    isOpen = true;
  }

  void close()
  {
    isOpen = false;
  }

  bool isOpened() 
  {
    return isOpen;
  }

  void addPerson()
  {
    if (isOpen)
    {
      ++peopleCount;
    }
    // TODO:: Handle case when gate is closed
    // TODO:: Handle case when gate is at maximum

  }

  void removePerson()
  {
    if (peopleCount > 0)
    {
      --peopleCount;
    }
    // Handle underflow or closed gate, if needed
  }

  int getLineCount()
  {
    return peopleCount;
  }

};