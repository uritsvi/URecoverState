#pragma once

#include <string>
#include <list>

struct DeleteOnReboot{
	std::list<std::string> Dirs;
	bool First;
};

class RAIIDirectory
{
public:
	RAIIDirectory();

	~RAIIDirectory();

	bool Create(
		_In_ std::string Path, 
		_In_ bool DeleteOnClose
	);
	bool Open(_In_ std::string Path);
	
	bool FilesWithExtension(
		_In_ std::string Ext, 
		_Out_ std::list<std::string>& Files
	);

	void MakePath(
		_In_ std::string FilePath,
		_Out_ std::string& DirFilePath
	);

	/*
	* This function mark all the files and sub dirs under a file in a given moment.
	* If new files or dirs are created thay will not be marked to be deleted
	*/
	bool SetDeleteOnReboot();


	bool OpenSubDir(
		_In_ std::string SubDir,
		_In_ bool FirstDir, 
		_Out_ RAIIDirectory& Dir
	);

private:
	bool InternalSetDeleteOnReboot(DeleteOnReboot& Info);


private:
	std::string m_Path;
	bool m_DeleteOnClose;
};
	