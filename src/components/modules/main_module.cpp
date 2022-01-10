#include "std_include.hpp"


BOOL init_threads()
{
	// -----------
	// I/O Console

	// Disable STDOUT buffering
	setvbuf(stdout, nullptr, _IONBF, 0);

	// Create an external console for Radiant
	if (AllocConsole())
	{
		FILE *file = nullptr;
		freopen_s(&file, "CONIN$", "r", stdin);
		freopen_s(&file, "CONOUT$", "w", stdout);
		freopen_s(&file, "CONOUT$", "w", stderr);

		SetConsoleTitleA("IW3Effects Console");
	}
	
	return TRUE;
}




namespace components
{
	main_module::main_module()
	{
		init_threads();

		//utils::hook(0x475A00, fx_system::FX_ElemFromHandleTest, HOOK_CALL).install()->quick();

		utils::hook::detour(0x473300, fx_system::FX_ElemToHandle, HK_JUMP);

	}

	main_module::~main_module()
	{ }
}
