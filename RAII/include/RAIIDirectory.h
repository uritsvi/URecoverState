#include <string>
#include <list>

class RAIIDirectory
{
public:
	RAIIDirectory();
	~RAIIDirectory();

	bool Open(_In_ std::string Path);
	
	bool FilesWithExtension(
		_In_ std::string Ext, 
		_Out_ std::list<std::string>& Files
	);

private:
	std::list<std::string> m_AllFiles;
};
