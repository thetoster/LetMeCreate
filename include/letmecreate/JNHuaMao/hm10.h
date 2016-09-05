/************************************************************************************************************************
 Copyright (c) 2016, The Tosters
 All rights reserved.
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/

#ifndef __LETMECREATE_HM10_H__
#define __LETMECREATE_HM10_H__

#include <stdint.h>

typedef enum {
    HM10_ROLE_MASTER,
    HM10_ROLE_SLAVE
} HM10_ROLE;

typedef enum {
    HM10_BOND_MODE_NO_PIN,
    HM10_BOND_MODE_PIN_REQUIRED
} HM10_BOND_MODE;

typedef enum {
    HM10_NOTIFY_MODE_NONE,
    HM10_NOTIFY_MODE_NOTIFY
} HM10_NOTIFY_MODE;

typedef enum {
    HM10_ADV_INTERVAL_100_MS = 0,
    HM10_ADV_INTERVAL_152_5_MS,
    HM10_ADV_INTERVAL_211_25_MS,
    HM10_ADV_INTERVAL_318_75_MS,
    HM10_ADV_INTERVAL_417_5_MS,
    HM10_ADV_INTERVAL_546_25_MS,
    HM10_ADV_INTERVAL_760_MS,
    HM10_ADV_INTERVAL_852_5_MS,
    HM10_ADV_INTERVAL_1022_5_MS,
    HM10_ADV_INTERVAL_1285_MS,
    HM10_ADV_INTERVAL_2000_MS,
    HM10_ADV_INTERVAL_3000_MS,
    HM10_ADV_INTERVAL_4000_MS,
    HM10_ADV_INTERVAL_5000_MS,
    HM10_ADV_INTERVAL_6000_MS,
    HM10_ADV_INTERVAL_7000_MS
} HM10_ADV_INTERVAL;

typedef enum {
    HM10_ADV_TYPE_ADVERTISING_SCANRESPONSE_CONNECTABLE = 0,
    HM10_ADV_TYPE_ALLOW_LAST_DEVICE_CONNECT,
    HM10_ADV_TYPE_ADVERTISING_SCANRESPONSE,
    HM10_ADV_TYPE_ADVERTISING
} HM10_ADV_TYPE;

typedef enum {
    /* When module is powered on, only respond the AT Command, don’t do anything
       until AT + START is received, or can use AT+CON,AT+CONNL */
    HM10_MODULE_WORK_TYPE_PASSIVE,

    //When power on, work immediately
    HM10_MODULE_WORK_TYPE_ACTIVE
} HM10_MODULE_WORK_TYPE;

typedef enum {
    HM10_MODULE_WORK_MODE_TRANSMISSION,
    HM10_MODULE_WORK_MODE_TRANSMISSION_PIO_COLLECTION,
    HM10_MODULE_WORK_MODE_REMOTE
} HM10_MODULE_WORK_MODE;

typedef enum {
    HM10_DISCOVERY_PARAMS_HIDE_NAME,
    HM10_DISCOVERY_PARAMS_SHOW_NAME
} HM10_DISCOVERY_PARAMS;
typedef int (*Hm10_readUart)(char *buffer, uint16_t count, uint32_t aTimeout);
typedef int (*Hm10_sendUart)(char *buffer, uint16_t count);

int hm10_init(Hm10_readUart readFunc, Hm10_sendUart sendFunc);

int hm10_self_test();
int hm10_reset();
int hm10_factory_reset();

int hm10_set_scurity_bond_mode(HM10_BOND_MODE);
int hm10_set_security_pin(char* pin);
int hm10_set_name(char* name);


int hm10_set_role(HM10_ROLE role);

int hm10_clear_last_connected();
int hm10_get_last_connected_address(char** address, uint16_t maxLen);

int hm10_set_connection_timeout(uint16_t timeoutSeconds);
int hm10_get_connection_timeout(uint16_t* timeoutSeconds);

int hm10_get_rssi(uint16_t* rssi);
int hm10_start_transmission_mode();
int hm10_set_notification_mode(HM10_NOTIFY_MODE mode);

int hm10_get_device_address(char** address, int16_t maxLen);

int hm10_set_advertising_interval(HM10_ADV_INTERVAL interval);
int hm10_get_advertising_interval(HM10_ADV_INTERVAL* interval, float* intervalInMs);

int hm10_set_advertising_type(HM10_ADV_TYPE type);
int hm10_get_advertising_type(HM10_ADV_TYPE* type);

int hm10_set_characteristic(uint16_t value);
int hm10_get_characteristic(uint16_t* value);

int hm10_set_module_work_type(HM10_MODULE_WORK_TYPE mode);
int hm10_get_module_work_type(HM10_MODULE_WORK_TYPE* mode);

int hm10_set_module_work_mode(HM10_MODULE_WORK_MODE mode);
int hm10_get_module_work_mode(HM10_MODULE_WORK_MODE* mode);

int hm10_set_discovery_parameter(HM10_DISCOVERY_PARAMS param);
int hm10_get_discovery_parameter(HM10_DISCOVERY_PARAMS* param);
#endif /* __LETMECREATE_HM10_H__ */
