#include "test.h"

void test_main()
{
    int object_id = 1;

    while(1)
    {
        sleep(5);
        attribute_set(object_id, AttNum_SmokeState, AlaState_Normal);
        attribute_set(object_id, AttNum_BatteryAlert, AlaState_Normal);
        attribute_set(object_id, AttNum_TestInProgress, 0);
        attribute_set(object_id, AttNum_HardwareFaultAlert, 0);
        attribute_set(object_id, AttNum_EndOfServiceAlert, Eos_Normal);
        attribute_set(object_id, AttNum_ContaminationState, CtmState_Normal);
        attribute_set(object_id, AttNum_SmokeSensitivityLevel, Slv_Standard);

        sleep(5);
        attribute_set(object_id, AttNum_SmokeState, AlaState_Warning);
        attribute_set(object_id, AttNum_BatteryAlert, AlaState_Warning);
        attribute_set(object_id, AttNum_TestInProgress, 1);
        attribute_set(object_id, AttNum_HardwareFaultAlert, 1);
        attribute_set(object_id, AttNum_EndOfServiceAlert, Eos_Expired);
        attribute_set(object_id, AttNum_ContaminationState, CtmState_Low);
        attribute_set(object_id, AttNum_SmokeSensitivityLevel, Slv_Low);
    }
}