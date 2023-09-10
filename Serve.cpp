#include <pthread.h>
#include <jni.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#include <Includes/Substrate/SubstrateHook.h>
#include "Includes/KittyMemory/MemoryPatch.h"
#include "Includes/Unity/Color.hpp"
#include "Includes/Unity/Unity.h"
#include "Includes/layout.h"
#include "Includes/obfuscate.h"

#include "Logger.h"
#include "Utils.h"
#include "bools.h"
#include "hook.h"

using namespace std;

// example Fov Hack All Game

// Bool function
float getFieldOfView = 0;
float setFieldOfView = 0; // this for use in function

// All Class Camera you Can take Name in public Find in Dump
public:
	static Camera *get_main()
	{
		Camera *(*get_main_)() = (Camera *(*)())getRealOffset(0x31C63B8);
		return get_main_();
	}

	float get_fieldOfView()
	{
		float (*get_fieldOfView_)(Camera * camera) = (float (*)(Camera *))getRealOffset(0x31C3C9C);
		return get_fieldOfView_(this);
	}

	void set_fieldOfView(float value)
	{
		void (*set_fieldOfView_)(Camera * camera, float value) = (void (*)(Camera *, float))getRealOffset(0x31C3CEC);
		set_fieldOfView_(this, value);
	}
};

void (*orig_CameraSystem_LateUpdate)(void * thiz);
void hook_CameraSystem_LateUpdate(void * thiz)
{
	if (thiz != NULL)
	{
		Camera *get_main = Camera::get_main();
		if (get_main != nullptr)
		{
			if (getFieldOfView == 0)
			{
				getFieldOfView = get_main->get_fieldOfView();
			}
			if (setFieldOfView > 0 && getFieldOfView != 0)
			{
				get_main->set_fieldOfView(getFieldOfView + setFieldOfView);
			}
			else
			{
				get_main->set_fieldOfView(getFieldOfView);
			}
		}
	}
}

extern "C" {
    
	JNIEXPORT void JNICALL Java_saygus_xposedmoduleteste_LegendMods_DDDDDDDDDDDVVVVVVVVVV(JNIEnv *env, jclass thisObj, jfloat progress)
	{
		setFieldOfView = progress;
	}
	
}

void *Main(void *)
{
    ProcMap il2cppMap;
    do {
        il2cppMap = KittyMemory::getLibraryMap(libName);
        sleep(2);
    } while (!il2cppMap.isValid());
	
    // Class Player Update
	HOOK(0x18EBA24, hook_CameraSystem_LateUpdate, orig_CameraSystem_LateUpdate);

	return nullptr;
}

__attribute__((constructor))
void initializer()
{
    pthread_t ptid;
    pthread_create(&ptid, NULL, Main, NULL);
}
