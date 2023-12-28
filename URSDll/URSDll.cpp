#include "URSDll.h"
#include "..\Error.h"

bool URSDll::Init() {
	bool res = true;

	do {



		res = m_URSDllSever.Init();
		if (!res) {
			ERROR_LOG("Failed to init URSDllServer");
			break;
		}
	} while (false);

	return res;
}

bool URSDll::Shutdown() {
	return true;
}