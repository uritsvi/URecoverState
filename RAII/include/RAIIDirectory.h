#pragma once

#include <string>
#include <list>


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

private:
	std::string m_Path;
	bool m_DeleteOnClose;
};
