#pragma once

#include "fmod_studio.hpp"
#include "fmod.hpp"

class FMODManager
{
public:
	FMODManager();
	~FMODManager();

	void Update();

protected:
	FMOD_RESULT ERR_CHECK(FMOD_RESULT func);

private:
	void* extraDriverData = nullptr;
	FMOD::Studio::System* system = nullptr;
	FMOD::System* coreSystem = nullptr;

	FMOD::Studio::Bank* masterBank = nullptr;
	FMOD::Studio::Bank* masterStringsBank = nullptr;
	FMOD::Studio::Bank* musicBank = nullptr;

	FMOD::Studio::EventDescription* musicDesc = nullptr;
	FMOD::Studio::EventInstance* musicInst = nullptr;

	FMOD_RESULT result = FMOD_OK;
};
