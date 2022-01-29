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
	//void __declspec(naked) asd()
	//{
	//	const static uint32_t retn_pt = 0x4870B5;
	//	__asm
	//	{
	//		// elemOrigin ------------- ! pushed
	//		// elemSequence ------------- ! pushed
	//		// elemMsecBegin ------------- ! pushed
	//		// elemAtRestFraction ------------- ! pushed 
	//		// elemDefIndex ------------- ! pushed
	//		// effect ------------- ! pushed
	//		// msecUpdateBegin (ebx)
	//		// msecUpdateEnd (ecx)
	//		// update (eax)

	//		//mov     eax, [ebp - 94h];

	//		push	ebx;
	//		push	ecx;
	//		push	eax;
	//		call	fx_system::FX_UpdateElement_SetupUpdate;
	//		add		esp, 12;
	//		jmp		retn_pt; // jump back to add esp 0x18
	//	}
	//}

	//void __declspec(naked) asd2()
	//{
	//	const static uint32_t retn_pt = 0x4870C0;
	//	const static uint32_t retn_pt_jz = 0x4871E9;
	//	__asm
	//	{
	//		// elemOrigin ------------- ! pushed
	//		// elemSequence ------------- ! pushed
	//		// elemMsecBegin ------------- ! pushed
	//		// elemAtRestFraction ------------- ! pushed 
	//		// elemDefIndex ------------- ! pushed
	//		// effect ------------- ! pushed
	//		// msecUpdateBegin (ebx)
	//		// msecUpdateEnd (ecx)
	//		// update (eax)

	//		//mov     eax, [ebp - 94h];

	//		push	ebx;
	//		push	ecx;
	//		push	eax;
	//		call	fx_system::FX_UpdateElement_SetupUpdate;
	//		add		esp, 12;

	//		add     esp, 0x18; // stock
	//		test    al, al;
	//		jz      loc_4871E9;
	//		jmp		retn_pt; // jump back to lea

	//		loc_4871E9:
	//		jmp		retn_pt_jz;
	//	}
	//}

	//utils::hook(0x4870B0, asd, HOOK_JUMP).install()->quick();
	//utils::hook(0x486FA1, asd2, HOOK_JUMP).install()->quick();


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


		//utils::hook(0x475A00, fx_system::FX_ElemFromHandleTest, HOOK_CALL).install()->quick();

		

		

	}

	main_module::~main_module()
	{ }
}
