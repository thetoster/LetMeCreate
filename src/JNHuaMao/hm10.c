/************************************************************************************************************************
 Copyright (c) 2016, The Tosters.
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

#include "hm10.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static Hm10_readUart uartRead = NULL;
static Hm10_sendUart uartWrite = NULL;

int hm10_send_cmd(char* cmd, char* expResp) {
    uartWrite(cmd, strlen(cmd));
    char response[30];
    memset(response, 0, 30);
    uartRead(response, strlen(expResp), 1000);
    printf("hm10: %s --> %s\n", cmd, response);
    return strcasecmp(expResp, response) == 0 ? 0 : -1;
}

int hm10_send_query(char* cmd, char* respPrefix, char** outStr, uint16_t strMaxLen, int* outInt) {
    uartWrite(cmd, strlen(cmd));
    char response[40];
    memset(response, 0, 40);
    int readLen = uartRead(response, 40 , 1000);
    if (readLen < 0) {
        return -1;
    }
    char* data = response + strlen(respPrefix);
    if (outStr != NULL) {
        uint16_t len = strlen(data);
        strncpy(*outStr, data, len > strMaxLen ? strMaxLen : len);
    }
    if (outInt != NULL) {
        *outInt = atoi(data);
    }
    return readLen;
}

int hm10_init(Hm10_readUart readFunc, Hm10_sendUart sendFunc) {
    uartRead = readFunc;
    uartWrite = sendFunc;

    return hm10_self_test();
}

int hm10_self_test() {
    return hm10_send_cmd("AT", "OK");
}

int hm10_set_scurity_bond_mode(HM10_BOND_MODE mode) {
    if (mode == HM10_BOND_MODE_NO_PIN) {
        return hm10_send_cmd("AT+TYPE0", "OK+SET:0");

    } else if (mode == HM10_BOND_MODE_PIN_REQUIRED) {
        return hm10_send_cmd("AT+TYPE1", "OK+SET:1");
    }
    printf("hm10: unknown bond mode.\n");
    return -1;
}

int hm10_set_security_pin(char* pin) {

    char buff[30], resp[30];
    sprintf(buff, "AT+PASS%s", pin);
    sprintf(resp, "OK+SET:%s", pin);
    return hm10_send_cmd(buff, resp);
}

int hm10_set_name(char* name) {
    if (strlen(name) > 12) {
        printf("hm10: name can be max 12 chars length!\n");
        return -1;
    }
    char buff[30], resp[30];
    sprintf(buff, "AT+NAME%s", name);
    sprintf(resp, "OK+SET:%s", name);
    return hm10_send_cmd(buff, resp);
}

int hm10_reset() {
    return hm10_send_cmd("AT+RESET", "OK+RESET");
}

int hm10_factory_reset() {
    return hm10_send_cmd("AT+RENEW", "OK+RENEW");
}

int hm10_set_role(HM10_ROLE role) {
    if (role == HM10_ROLE_MASTER) {
        return hm10_send_cmd("AT+ROLE1", "OK+SET:1");

    } else if (role == HM10_ROLE_SLAVE) {
        return hm10_send_cmd("AT+ROLE0", "OK+SET:0");
    }

    printf("hm10: unknown role.\n");
    return -1;
}

int hm10_clear_last_connected() {
    return hm10_send_cmd("AT+CLEAR", "OK+CLEAR");
}

int hm10_get_device_address(char** address, int16_t maxLen) {
    return hm10_send_query("AT+ADDR?", "OK+ADDR:", address, maxLen, NULL);
}

int hm10_set_advertising_interval(HM10_ADV_INTERVAL interval) {
    if (interval > HM10_ADV_INTERVAL_7000_MS) {
        printf("hm10: wrong interval.\n");
        return -1;
    }
    char* val = "0123456789ABCDEF";
    char cmd[20];
    char resp[20];
    sprintf(cmd, "AT+ADVI%c", val[interval]);
    sprintf(resp, "OK+SET:%c", val[interval]);
    return hm10_send_cmd(cmd, resp);
}

int hm10_get_advertising_interval(HM10_ADV_INTERVAL* interval, float* intervalInMs) {
    char str[1];
    char* pstr = str;
    int resp = hm10_send_query("AT+ADVI?", "OK+GET:", &pstr, 1, NULL);
    if (resp < 0) {
        return -1;
    }
    int index = strtol(&str[0], NULL, 16);
    const float inter[] = {
            100.0f,
            152.5f,
            211.25f,
            318.75f,
            417.5f,
            546.25f,
            760.0f,
            852.5f,
            1022.5f,
            1285.0f,
            2000.0f,
            3000.0f,
            4000.0f,
            5000.0f,
            6000.0f,
            7000.0f
    };
    HM10_ADV_INTERVAL intEnu[] = {
        HM10_ADV_INTERVAL_100_MS,
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
    };
    if (interval != NULL) {
        *interval = intEnu[index];
    }
    if (intervalInMs != NULL) {
        *intervalInMs = inter[index];
    }
    return 0;
}

int hm10_set_advertising_type(HM10_ADV_TYPE type) {
    char buff[30], resp[30];
    sprintf(buff, "AT+ADTY%d", type);
    sprintf(resp, "OK+SET:%d", type);
    return hm10_send_cmd(buff, resp);
}

int hm10_get_advertising_type(HM10_ADV_TYPE* type) {
    int i;
    int r= hm10_send_query("AT+ADTY?", "OK+GET:", NULL, 0, &i);
    *type = (HM10_ADV_TYPE)i;
    return r;
}

int hm10_get_last_connected_address(char** address, uint16_t maxLen) {
    //return hm10_send_query("")
    //todo: napisac
    return -1;
}

int hm10_set_module_work_type(HM10_MODULE_WORK_TYPE mode) {
    char buff[30], resp[30];
    sprintf(buff, "AT+IMME%d", mode);
    sprintf(resp, "OK+SET:%d", mode);
    return hm10_send_cmd(buff, resp);
}

int hm10_get_module_work_type(HM10_MODULE_WORK_TYPE* mode) {
    int i;
    int r = hm10_send_query("AT+IMME?", "OK+GET:", NULL, 0, &i);
    *mode = (HM10_MODULE_WORK_TYPE)i;
    return r;
}

int hm10_set_characteristic(uint16_t value) {
    char buff[30], resp[30];
    sprintf(buff, "AT+CHAR0x%X", value);
    sprintf(resp, "OK+SET:0x%X", value);
    return hm10_send_cmd(buff, resp);
}

int hm10_get_characteristic(uint16_t* value) {
    int i;
    int r = hm10_send_query("AT+CHAR?", "OK+GET:0x", NULL, 0, &i);
    *value = (uint16_t)i;
    return r;
}

int hm10_set_connection_timeout(uint16_t timeoutSeconds) {
    char buff[30], resp[30];
    sprintf(buff, "AT+TCON%d", timeoutSeconds);
    sprintf(resp, "OK+SET:%d", timeoutSeconds);
    return hm10_send_cmd(buff, resp);
}

int hm10_get_connection_timeout(uint16_t* timeoutSeconds) {
    //todo: napisac
    return -1;
}

int hm10_get_rssi(uint16_t* rssi) {
    char* cmd = "AT+RSSI";
    if (uartWrite(cmd, strlen(cmd)) < 0) {
        return -1;
    }
    char rec[30];
    memset(rec, 0, 30);
    if (uartRead(rec, strlen(cmd) + 3, 1000) < 0) {
        return -1;
    }
    char* param = rec + strlen(cmd) + 1;
    return atoi(param);
}

int hm10_start_transmission_mode() {
    return hm10_send_cmd("AT+START", "OK+START");
}

int hm10_set_notification_mode(HM10_NOTIFY_MODE mode) {
    if (mode == HM10_NOTIFY_MODE_NONE) {
        return hm10_send_cmd("AT+NOTI0", "OK+SET:0");

    } else if (mode == HM10_NOTIFY_MODE_NOTIFY) {
        return hm10_send_cmd("AT+NOTI1", "OK+SET:1");

    }
    printf("hm10: unknown notification mode.\n");
    return -1;
}

int hm10_set_module_work_mode(HM10_MODULE_WORK_MODE mode) {
    char buff[30], resp[30];
    sprintf(buff, "AT+MODE%d", mode);
    sprintf(resp, "OK+SET:%d", mode);
    return hm10_send_cmd(buff, resp);
}

int hm10_get_module_work_mode(HM10_MODULE_WORK_MODE* mode) {
    int i;
    int r = hm10_send_query("AT+MODE?", "OK+GET:", NULL, 0, &i);
    *mode = (uint16_t) i;
    return r;
}

int hm10_set_discovery_parameter(HM10_DISCOVERY_PARAMS param)  {
    char buff[30], resp[30];
    sprintf(buff, "AT+SHOW%d", param);
    sprintf(resp, "OK+SET:%d", param);
    return hm10_send_cmd(buff, resp);
}

int hm10_get_discovery_parameter(HM10_DISCOVERY_PARAMS* param)  {
    int i;
    int r = hm10_send_query("AT+SHOW?", "OK+GET:", NULL, 0, &i);
    *param = (uint16_t) i;
    return r;
}

