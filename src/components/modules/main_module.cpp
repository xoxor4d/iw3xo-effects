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

void com_printf_to_console([[maybe_unused]] int channel, const char* _format, ...)
{
	va_list _arglist;
	char text_out[1024];

	__crt_va_start(_arglist, _format);
	vsprintf(text_out, _format, _arglist);
	_vfprintf_l(stdout, _format, NULL, _arglist);
	__crt_va_end(_arglist);
}

namespace components
{
	main_module::main_module()
	{
		init_threads();
		utils::hook::detour(0x402600, com_printf_to_console, HK_JUMP);

		// ------------------

		utils::hook::detour(0x487880, fx_system::FX_BeginLooping, HK_JUMP);
		utils::hook::detour(0x4753C0, fx_system::FX_KillEffect, HK_JUMP);
		utils::hook::detour(0x4740E0, fx_system::FX_SpawnEffect, HK_JUMP);
		utils::hook::detour(0x487050, fx_system::FX_UpdateElement, HK_JUMP);
		utils::hook::detour(0x4E64E0, fx_system::Sys_DoWorkerCmd, HK_JUMP);
		utils::hook::detour(0x475A70, fx_system::FX_RetriggerEffect, HK_JUMP);
		utils::hook::detour(0x47F840, fx_system::FX_Convert, HK_JUMP);

		utils::hook::detour(0x4778B0, fx_system::FX_Register, HK_JUMP);

		// works but keeping it active doesn't make sense
		//utils::hook::detour(0x403C80, fx_system::FX_SaveEditorEffect, HK_JUMP);

		utils::hook(0x404DB8, physics::Phys_Init, HOOK_CALL).install()->quick();
		utils::hook(0x4A2EC8, fx_system::FX_RunPhysics, HOOK_CALL).install()->quick();
	}

	main_module::~main_module()
	{ }
}
