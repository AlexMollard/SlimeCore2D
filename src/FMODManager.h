#pragma once

#include "Config.h"

#if ENABLE_FMOD
#include "fmod_studio.hpp"
#include "fmod.hpp"
#endif

class FMODManager
{
public:
	FMODManager();
	~FMODManager();

	void Update();

protected:
#if ENABLE_FMOD
	FMOD_RESULT ERR_CHECK(FMOD_RESULT func);
#endif

private:
	void* extraDriverData = nullptr;

#if ENABLE_FMOD
	FMOD::Studio::System* system = nullptr;
	FMOD::System* coreSystem = nullptr;

	FMOD::Studio::Bank* masterBank = nullptr;
	FMOD::Studio::Bank* masterStringsBank = nullptr;
	FMOD::Studio::Bank* musicBank = nullptr;

	FMOD::Studio::EventDescription* musicDesc = nullptr;
	FMOD::Studio::EventInstance* musicInst = nullptr;

	FMOD_RESULT result = FMOD_OK;
#endif
};
