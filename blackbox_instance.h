#pragma once

namespace bb
{

class BBGamePluginInstance : public nfr::api::IGamePluginInstance
{
private:
    std::atomic_long refCount;

public:
    BBGamePluginInstance()
    {
        refCount.fetch_add(1);
    }

    bool isCompatible() const override;
    bool isRunnable() const override;

    bool initialize() override;
    void destroy() override;

    bool tick(float dt) override;

    long addRef() override;
    long release() override;
};

}