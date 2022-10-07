#pragma once
#include "GfxConfiguration.h"

class ApplicationInterface
{
public:
    virtual ~ApplicationInterface() = default;

private:
    virtual bool Initialize(GfxConfiguration* cfg) = 0;
    virtual void Shutdown() = 0;

    virtual void Tick() = 0;

};

