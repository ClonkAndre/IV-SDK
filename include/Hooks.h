namespace plugin
{
	namespace processScriptsEvent
	{
		uintptr_t returnAddress;
		std::list<void(*)()> funcPtrs;

		void Run()
		{
			for (auto& f : funcPtrs)
			{
				f();
			}
		}
		void __declspec(naked) MainHook()
		{
			__asm
			{
				pushad
				call Run
				popad
				jmp returnAddress
			}
		}
		// CGame::Process
		void Add(void(*funcPtr)())
		{
			funcPtrs.emplace_back(funcPtr);
		}
	};

	namespace gameLoadEvent
	{
		uintptr_t returnAddress;
		std::list<void(*)()> funcPtrs;

		void Run()
		{
			for (auto& f : funcPtrs)
			{
				f();
			}
		}
		void __declspec(naked) MainHook()
		{
			__asm
			{
				pushad
				call Run
				popad
				jmp returnAddress
			}
		}
		// after the last LoadLevel call
		void Add(void(*funcPtr)())
		{
			funcPtrs.emplace_back(funcPtr);
		}
	};
	
	namespace drawingEvent
	{
		uintptr_t returnAddress;
		std::list<void(*)()> funcPtrs;

		void Run()
		{
			for (auto& f : funcPtrs)
			{
				f();
			}
		}
		void __declspec(naked) MainHook()
		{
			__asm
			{
				pushad
				call Run
				popad
				jmp returnAddress
			}
		}
		// CRenderPhasePostRenderViewport, also works in menu, runs twice per frame when in game
		void Add(void(*funcPtr)())
		{
			funcPtrs.emplace_back(funcPtr);
		}
	};

	namespace processAutomobileEvent
	{
		CVehicle* thisParam;
		uintptr_t callAddress;
		std::list<void(*)(CVehicle*)> funcPtrs;

		void Run()
		{
			for (auto& f : funcPtrs)
			{
				f(thisParam);
			}
		}
		void __declspec(naked) MainHook()
		{
			__asm
			{
				mov [thisParam], ecx
				call callAddress
				pushad
				call Run
				popad
				ret
			}
		}
		// after CAutomobile::Process, overriding steer & pedals works here
		void Add(void(*funcPtr)(CVehicle*))
		{
			funcPtrs.emplace_back(funcPtr);
		}
	}
	
	namespace processPadEvent
	{
		CPad* thisParam;
		uintptr_t callAddress;
		std::list<void(*)(CPad*)> funcPtrs;

		void Run()
		{
			for (auto& f : funcPtrs)
			{
				f(thisParam);
			}
		}
		void __declspec(naked) MainHook()
		{
			__asm
			{
				mov [thisParam], ecx
				call callAddress
				pushad
				call Run
				popad
				ret
			}
		}
		// set all pad controls here, called once per frame for each pad
		void Add(void(*funcPtr)(CPad*))
		{
			funcPtrs.emplace_back(funcPtr);
		}
	}

	namespace Overrides
	{
		void GetTexture(CSprite2d(__stdcall* funcPtr)(char*))
		{
			injector::MakeJMP(AddressSetter::Get(0x21DA10, 0xD300), funcPtr);
		}
	}
};