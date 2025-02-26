#pragma once
#include <fstream>
#include <iostream>
#include <filesystem>
//Created by Nils.M for use at WormWorks
namespace Serialization
{
	const static char* Version = "WWA1";
	const static std::string format = ".rock";
}


class Serializer
{
public:
	enum Mode
	{
		Read,
		Write
	};
	Serializer() = default;
	virtual ~Serializer() = default;

	void Open(const std::filesystem::path& aPath, bool aMode)
	{
		if (myStream.is_open())
		{
			Flush();
		}
		//myStream.open(aPath, std::fstream::out | std::fstream::in |std::ios::binary |std::ios::trunc);
		myMode = aMode;
		if(aMode)
		{
			myStream.open(aPath, std::ios::out |std::ios::binary |std::ios::trunc);
			if(!myStream.is_open())
			{
				std::cout << "Failed to open file!\n";
				return;
			}
			myStream.write(Serialization::Version, 4);
		}
		else
		{
			myStream.open(aPath, std::ios::in |std::ios::binary);
			if(!myStream.is_open())
			{
				std::cout << "Failed to open file!\n";
				return;
			}
			char* vers = new char[5];
			//myStream.open(aPath, std::fstream::in |std::ios::binary |std::ios::trunc);
			myStream.read(vers, 4);
			vers[4] = '\0';
			if(std::strcmp(vers, Serialization::Version)!= 0)
			{
				std::cout << "FileType or Version Mismatch\nCurrent version: " << Serialization::Version << "\nFileVersion: " << vers <<"\n";
			}
		}
	}
	void Close()
	{
		myStream.close();
	}
	void Flush()
	{
		myStream.flush();
		myStream.close();
	};
	void operator<<(size_t& aValue)
	{
		Serialize(aValue);
	}
	template<class T>
	void operator<<(T& aValue)
	{
		Serialize(aValue);
	}
	template<class T>
	void Serialize(T& aValue)
	{
		SerializeByteCode((void*) & aValue, sizeof(T));
	}
	/*void operator<<(const std::string& aString, size_t aSize)
	{
		SerializeByteCode((void*)(aString.c_str()), aSize);
	}*/
	void Serialize(const std::string& aString, size_t aSize)
	{
		SerializeByteCode((void*)(aString.c_str()), aSize);
	}
	virtual void SerializeByteCode(void* aData, const size_t& aSize)
	{
		if(myStream.is_open())
		{
			if(myMode)
			{
				myStream.write(reinterpret_cast<char*>(aData), aSize);
			}
			else
			{
				myStream.read(reinterpret_cast<char*>(aData), aSize);
			}
		}
		else
		{
			std::cout << "File not Open";
		}
	}
private:
	std::fstream myStream;
	bool myMode;
};
