/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

/**********************************************************
 * Includes
 *********************************************************/

#include "TemperatureManager.h"
#include "AppConfig.h"
#include "AppEvent.h"
#include "AppTask.h"
#include "semphr.h"

/**********************************************************
 * Defines and Constants
 *********************************************************/

#define TEMP_TIMER_ms 3000
#define ENDPOINT_ID 1

using namespace chip;
using namespace ::chip::DeviceLayer;
namespace ThermAttr = chip::app::Clusters::Thermostat::Attributes;
/**********************************************************
 * Variable declarations
 *********************************************************/


TemperatureManager TemperatureManager::sTempMgr;

CHIP_ERROR TemperatureManager::Init()
{
    app::DataModel::Nullable<int16_t> temp;
    int16_t heatingSetpoint, coolingSetpoint;
    uint8_t systemMode;

    PlatformMgr().LockChipStack();
    ThermAttr::LocalTemperature::Get(ENDPOINT_ID, temp);
    ThermAttr::OccupiedCoolingSetpoint::Get(ENDPOINT_ID, &coolingSetpoint);
    ThermAttr::OccupiedHeatingSetpoint::Get(ENDPOINT_ID, &heatingSetpoint);
    ThermAttr::SystemMode::Get(ENDPOINT_ID, &systemMode);
    PlatformMgr().UnlockChipStack();

    mCurrentTemp     = ConvertToPrintableTemp(temp.Value());
    mHeatingSetPoint = ConvertToPrintableTemp(coolingSetpoint);
    mCoolingSetPoint = ConvertToPrintableTemp(heatingSetpoint);
    mThermMode = systemMode;

    AppTask::GetAppTask().UpdateThermoStatUI();

    return CHIP_NO_ERROR;
}

int8_t TemperatureManager::ConvertToPrintableTemp(int16_t temperature)
{
    constexpr uint8_t kRoundUpValue = 50;
    
    //Round up the temperute as we won't print decimals on LCD
    // Is it a negative temperate
    if (temperature < 0)
    {
        temperature -= kRoundUpValue;
    }
    else
    {
        temperature += kRoundUpValue;
    }

    return static_cast<int8_t>(temperature / 100);
}

void TemperatureManager::AttributeChangeHandler(EndpointId endpointId,  AttributeId attributeId, uint8_t* value, uint16_t size)
{
    switch(attributeId)
    {
        case ThermAttr::LocalTemperature::Id:
        {
            int8_t Temp = ConvertToPrintableTemp(*((int16_t*)value));
            EFR32_LOG("Local temp %d", Temp);
            mCurrentTemp = Temp;
        }
        break;

        case ThermAttr::OccupiedCoolingSetpoint::Id:
        {
            int8_t coolingTemp = ConvertToPrintableTemp(*((int16_t*)value));
            EFR32_LOG("CoolingSetpoint %d", coolingTemp);
            mCoolingSetPoint = coolingTemp;
        }
        break;

        case ThermAttr::OccupiedHeatingSetpoint::Id:
        {
            int8_t heatingTemp = ConvertToPrintableTemp(*((int16_t*)value));
            EFR32_LOG("HeatingSetpoint %d", heatingTemp);
            mHeatingSetPoint = heatingTemp;
        }
        break;

        case ThermAttr::SystemMode::Id:
        {
            EFR32_LOG("SystemMode %d", static_cast<uint8_t>(*value));
            uint8_t mode = static_cast<uint8_t>(*value);
            if (mThermMode != mode)
            {
                mThermMode = mode;
            }
        }
        break;

        default:
        {
            EFR32_LOG("Unhandled thermostat attribute %x", attributeId);
            return;
        }
        break;
    }

    AppTask::GetAppTask().UpdateThermoStatUI();
}

uint8_t TemperatureManager::GetMode(void)
{
    return mThermMode;
}

int8_t TemperatureManager::GetCurrentTemp(void)
{
    return mCurrentTemp;
}
int8_t TemperatureManager::GetHeatingSetPoint(void)
{
    return mHeatingSetPoint;
}

int8_t TemperatureManager::GetCoolingSetPoint(void)
{
    return mCoolingSetPoint;
}