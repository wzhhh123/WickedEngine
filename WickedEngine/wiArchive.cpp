#include "wiArchive.h"
#include "wiHelper.h"

// this should always be only INCREMENTED and only if a new serialization is implemeted somewhere!
uint64_t __archiveVersion = 3;
// this is the version number of which below the archive is not compatible with the current version
uint64_t __archiveVersionBarrier = 1;

// version history is logged in ArchiveVersionHistory.txt file!

wiArchive::wiArchive(const string& fileName, bool readMode):readMode(readMode),pos(0),DATA(nullptr),dataSize(0),fileName(fileName)
{
	if (!fileName.empty())
	{
		if (readMode)
		{
			ifstream file(fileName, ios::binary | ios::ate);
			if (file.is_open())
			{
				dataSize = (size_t)file.tellg();
				file.seekg(0, file.beg);
				DATA = new char[(size_t)dataSize];
				file.read(DATA, dataSize);
				file.close();
				(*this) >> version;
				if (version < __archiveVersionBarrier)
				{
					stringstream ss("");
					ss << "The archive version (" << version << ") is no longer supported!";
					wiHelper::messageBox(ss.str(), "Error!");
					Close();
				}
				if (version > __archiveVersion)
				{
					stringstream ss("");

					ss << "The archive version (" << version << ") is higher than the program's ("<<__archiveVersion<<")!";
					wiHelper::messageBox(ss.str(), "Error!");
					Close();
				}
			}
		}
		else
		{
			version = __archiveVersion;
			dataSize = 128; // this will grow if necessary anyway...
			DATA = new char[dataSize];
			(*this) << version;
		}
	}
}


wiArchive::~wiArchive()
{
	Close();
}

bool wiArchive::IsOpen()
{
	// when it is open, DATA is not null because it contains the version number at least!
	return DATA != nullptr;
}

void wiArchive::Close()
{
	if (!readMode && !fileName.empty())
	{
		SaveFile(fileName);
	}
	SAFE_DELETE_ARRAY(DATA);
}

bool wiArchive::SaveFile(const string& fileName)
{
	if (pos <= 0)
	{
		return false;
	}

	ofstream file(fileName, ios::binary | ios::trunc);
	if (file.is_open())
	{
		file.write(DATA, (streamsize)pos);
		file.close();
		return true;
	}

	return false;
}