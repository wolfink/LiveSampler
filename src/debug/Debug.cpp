#include "../LiveSampler.h"
#include "Debug.h"

DebugVariableBroadcaster debugVariableBroadcaster;

DebugVariableBroadcaster& getDebugVariableBroadcaster()
{
	return debugVariableBroadcaster;
}