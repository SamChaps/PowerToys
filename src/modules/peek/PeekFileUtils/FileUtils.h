#pragma once
#include <vector>

#include "common.h"

#pragma comment(lib, "shlwapi.lib")

namespace FileUtils
{
    HRESULT GetSelectedFile(std::vector<String>& selectedFiles);
};
