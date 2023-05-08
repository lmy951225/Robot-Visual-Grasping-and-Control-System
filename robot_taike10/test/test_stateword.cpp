#include <iostream>
using namespace std;

enum {
    STATE_NOT_READY_TO_SWITCH_ON = 1,
    STATE_SWITCH_ON_DISABLED = 2,
    STATE_READY_TO_SWITCH_ON = 3,
    STATE_SWITCH_ON = 4,
    STATE_OPERATION_ENABLED = 5,
    STATE_QUICK_STOP_ACTIVE = 6,
    STATE_FAULT_REACTION_ACTIVE = 7,
    STATE_FAULT = 8,
    STATE_UNDEFINED = -1

};

int deviceState(uint16_t status_word)
{

    if ((status_word & 0x4F) == 0x00)
    {
        return STATE_NOT_READY_TO_SWITCH_ON;
    }
    else if ((status_word & 0x4F) == 0x40)
    {
        return STATE_SWITCH_ON_DISABLED;
    }
    else if ((status_word & 0x6F) == 0x21)
    {
        return STATE_READY_TO_SWITCH_ON;
    }
    else if ((status_word & 0x6F) == 0x23)
    {
        return STATE_SWITCH_ON;
    }
    else if ((status_word & 0x6F) == 0x27)
    {
        return STATE_OPERATION_ENABLED;
    }
    else if ((status_word & 0x6F) == 0x07)
    {
        return STATE_QUICK_STOP_ACTIVE;
    }
    else if ((status_word & 0x4F) == 0x0F)
    {
        return STATE_FAULT_REACTION_ACTIVE;
    }
    else if ((status_word & 0x4F) == 0x08)
    {
        return STATE_FAULT;
    }
    return STATE_UNDEFINED;

}

int main()
{
    cout << "从站0状态:" << deviceState(uint16_t(0x6070)) << endl;
    cout << "从站1状态:" << deviceState(uint16_t(0x5237)) << endl;
    cout << "从站2状态:" << deviceState(uint16_t(0x5237)) << endl;
    cout << "从站3状态:" << deviceState(uint16_t(0x5237)) << endl;
    cout << "从站4状态:" << deviceState(uint16_t(0x5237)) << endl;
    cout << "从站5状态:" << deviceState(uint16_t(0x5237)) << endl;
    return 0;
}