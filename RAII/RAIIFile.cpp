#include "include/RAIIFile.h"

RAIIFile::RAIIFile() {
	m_Handle = INVALID_HANDLE_VALUE;
	//m_MappedFileAddr = nullptr;
}
RAIIFile::~RAIIFile() {
	if (m_Handle != nullptr) {
		CloseHandle(m_Handle);
	}
}

bool RAIIFile::Create(std::string Path, bool DeleteOnClose) {
	
	bool res = true;

	UINT flags = 0;
	if (DeleteOnClose)  {
		flags |= FILE_FLAG_DELETE_ON_CLOSE;
	}

	do {
		m_Handle = CreateFileA(
			Path.c_str(),
			FILE_ALL_ACCESS,
			FILE_SHARE_READ,
			nullptr,
			CREATE_NEW,
			flags,
			nullptr
		);
		if (m_Handle == INVALID_HANDLE_VALUE) {
			res = false;
			break;
		}
		
		m_Path = Path;
	} while (false);

	return res;

}

bool RAIIFile::Open(std::string Path) {
	bool res = true;
	
	do {
		m_Handle = CreateFileA(
			Path.c_str(),
			FILE_ALL_ACCESS,
			0,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr
		);
		if (m_Handle == INVALID_HANDLE_VALUE) {
			res = false;
			break;
		}

		m_Path = Path;
	} while (false);

	return res;
}

/*
bool RAIIFile::Map(_Out_ LPVOID& Addr) {
	bool res = true;

	do {
		DWORD fileSize;
		res = GetRAIIFileSize(fileSize);
		if (!res) {
			break;
		}

		HANDLE map = CreateFileMappingA(
			m_Handle,
			0,
			PAGE_READWRITE | SEC_NOCACHE | SEC_COMMIT,
			0,
			fileSize,
			nullptr
		);
		if (map == INVALID_HANDLE_VALUE) {
			res = false;
			break;
		}
		
		Addr = MapViewOfFile(
			map,
			FILE_MAP_ALL_ACCESS, 
			0, 
			0, 
			fileSize
		);
		if (Addr == nullptr) {
			res = false;
			break;
		}
		m_MappedFileAddr = Addr;
	} while (false);

	return res;
}
bool RAIIFile::Unmap() {
	bool res = true;

	do {
		if (m_MappedFileAddr == nullptr) {
			break;
		}

		res = UnmapViewOfFile(m_MappedFileAddr);
		if (!res) {
			break;
		}

		m_MappedFileAddr = nullptr;
	} while (false);

	return res;
}



bool RAIIFile::IsMapped() {
	return (m_MappedFileAddr != nullptr);
}
*/

DWORD RAIIFile::GetSize() {
	DWORD size;
	GetFileSize(m_Handle, &size);

	return size;
}

bool RAIIFile::IsOpen() {
	return (m_Handle != INVALID_HANDLE_VALUE);
}

bool RAIIFile::WriteStr(_In_ std::string Str) {
	return WriteFile(
		m_Handle, 
		Str.c_str(), 
		Str.length(), 
		nullptr, 
		0
	);
}

bool RAIIFile::Delete() {
	return DeleteFileA(m_Path.c_str());
}

/*
LPVOID RAIIFile::GetMappedAddr() {
	return m_MappedFileAddr;
}
*/

HANDLE RAIIFile::GetHandle() {
	return m_Handle;
}

std::string RAIIFile::GetPath() {
	return m_Path;
}

bool RAIIFile::DeleteOnReboot() {
	return MoveFileExA(
		m_Path.c_str(), 
		nullptr, 
		MOVEFILE_DELAY_UNTIL_REBOOT
	);
}