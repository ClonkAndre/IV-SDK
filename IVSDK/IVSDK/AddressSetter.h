namespace AddressSetter
{
	uint32_t gBaseAddress;
	bool bAddressesRead = false;

	uint32_t GetVersionFromEXE()
	{
		TCHAR szFileName[MAX_PATH];

		GetModuleFileName(NULL, szFileName, MAX_PATH);

		DWORD  verHandle = 0;
		UINT   size = 0;
		LPBYTE lpBuffer = NULL;
		DWORD  verSize = GetFileVersionInfoSize(szFileName, &verHandle);

		if (verSize != NULL)
		{
			LPSTR verData = new char[verSize];

			if (GetFileVersionInfo(szFileName, verHandle, verSize, verData))
			{
				if (VerQueryValue(verData, TEXT("\\"), (VOID FAR * FAR*) & lpBuffer, &size))
				{
					if (size)
					{
						VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
						if (verInfo->dwSignature == 0xfeef04bd)
						{
							std::string str = std::to_string((verInfo->dwFileVersionMS >> 16) & 0xffff);
							str += std::to_string((verInfo->dwFileVersionMS >> 0) & 0xffff);
							str += std::to_string((verInfo->dwFileVersionLS >> 16) & 0xffff);
							str += std::to_string((verInfo->dwFileVersionLS >> 0) & 0xffff);
							delete[] verData;
							return std::stoi(str);
						}
					}
				}
			}
			delete[] verData;
		}
		return 0;
	}

	// 1040 is too different to support in this project
	void DetermineVersion()
	{
		switch (GetVersionFromEXE())
		{
		case 1070:
			plugin::gameVer = plugin::VERSION_1070;
			break;
		case 1080:
			plugin::gameVer = plugin::VERSION_1080;
			break;
		default:
			plugin::gameVer = plugin::VERSION_NONE;
			break;
		}
	}

	bool Init()
	{
		if (bAddressesRead)
			return true;

		DebugLog(L"[IV-SDK] [AddressSetter::Init] Starting to determine the game version now.\r\n");

		// Determine the version of the game
		DetermineVersion();

		if (plugin::gameVer == plugin::VERSION_NONE)
		{
			DebugLog(L"[IV-SDK] [AddressSetter::Init] Could not determine game version.\r\n");
			return false;
		}
		else
		{
			switch (plugin::gameVer)
			{
				case plugin::VERSION_1070: DebugLog(L"[IV-SDK] [AddressSetter::Init] Detected version: 1070.\r\n"); break;
				case plugin::VERSION_1080: DebugLog(L"[IV-SDK] [AddressSetter::Init] Detected version: 1080.\r\n"); break;
			}
		}

		// Get the base address
		gBaseAddress = (uint32_t)GetModuleHandle(NULL);

		bAddressesRead = true;

		return true;
	}

	template<typename T> T& GetRefPattern(const std::string_view& pattern, size_t index, ptrdiff_t offset)
	{
		//if (!bAddressesRead)
		//	Init();

		auto scan = hook::pattern(pattern);
		uint32_t* ptr = scan.get(index).get<uint32_t>(offset);

		assert(!ptr);

		return *reinterpret_cast<T*>(ptr);
	}

	// note that the base address is added here and 0x400000 is not subtracted, so rebase your .idb to 0x0 or subtract it yourself
	template<typename T> T& GetRef(uint32_t addr1070, uint32_t addr1080)
	{
		if (!bAddressesRead)
			Init();

		switch (plugin::gameVer)
		{
			case plugin::VERSION_1070:
				return *reinterpret_cast<T*>(gBaseAddress + addr1070);
			case plugin::VERSION_1080:
				return *reinterpret_cast<T*>(gBaseAddress + addr1080);
		}

		return *reinterpret_cast<T*>(nullptr);
	}
	template<typename T> T& GetRef(uint32_t addr)
	{
		if (!bAddressesRead)
			Init();

		return *reinterpret_cast<T*>(gBaseAddress + addr);
	}

	// Gets the reference from an absolut address where offset is already added on the base address
	template<typename T> T& GetRefAbs(uint32_t addr)
	{
		if (!bAddressesRead)
			Init();

		return *reinterpret_cast<T*>(addr);
	}

	uint32_t Get(uint32_t addr1070, uint32_t addr1080)
	{
		if (!bAddressesRead)
			Init();

		switch (plugin::gameVer)
		{
			case plugin::VERSION_1070:
				return gBaseAddress + addr1070;
			case plugin::VERSION_1080:
				return gBaseAddress + addr1080;
		}

		return 0;
	}
	uint32_t Get(uint32_t addr)
	{
		if (!bAddressesRead)
			Init();

		return gBaseAddress + addr;
	}

}