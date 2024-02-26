#pragma once

#include "URSDLLServer.h"

class URSDll
{
public:
	bool Init();
	bool Shutdown();

private:
	URSDLLServer m_URSDllSever;
};

