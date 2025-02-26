#include <windows.h>
#include <shobjidl.h> 
#include <filesystem>
#include <string>

#include "AssetManagement/AssetManager.h"
#include "GraphicsEngine/GraphicsEngine.h"
std::filesystem::path GetFolder()
{
	HRESULT hr;
	IFileDialog* pfd = nullptr;

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Set the options on the dialog.
		DWORD dwOptions;
		hr = pfd->GetOptions(&dwOptions);
		if (SUCCEEDED(hr))
		{
			hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
		}

		// Show the dialog
		hr = pfd->Show(nullptr);
		if (SUCCEEDED(hr))
		{
			// Get the result
			IShellItem* psiResult;
			hr = pfd->GetResult(&psiResult);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath = nullptr;
				hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					std::filesystem::path folderPath = pszFilePath;
					CoTaskMemFree(pszFilePath);
					psiResult->Release();
					pfd->Release();
					return folderPath;
				}
				psiResult->Release();
			}
		}
		pfd->Release();
	}
	return {};
}

int main(int argc, char*argv[])
{
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	
	std::filesystem::path source;
	std::filesystem::path target;
	bool structure = true;
	switch (argc)
	{
	case 2:
		source = argv[1];
		target = GetFolder();
		break;
	case 3:
		source = argv[1];
		target = argv[2];
		break;
	case 4:
		source = argv[1];
		target = argv[2];
		structure = (std::string(argv[3]) == "true");
		break;
	default:
		source = GetFolder();
		target = GetFolder();
		break;
	}

	AssetManager::Get().ExportFolderToBinary(source, target, structure);
	return 0;
}