#pragma once

#include <string>

class SymbolicLink
{
public:
	bool Create(std::wstring Src, std::wstring Target);
	bool Delete();
};

