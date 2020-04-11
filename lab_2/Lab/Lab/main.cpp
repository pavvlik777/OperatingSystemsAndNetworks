#include <windows.h>
#include <string>
#include <stdio.h>
#include <initguid.h>
#include <virtdisk.h>
#include <winioctl.h>
#include <iostream>
#include <fstream>

using namespace std;

void PrintErrorMessage(ULONG ErrorId);
void GetInfo(string filename);

int main()
{
	setlocale(LC_ALL, "rus");
	string filename;
	cin >> filename;

	string input = "           ";
	int dotPos = 0;
	for (int i = 0; i < 8; i++)
	{
		if (filename[i] == '.')
		{
			dotPos = i;
			break;
		}
		else
		{
			input[i] = filename[i];
		}
	}
	for (int i = dotPos + 1; i < filename.size(); i++)
	{
		input[8 + i - dotPos - 1] = filename[i];
	}
	//string input = "SCREEN  TXT";
	GetInfo(input);

	system("pause");
	return 0;
}

void GetInfo(string filename)
{
	const unsigned int SectorSize = 512;
	BOOL result;

	HANDLE hDisk = CreateFile("\\\\.\\G:", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); //получаем указатель на диск
	if (hDisk == NULL)
	{
		PrintErrorMessage(GetLastError());
		return;
	}
	BYTE rootTable[SectorSize];
	DWORD dwRead;
	result = ReadFile(hDisk, rootTable, sizeof(rootTable), &dwRead, NULL);
	if (!result || dwRead != sizeof(rootTable))
	{
		PrintErrorMessage(GetLastError());
		return;
	}
	for (int i = 0; i < 512; i++)
	{
		char f[3];  
		_itoa(rootTable[i], f, 16);
	}

	int reservedSector = rootTable[15] * 256 + rootTable[14];
	int fatCount = rootTable[16];
	int rootEntries = rootTable[18] * 256 + rootTable[17];
	int fatSize = rootTable[23] * 256 + rootTable[22];

	int rootDirSecNum = fatCount * fatSize + reservedSector;

	SetFilePointer(hDisk, rootDirSecNum * SectorSize, NULL, FILE_BEGIN);

	BYTE rootDirBuffer[SectorSize];
	result = ReadFile(hDisk, rootDirBuffer, sizeof(rootDirBuffer), &dwRead, NULL);
	if (!result || dwRead != sizeof(rootDirBuffer))
	{
		PrintErrorMessage(GetLastError());
		return;
	}
	for (int i = 0; i < 512; i++)
	{
		char f[3];
		_itoa(rootDirBuffer[i], f, 16);
	}

	int pos = 0;
	int rootElemCount = 0;
	bool isFind = false;
	do
	{
		if (rootDirBuffer[pos] == 0 && rootDirBuffer[pos + 1] == 0)
		{
			isFind = false;
			break;
		}
		else
		{
			bool status = true;
			for (int i = 0; i < filename.size(); i++)
			{
				if (rootDirBuffer[pos + i] != filename[i])
				{
					status = false;
					break;
				}
			}
			if (status) //нашли файл
			{
				isFind = true;
				break;
			}
			else //не нашли
			{
				pos += 32;
				rootElemCount++;
				if (rootElemCount > rootEntries)
				{
					isFind = false;
					break;
				}
				if (pos >= SectorSize)
				{
					pos = 0;
					result = ReadFile(hDisk, rootDirBuffer, sizeof(rootDirBuffer), &dwRead, NULL);
					if (!result || dwRead != sizeof(rootDirBuffer))
					{
						PrintErrorMessage(GetLastError());
						return;
					}
					for (int i = 0; i < 512; i++)
					{
						char f[3];
						_itoa(rootDirBuffer[i], f, 16);
					}
				}
			}
		}
	} while (true);
	if (isFind)
	{
		int firstCluster = rootDirBuffer[pos + 0x1a] + rootDirBuffer[pos + 0x1a + 1] * 256;
		int fileSize = rootDirBuffer[pos + 0x1c] + rootDirBuffer[pos + 0x1c + 1] * 256 + rootDirBuffer[pos + 0x1c + 2] * 256 * 256 + rootDirBuffer[pos + 0x1c + 3] * 256 * 256 * 256;
		int clusterPos = fatCount * fatSize + reservedSector + rootEntries * 32 / 512 + firstCluster - 2;

		SetFilePointer(hDisk, clusterPos * SectorSize, NULL, FILE_BEGIN);
		while (fileSize > 0)
		{
			BYTE valueBuffer[SectorSize];
			result = ReadFile(hDisk, valueBuffer, sizeof(valueBuffer), &dwRead, NULL);
			if (!result || dwRead != sizeof(valueBuffer))
			{
				PrintErrorMessage(GetLastError());
				return;
			}
			for (int i = 0; i < 512; i++)
			{
				char f[3];
				_itoa(valueBuffer[i], f, 16);
			}
			BYTE output[SectorSize + 1];
			for (int i = 0; i < SectorSize; i++)
				output[i] = valueBuffer[i];
			output[SectorSize] = 0;
			cout << output;
			fileSize -= SectorSize;
		}
		cout << "\n";
	}
	else
	{
		cout << "File wasn't found\n";
	}

	CloseHandle(hDisk);
	return;
}

void PrintErrorMessage(ULONG ErrorId) //вывод сообщения об ошибке
{
	PVOID Message = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |  //записать сообщение об ошибке в Message
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ErrorId,
		0,
		(LPWSTR)&Message,
		16,
		NULL);

	wprintf(L"%s\n", Message);
	LocalFree(Message);
}