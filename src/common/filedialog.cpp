#include "common/filedialog.h"
#include <windows.h>
#include <shobjidl.h> 
#include <filesystem>

std::wstring open_file_dialog() {
    return open_file_dialog(std::filesystem::current_path());
}

std::wstring open_file_dialog(const std::filesystem::path& default_folder) {
    std::wstring result;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog *pFileOpen;
        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
        if (SUCCEEDED(hr)) {
            LPCWSTR szTXT = L"Text Files";
            LPCWSTR szAll = L"All Files";
            COMDLG_FILTERSPEC rgSpec[] = {
                { szTXT, L"*.txt" },
                { szAll, L"*.*" },
            };
            hr = pFileOpen->SetFileTypes(2, rgSpec);
            if (SUCCEEDED(hr)) {
                IShellItem* pCurFolder = nullptr;
                std::string absolutePath = std::filesystem::absolute(default_folder).string();
                std::wstring stemp = std::wstring(absolutePath.begin(), absolutePath.end());
                PCWSTR sw = stemp.c_str();
                hr = SHCreateItemFromParsingName(sw, NULL, IID_PPV_ARGS(&pCurFolder));
                if (SUCCEEDED(hr)) {
                    hr = pFileOpen->SetFolder(pCurFolder);
                    if (SUCCEEDED(hr)) {
                        // Show the Open dialog box.
                        hr = pFileOpen->Show(NULL);
                        // Get the file name from the dialog box.
                        if (SUCCEEDED(hr)) {
                            IShellItem* pItem;
                            hr = pFileOpen->GetResult(&pItem);
                            if (SUCCEEDED(hr)) {
                                PWSTR pszFilePath;
                                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                // Display the file name to the user.
                                if (SUCCEEDED(hr)) {
                                    result = std::wstring(pszFilePath);
                                    CoTaskMemFree(pszFilePath);
                                }
                                pItem->Release();
                            }
                        }
                        pFileOpen->Release();
                    }
                }
            }
        }
        CoUninitialize();
    }
    return result;
}

std::wstring save_file_dialog() {
    return save_file_dialog(std::filesystem::current_path());
}

std::wstring save_file_dialog(const std::filesystem::path& default_folder) {
    std::wstring result;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileSaveDialog *pFileSave;
        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, 
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
        if (SUCCEEDED(hr)) {
            LPCWSTR szTXT = L"Text Files";
            LPCWSTR szAll = L"All Files";
            COMDLG_FILTERSPEC rgSpec[] = {
                { szTXT, L"*.txt" },
                { szAll, L"*.*" },
            };
            hr = pFileSave->SetFileTypes(2, rgSpec);
            if (SUCCEEDED(hr)) {
                hr = pFileSave->SetDefaultExtension(L"txt");
                if (SUCCEEDED(hr)) {
                    IShellItem* pCurFolder = nullptr;
                    std::filesystem::path currentFolder = std::filesystem::current_path();
                    std::string absolutePath = std::filesystem::absolute(currentFolder).string();
                    std::wstring stemp = std::wstring(absolutePath.begin(), absolutePath.end());
                    PCWSTR sw = stemp.c_str();
                    hr = SHCreateItemFromParsingName(sw, NULL, IID_PPV_ARGS(&pCurFolder));
                    if (SUCCEEDED(hr)) {
                        hr = pFileSave->SetFolder(pCurFolder);
                        if (SUCCEEDED(hr)) {
                            // Show the Open dialog box.
                            hr = pFileSave->Show(NULL);
                            // Get the file name from the dialog box.
                            if (SUCCEEDED(hr)) {
                                IShellItem* pItem;
                                hr = pFileSave->GetResult(&pItem);
                                if (SUCCEEDED(hr)) {
                                    PWSTR pszFilePath;
                                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                    // Display the file name to the user.
                                    if (SUCCEEDED(hr)) {
                                        result = std::wstring(pszFilePath);
                                        CoTaskMemFree(pszFilePath);
                                    }
                                    pItem->Release();
                                }
                            }
                            pFileSave->Release();
                        }
                    }
                }
            }
        }
        CoUninitialize();
    }
    return result;
}
