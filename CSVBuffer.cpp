#include "CSVBuffer.h"

void CSVBuffer::PushRow(std::string Row) {
	
	auto fullRow = Row + "\n";

	std::vector<std::string> vec;

	int last = 0;
	for (int i = 0; i < fullRow.size(); i++) {
		if (fullRow[i] != ',') {
			continue;
		}

		auto str = fullRow.substr(last, i - last);
		
		vec.push_back(str);

		last = i + 1;
	}

	m_Buffers.push_back(vec);
	m_Size += fullRow.length();
}

/*
* Returns the address of the buffer affter writing
*/
/*
LPVOID CSVBuffer::Serialize(LPVOID Buffer) {

	auto current = (UINT64)Buffer;
	for (auto& vec : m_Buffers) {
		for (auto& str : vec) {
			auto element = str + ",";
			auto size = element.size();

			memcpy(
				(LPVOID)current,
				element.c_str(),
				size
			);

			current += size;
		}
	}

	return (LPVOID)current;
}

LPVOID CSVBuffer::Load(LPVOID Buffer) {

	return nullptr;
}
*/

bool CSVBuffer::GetByIndex(
	ULONG Coloumn,
	ULONG Row,
	std::string& Str) {

	try {
		Str = m_Buffers[Coloumn][Row];
	}
	catch (std::exception ex) {
		return false;
	}

	return true;
	
}

ULONG64 CSVBuffer::GetSerializedSize() {
	return m_Size;
}