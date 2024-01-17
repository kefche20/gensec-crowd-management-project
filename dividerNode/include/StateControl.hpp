#ifndef STATECONTROL_HPP
#define STATECONTROL_HPP

/*
   1. how to check newState is invalid 

*/
template <typename T>
class StateControl
{
    private: 
    T preState;
    T state;

    public:
    StateControl(T startState) : preState(startState), state(startState)
    {
    }

    T GetState()
    {
        return state;
    }

    bool IsNewState()
    {
        return preState != state;
    }

    void Refresh()
    {
         preState = state;
    }

    void UpdateState(T newState)
    {
        state = newState;
    }
};



#endif