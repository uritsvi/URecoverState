#pragma once

#include <Windows.h>
#include <string>

class RAIIFile
{
public:
	RAIIFile();
	~RAIIFile();

	bool Create(std::string Path, bool DeleteOnClose=false);
	bool Open(_In_ std::string Path);

	//bool Map(_Out_ LPVOID& Addr);
	//bool Unmap();

	//bool IsMapped();

	DWORD GetRAIIFileSize();

	bool IsOpen();

	bool Delete();

	bool WriteStr(_In_ std::string Str);

	//LPVOID GetMappedAddr();

	HANDLE GetHandle();

	std::string GetPath();


private:
	HANDLE m_Handle;
	//LPVOID m_MappedFileAddr;

	std::string m_Path;
};

