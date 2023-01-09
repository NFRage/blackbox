#include "blackbox.h"

BLACKBOX_PLUGIN_API nfr::api::IEngineFactory* EngineFactory = nullptr;

class BBPluginFactory : public nfr::api::IPluginFactory
{
private:

public:
	nfr::api::EFactoryType getType() const
	{
		return nfr::api::EFactoryType::GameFactory;
	}

	std::uint32_t getId() const
	{
		return 'BBBB';
	}

	std::uint32_t getVersion() const
	{
		return 1;
	}

	nfr::api::IPluginInstance* createInstance(void* options)
	{
		return new bb::BBGamePluginInstance();
	}
};

BBPluginFactory BBFactory;

extern "C"
{
	
nfr::api::IPluginFactory* CreatePluginFactory(nfr::api::IEngineFactory* engineFactory)
{
	EngineFactory = engineFactory;
	return &BBFactory;
}

}