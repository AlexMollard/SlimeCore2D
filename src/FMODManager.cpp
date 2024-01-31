#include "FMODManager.h"

FMODManager::FMODManager()
{
	result = FMOD::Studio::System::create(&system);
	result = system->getCoreSystem(&coreSystem);
	result = coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);

	result = system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);

	result = system->loadBankFile("C:\\Users\\Connor Young\\Documents\\FMOD Studio\\Example\\Build\\Desktop\\Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

	result = system->loadBankFile("C:\\Users\\Connor Young\\Documents\\FMOD Studio\\Example\\Build\\Desktop\\Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

	result = system->loadBankFile("C:\\Users\\Connor Young\\Documents\\FMOD Studio\\Example\\Build\\Desktop\\Music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicBank);

	result = system->getEvent("event:/Henry Mancini - Baby Elephant Walk", &musicDesc);

	result = musicDesc->createInstance(&musicInst);

	result = musicInst->start();
	result = musicInst->release();

	if (musicInst->isValid())
	{
		int i = 1;
		i++;
	}
	else
	{
		int i = 2;
		i++;

	}
}

FMODManager::~FMODManager()
{
	musicBank->unload();
	masterStringsBank->unload();
	masterBank->unload();

	system->release();
}

void FMODManager::Update()
{
	if (system->isValid())
		system->update();
}
