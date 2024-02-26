#include <Windows.h>
#include <fileapi.h>
#include <shlwapi.h>
#include <pathcch.h>

#include "include\RAIIDirectory.h"
#include "include\RAIIFile.h"


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "pathcch.lib")


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

	if (!PathFileExistsA(Path.c_str())) {
		return false;
	}

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

bool RAIIDirectory::InternalSetDeleteOnReboot(_In_ DeleteOnReboot& Info) {
	bool res = true;
	bool first = Info.First;

	if (first) {
		Info.First = false;
	}


	do {
		Info.Dirs.push_back(m_Path);

		auto path = m_Path + "\\*";
		WIN32_FIND_DATAA data;
		auto currentFile = FindFirstFileA(path.c_str(), &data);
		if (currentFile == INVALID_HANDLE_VALUE) {
			break;
		}

		do {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (strcmp(data.cFileName, ".") == 0 ||
					strcmp(data.cFileName, "..") == 0) {

					continue;
				}
				RAIIDirectory subDir;
				res = OpenSubDir(
					data.cFileName,
					false,
					subDir
				);
				if (!res) {
					res = false;
					break;
				}
				res = subDir.InternalSetDeleteOnReboot(Info);
				if (!res) {
					res = false;
					break;
				}
				continue;
			}
			std::string filePath;
			MakePath(
				data.cFileName, 
				filePath
			);
			RAIIFile file;
			res = file.Open(filePath);
			if (!res) {
				break;
			}
			res = file.DeleteOnReboot();
			if (!res) {
				break;
			}
			
		} while (FindNextFileA(currentFile, &data));
		if (!res) {
			break;
		}
		res = FindClose(currentFile);
		if (!res) {
			break;
		}

		if (first) {
			for (auto current = Info.Dirs.end(); current != Info.Dirs.begin();) {
				current--;
				
				res = MoveFileExA(
					current->c_str(),
					nullptr, 
					MOVEFILE_DELAY_UNTIL_REBOOT
				);
				if (!res) {
					break;
				}
			}
		}

	} while (false);

	return res;
}

bool RAIIDirectory::SetDeleteOnReboot() {
	DeleteOnReboot info;
	info.First = true;

	return InternalSetDeleteOnReboot(info);
}

bool RAIIDirectory::OpenSubDir(
	_In_ std::string SubDir,
	_In_ bool FirstDir,
	_Out_ RAIIDirectory& Dir) {
	
	bool res = true;

	do {
			auto path = m_Path + "\\*";
			WIN32_FIND_DATAA data;
			auto currentFile = FindFirstFileA(path.c_str(), &data);
			if (currentFile == INVALID_HANDLE_VALUE) {
				break;
			}

			bool found = false;
			do {
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (strcmp(data.cFileName, ".") == 0 || 
						strcmp(data.cFileName, "..") == 0) {
						
						continue;
					}
					if (FirstDir || strcmp(data.cFileName, SubDir.c_str()) == 0) {
						std::string fullPath = m_Path + "\\" + data.cFileName;
						res = Dir.Open(fullPath);
						if (!res) {
							break;
						}
						found = true;
						break;
					}
				}
			} while (FindNextFileA(currentFile, &data));
			if (!res) {
				break;
			}

			res = FindClose(currentFile);
			if (!res) {
				break;
			}

			res = found;

		} while (false);

	return res;
}
