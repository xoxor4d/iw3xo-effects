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

	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(components::command::command_thread), nullptr, 0, nullptr);
	
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

		utils::hook::nop(0x4F3A32, 2); // enable debug drawing
		utils::hook(0x492308, game::R_InitFonts, HOOK_CALL).install()->quick();

		// print dvar values to console
		command::register_command("getdvar"s, [](std::vector < std::string > args)
			{
				// Check if enough arguments have been passed to the command
				if (args.size() == 1)
				{
					printf("usage: getdvar <dvarName>");
					return;
				}

				std::string dvarType;
				game::dvar_s* getDvarDummy = game::Dvar_FindVar(args[1].c_str());

				//Dvars::radiant_livePort
				if (getDvarDummy)
				{
					switch (getDvarDummy->type)
					{
					case game::dvar_type::boolean:
						dvarType = "[BOOL] %s->current.enabled = %s\n";
						break;

					case game::dvar_type::value:
						dvarType = "[FLOAT] %s->current.value = %s\n";
						break;

					case game::dvar_type::vec2:
						dvarType = "[VEC2] %s->current.vector = %s\n";
						break;

					case game::dvar_type::vec3:
						dvarType = "[VEC3] %s->current.vector = %s\n";
						break;

					case game::dvar_type::vec4:
						dvarType = "[VEC4] %s->current.vector = %s\n";
						break;

					case game::dvar_type::integer:
						dvarType = "[INT] %s->current.integer = %s\n";
						break;

					case game::dvar_type::enumeration:
						dvarType = "[ENUM] %s->current.integer = " + std::to_string(getDvarDummy->current.integer) + " :: %s\n";
						break;

					case game::dvar_type::string:
						dvarType = "[STRING] %s->current.string = %s\n";
						break;

					case game::dvar_type::color:
						dvarType = "[COLOR] %s->current.vector = %s\n";
						break;

					case game::dvar_type::rgb:
						dvarType = "[RGB] %s->current.vector = %s\n";
						break;

					default:
						dvarType = "[UNKOWN] %s = %s\n";
						break;
					}

					// dvar description
					dvarType += "|-> %s\n";

					std::string dvarDescription;

					if (!getDvarDummy->description)
					{
						dvarDescription = "no description";
					}
					else
					{
						dvarDescription = getDvarDummy->description;
					}

					printf(utils::va(dvarType.c_str(), getDvarDummy->name, game::Dvar_DisplayableValue(getDvarDummy), dvarDescription.c_str()));
				}

				else
				{
					printf(utils::va("unkown dvar: \"%s\"", args[1].data()));
				}
			});

		// set dvar values via console
		command::register_command("setdvar"s, [](std::vector < std::string > args)
			{
				// Check if enough arguments have been passed to the command
				if (args.size() <= 2)
				{
					printf("usage: setdvar <dvarName> <value/s>");
					return;
				}

				game::dvar_s* getDvarDummy = game::Dvar_FindVar(args[1].c_str());

				// if dvar exists
				if (getDvarDummy)
				{

					std::string dvarValue;

					// do not append a " " if we only have 1 dvarString arg
					if (args.size() == 3)
					{
						dvarValue = args[2];
					}

					else
					{
						// combine all dvar value args
						for (auto argCount = 2; argCount < (int)args.size(); argCount++)
						{
							dvarValue += args[argCount] + " ";
						}
					}

					game::Dvar_SetFromStringByNameFromSource(args[1].c_str(), dvarValue.c_str(), 0);
					//game::Dvar_SetFromStringFromSource(dvarValue.c_str(), getDvarDummy, 0);
				}
				else
				{
					printf(utils::va("unkown dvar: \"%s\"", args[1].data()));
				}
			});
	}

	main_module::~main_module()
	{ }
}
