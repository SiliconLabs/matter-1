/*
 *
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "AppEvent.h"

#include "FreeRTOS.h"
#include "timers.h" // provides FreeRTOS timer support
#include <app-common/zap-generated/attributes/Accessors.h>

#include <lib/core/CHIPError.h>

#define TEMPERATURE_OFFSET 100  // Offset taken into account before switching thermostat states
#define TEMP_INCREMENT 10

using namespace chip;

// AppCluster Spec Table 85.
enum ThermMode
{
    OFF = 0,
    AUTO,
    NOT_USED,
    COOL,
    HEAT,
};

class TemperatureManager
{
public:
    CHIP_ERROR Init();
    void AttributeChangeHandler(EndpointId endpointId,  AttributeId attributeId, uint8_t* value, uint16_t size);
    uint8_t GetMode(void);
    int8_t GetCurrentTemp(void);
    int8_t GetHeatingSetPoint(void);
    int8_t GetCoolingSetPoint(void);

private:
    friend TemperatureManager & TempMgr(void);

    int8_t mCurrentTemp; // in Celsius
    int8_t mCoolingSetPoint; // in Celsius
    int8_t mHeatingSetPoint; // in Celsius
    uint8_t mThermMode;

    int8_t ConvertToPrintableTemp(int16_t temperature);
    static TemperatureManager sTempMgr;
};

inline TemperatureManager & TempMgr(void)
{
    return TemperatureManager::sTempMgr;
}