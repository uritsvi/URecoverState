#include <Windows.h>
#include <fileapi.h>

#include "include\RAIIDirectory.h"

RAIIDirectory::RAIIDirectory() {
}
RAIIDirectory::~RAIIDirectory() {

}

bool RAIIDirectory::Open(_In_ std::string Path) {
	bool res = true;
	
	auto originalPath = Path;
	Path += "\\*";

	do {
		WIN32_FIND_DATAA data;
		auto currentFile = FindFirstFileA(Path.c_str(), &data);
		if (currentFile == INVALID_HANDLE_VALUE) {
			break;
		}

		do {
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				m_AllFiles.push_back(originalPath + "\\" + data.cFileName);
			}
		} while (FindNextFileA(currentFile, &data));


		res = FindClose(currentFile);
		if (!res) {
			break;
		}

	} while (false);


	return res;
}

bool RAIIDirectory::FilesWithExtension(
	_In_ std::string Ext,
	_Out_ std::list<std::string>& Files) {

	for (auto file : m_AllFiles) {
		auto lastIndex = file.find_last_of('.');
		auto filePath = file.substr(lastIndex);

		if (Ext == filePath) {
			Files.push_back(file);
		}
	}

	return true;
}