#include <Windows.h>
#include <fileapi.h>

#include "include\RAIIDirectory.h"

RAIIDirectory::RAIIDirectory() {
}
bool RAIIDirectory::Create(
	_In_ std::string Path,
	_In_ bool DeleteOnClose) {

	m_Path = Path;
	m_DeleteOnClose = DeleteOnClose;

	return CreateDirectoryA(
		Path.c_str(),
		nullptr
	);

}
bool RAIIDirectory::Open(_In_ std::string Path) {

	m_Path = Path;
	m_DeleteOnClose = false;

	return true;
}


RAIIDirectory::~RAIIDirectory() {
	if (m_DeleteOnClose) {
		bool res = RemoveDirectoryA(m_Path.c_str());
	}
}

bool RAIIDirectory::FilesWithExtension(
	_In_ std::string Ext,
	_Out_ std::list<std::string>& Files) {
	
	bool res = true;

	std::list<std::string> allFiles;

	do {
		auto path = m_Path + "\\*";
		WIN32_FIND_DATAA data;
		auto currentFile = FindFirstFileA(path.c_str(), &data);
		if (currentFile == INVALID_HANDLE_VALUE) {
			break;
		}

		do {
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				allFiles.push_back(m_Path + "\\" + data.cFileName);
			}
		} while (FindNextFileA(currentFile, &data));


		res = FindClose(currentFile);
		if (!res) {
			break;
		}

	} while (false);

	for (auto file : allFiles) {
		auto lastIndex = file.find_last_of('.');
		auto filePath = file.substr(lastIndex);

		if (Ext == filePath) {
			Files.push_back(file);
		}
	}

	return res;
}

void RAIIDirectory::MakePath(
	_In_ std::string FilePath,
	_Out_ std::string& DirFilePath) {

	DirFilePath = m_Path + "\\" + FilePath;
}