#include "std_include.hpp"

namespace components
{
	std::vector<component*> loader::components_;
	utils::memory::allocator loader::mem_allocator_;

	void loader::initialize()
	{
		loader::mem_allocator_.clear();

		loader::_register(new main_module());

		// print to external console
		printf("[Modules] ---------------------\n");
		
		for (const auto& str : game::glob::loadedModules)
		{
			printf(str.c_str());
		}

		printf("\n");
	}

	void loader::uninitialize()
	{
		std::reverse(loader::components_.begin(), loader::components_.end());
		for (auto component : loader::components_)
		{
			delete component;
		}

		loader::components_.clear();
		loader::mem_allocator_.clear();
		fflush(stdout);
		fflush(stderr);
	}

	void loader::_register(component* component)
	{
		if (component)
		{
			game::glob::loadedModules.push_back("component registered: "s + component->get_name() + "\n");
			loader::components_.push_back(component);
		}
	}

	utils::memory::allocator* loader::get_alloctor()
	{
		return &loader::mem_allocator_;
	}
}
