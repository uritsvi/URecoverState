#pragma once

class RecoverState
{
public:
	static RecoverState& GetInstance();

	bool Capture();
	bool Recover();
};

