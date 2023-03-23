#pragma once

class DebugVariableBroadcaster : public ChangeBroadcaster
{
	float _var;
public:
	DebugVariableBroadcaster() : _var(0.0) {}
	void setVariable(float var)
	{
		_var = var;
		sendChangeMessage();
	}
	float getVariable() { return _var; }
};

extern DebugVariableBroadcaster& getDebugVariableBroadcaster();

#include "DebugPanel.h"