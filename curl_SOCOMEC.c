// The MIT License (MIT)
// Copyright (c) 2018 TEST
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <curl/curl.h>
#include <cJSON.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "msg_q.h"

//Global Variables
pthread_mutex_t mutex;
//#define DEBUG 1
#define DEBUG0723 1

typedef struct NV_data
{
    bool CreateDeviceFlag;
    bool NewProfileFlag;
    bool SynchronizeFlag;
    char ActivationKey[128];
    char ApiKey[128];
    char id_developer_gateway[128];
    char id_developer_ups[128];
    char GatewayChecksum[256];
    char UpsChecksum[256];
    char message[128];
    char response[128];
    char SN;
    char Type;

} NV_data;

/*For state machine*/
enum
{
    INIT_STATE = 1,
    PROCEESS_ACTIVATION_KEY,
    CREATE_DEVICE_GATEWAY,
    CREATE_DEVICE_UPS,
    PUSH_CONFIGURE,
    CHECK_PROFILE,
    PUSH_DATA,
    LIST_DEVICE_CONFIGS,
    DOWNLOAD_PROFILE,
    ToBeDebug
};
/*For make JSON content*/
enum
{
    GET_ACTIVATION_KEY_JSON = 1,
    CREATE_DEV_GATEWAY_JSON,
    CREATE_DEV_UPS_JSON,
    PUSH_CONFIGURE_JSON,
    CHECK_PROFILE_JSON,
    PUSH_DATA_JSON,
    LIST_DEV_CONFIGS_JSON,
    DOWNLOAD_PROFILE_JSON
};

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

NV_data g_NV_data; //Global NV_data
unsigned int api_status;
//char *DownloadedResponse;

//Get value form POST_RESPONSE and put it into global value
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    char *buf = (char *)malloc(size * nmemb + 1);
    char *temp = (char *)malloc(size * nmemb + 1);
    buf[size * nmemb] = '\0';
    memcpy(buf, ptr, size * nmemb);

    cJSON *cjson = cJSON_Parse(buf);
    cJSON *arrayItem_message, *arrayItem_detail, *item, *item_1; // *object

    if (cjson == NULL)
    {
        printf("JSON pack into cjson error...\n");
    }
    else
    {
#ifdef DEBUG
        printf("Response Code: \n");
        temp = cJSON_Print(cjson);
        printf(temp);
        printf("\n");
#endif

        switch (api_status)
        {
        case PROCEESS_ACTIVATION_KEY:
            printf("api_status: %s\n", "PROCEESS_ACTIVATION_KEY");
            arrayItem_message = cJSON_GetObjectItem(cjson, "message");
            if (arrayItem_message != NULL)
            {
                memcpy(g_NV_data.message, arrayItem_message->valuestring, strlen(arrayItem_message->valuestring) + 1);
                printf("Response Message: %s\n", g_NV_data.message);
            }
            arrayItem_detail = cJSON_GetObjectItem(cjson, "details");
            if (arrayItem_detail != NULL)
            {
                item = cJSON_GetObjectItem(arrayItem_detail, "ApiKey");
                if (item != NULL)
                {
#ifdef DEBUG
                    printf("ApiKey Get: %s\n", g_NV_data.ApiKey);
#endif
                    memcpy(g_NV_data.ApiKey, item->valuestring, strlen(item->valuestring) + 1);
                }
            }
            break;
        case CREATE_DEVICE_GATEWAY:
            printf("api_status: %s\n", "CREATE_DEVICE_GATEWAY");
            arrayItem_message = cJSON_GetObjectItem(cjson, "message");
            if (arrayItem_message != NULL)
            {
                memcpy(g_NV_data.message, arrayItem_message->valuestring, strlen(arrayItem_message->valuestring) + 1);
                printf("Response Message: %s\n", g_NV_data.message);
            }
            arrayItem_detail = cJSON_GetObjectItem(cjson, "details");
            if (arrayItem_detail != NULL)
            {
                item = cJSON_GetObjectItem(arrayItem_detail, "id_developer");
                if (item != NULL)
                {
                    memcpy(g_NV_data.id_developer_gateway, item->valuestring, strlen(item->valuestring) + 1);
                }
                item_1 = cJSON_GetObjectItem(arrayItem_detail, "checksum");
                if (item_1 != NULL)
                {
                    memcpy(g_NV_data.GatewayChecksum, item_1->valuestring, strlen(item_1->valuestring) + 1);
                }
            }
            break;
        case CREATE_DEVICE_UPS:
            printf("api_status: %s\n", "CREATE_DEVICE_UPS");
            arrayItem_message = cJSON_GetObjectItem(cjson, "message");
            if (arrayItem_message != NULL)
            {
                memcpy(g_NV_data.message, arrayItem_message->valuestring, strlen(arrayItem_message->valuestring) + 1);
                printf("Response Message: %s\n", g_NV_data.message);
            }
            arrayItem_detail = cJSON_GetObjectItem(cjson, "details");
            if (arrayItem_detail != NULL)
            {
                item = cJSON_GetObjectItem(arrayItem_detail, "id_developer");
                if (item != NULL)
                {
                    memcpy(g_NV_data.id_developer_ups, item->valuestring, strlen(item->valuestring) + 1); //UpsChecksum
                }
                item_1 = cJSON_GetObjectItem(arrayItem_detail, "checksum");
                if (item_1 != NULL)
                {
                    memcpy(g_NV_data.UpsChecksum, item_1->valuestring, strlen(item_1->valuestring) + 1);
                }
            }
            break;
        case PUSH_CONFIGURE:
            printf("api_status: %s\n", "PUSH_CONFIGURE");
            arrayItem_message = cJSON_GetObjectItem(cjson, "response");
            if (arrayItem_message != NULL)
            {
                memcpy(g_NV_data.response, arrayItem_message->valuestring, strlen(arrayItem_message->valuestring) + 1);
                printf("Response Message: %s\n", g_NV_data.response);
            }
            break;

        case CHECK_PROFILE:
            printf("api_status: %s\n", "CHECK_PROFILE");
            arrayItem_message = cJSON_GetObjectItem(cjson, "response");
            if (arrayItem_message != NULL)
            {
                memcpy(g_NV_data.response, arrayItem_message->valuestring, strlen(arrayItem_message->valuestring) + 1);
                printf("Response Message: %s\n", g_NV_data.response);
            }
            break;
        case PUSH_DATA:
            printf("api_status: %s\n", "PUSH_DATA");
            arrayItem_message = cJSON_GetObjectItem(cjson, "response");
            if (arrayItem_message != NULL)
            {
                memcpy(g_NV_data.response, arrayItem_message->valuestring, strlen(arrayItem_message->valuestring) + 1);
                printf("Response Message: %s\n", g_NV_data.response);
            }
            break;

            // case DOWNLOAD_PROFILE:
            //     printf("api_status: %s\n", "DOWNLOAD_PROFILE");
            //     break;
        }
    }

    //delete cjson
    cJSON_Delete(cjson);
    free(buf);
#ifdef DEBUG
    free(temp);
#endif
    return size * nmemb;
}

static size_t
write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{

    char *buf = (char *)malloc(size * nmemb + 1);
    char *temp = (char *)malloc(size * nmemb + 1);
    buf[size * nmemb] = '\0';
    memcpy(buf, ptr, size * nmemb);

    cJSON *cjson = cJSON_Parse(buf);
    cJSON *arrayItem_message, *arrayItem_detail, *item, *item_1; // *object

    if (cjson == NULL)
    {
        printf("JSON pack into cjson error...\n");
    }
    else
    {
        printf("api_status: %s\n", "LIST_DEVICE_CONFIGS");
        arrayItem_message = cJSON_GetObjectItem(cjson, "total_documents");
        if (arrayItem_message != NULL)
        {
            printf("total_documents: %s\n", arrayItem_message);
            printf("total_documents: %s\n", arrayItem_message);
            printf("total_documents: %s\n", arrayItem_message);
        }
    }

    // case LIST_DEVICE_CONFIGS:
    //     printf("api_status: %s\n", "LIST_DEVICE_CONFIGS");
    //     arrayItem_message = cJSON_GetObjectItem(cjson, "total_documents");
    //     if (arrayItem_message != NULL)
    //     {
    //         printf("total_documents: %s\n", arrayItem_message);
    //         printf("total_documents: %s\n", arrayItem_message);
    //         printf("total_documents: %s\n", arrayItem_message);
    //     }
    //     break;
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

/*Base64 encription*/
void base64(const unsigned char *input, int length, char *source)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    memcpy(source, bptr->data, bptr->length - 1);
    source[bptr->length - 1] = 0;
    BIO_free_all(b64);
}

char *creat_base64(char *source)
{
    base64((const unsigned char *)source, strlen(source), source);
    return source;
}

/*
char *trimwhitespace(char *str)
{
    int i = 0, j = 0;
    while (str[i])
    {
        if (str[i] != ' ')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';

    i = 0;
    j = 0;
    while (str[i])
    {
        if (str[i] != '\n')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';

    i = 0;
    j = 0;
    while (str[i])
    {
        if (str[i] != '\t')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';

    i = 0;
    j = 0;
    while (str[i])
    {
        if (str[i] != '\\')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
    return str;
}

*/

int CheckAPIKey()
{
    if (strlen(g_NV_data.ApiKey) == 0)
        return 0;
    else
        return 1;
}

int CheckDeviceCreate()
{
    if (g_NV_data.CreateDeviceFlag)
        return 0;
    else
        return -1;
}

char *ActivationKeyJSONObject()
{
    int i;
    char *tmp_buf = NULL;
    tmp_buf = malloc(0x400);
    sprintf(tmp_buf, "1740580713"); //Get From Web
    tmp_buf = creat_base64(tmp_buf);
    sprintf(g_NV_data.ActivationKey, "%s", tmp_buf);

    cJSON *myCommands = cJSON_CreateObject();

    char *JsonObjKey[3] = {"ActivationKey", "SN", "Type"};
    char *JsonObjValue[3] = {
        g_NV_data.ActivationKey,
        "9299_NetVisionCardSN",
        "9299",
    };

    for (i = 0; i < (sizeof(JsonObjKey) / sizeof(*JsonObjKey)); i++)
    {
        if (cJSON_AddStringToObject(myCommands, JsonObjKey[i], JsonObjValue[i]) == NULL)
        {
            printf("cJSON Error(%s)\n", JsonObjKey[i]);
        };
    }

    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_CDCommands.\n");
    }
    else
    {
#ifdef DEBUG
        printf("ActivationKeyJSONObject\n");
        printf("%s\n", Str_myCommands);
#endif
    }

    cJSON_Delete(myCommands);
    free(tmp_buf);
    return Str_myCommands;
}

char *DeviceGatewayJSONObject()
{
    int i;
    cJSON *myCommands = cJSON_CreateObject();

    char *JsonObjKey[7] = {"Type", "Name", "SN", "id_group", "time_zone", "deviceId", "ActivationKey"};
    char *JsonObjValue[7] = {
        "9299",
        "9299_NetVision_IoTGateway",
        "9299_NetVisionCardSN",
        "5ad0d3745c5d75024d21b283",
        "Europe/Paris",
        "1",
        g_NV_data.ActivationKey};

    for (i = 0; i < (sizeof(JsonObjKey) / sizeof(*JsonObjKey)); i++)
    {
        if (cJSON_AddStringToObject(myCommands, JsonObjKey[i], JsonObjValue[i]) == NULL)
        {
            printf("cJSON Error(%s)\n", JsonObjKey[i]);
        };
    }

    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_CDCommands.\n");
    }
    else
    {

#ifdef DEBUG
        printf("DeviceGatewayJSONObject\n"); //BUG: Print at the begging
        printf("%s\n", Str_myCommands);
#endif
    }

    cJSON_Delete(myCommands);
    return Str_myCommands;
}

char *DeviceUPSJSONObject()
{
    int i;
    cJSON *myCommands = cJSON_CreateObject();

    char *JsonObjKey[7] = {"Type", "Name", "SN", "id_group", "time_zone", "deviceId", "ActivationKey"};
    char *JsonObjValue[7] = {
        "9201",
        "9201_SN_DGP_NetVisionn",
        "9201_SN_DGP_NetVision_Installation",
        "5ad0d3745c5d75024d21b283",
        "Europe/Paris",
        "1",
        g_NV_data.ActivationKey};

    for (i = 0; i < (sizeof(JsonObjKey) / sizeof(*JsonObjKey)); i++)
    {
        if (cJSON_AddStringToObject(myCommands, JsonObjKey[i], JsonObjValue[i]) == NULL)
        {
            printf("cJSON Error(%s)\n", JsonObjKey[i]);
        };
    }

    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_CDCommands.\n");
    }
    else
    {

#ifdef DEBUG
        printf("%s\n", Str_myCommands);
#endif
    }

    cJSON_Delete(myCommands);
    return Str_myCommands;
}

char *PushConfigureJSONObject()
{
    int i;
    cJSON *myCommands = cJSON_CreateObject();
    cJSON *data, *items;

    cJSON_AddStringToObject(myCommands, "protocol", "v2");
    cJSON_AddStringToObject(myCommands, "device", "9201_SN_DGP_NetVision_Installation@florian.petit.netvision.netvision_site");
    cJSON_AddStringToObject(myCommands, "at", "1525427348");
    cJSON_AddItemToObject(myCommands, "data", data = cJSON_CreateObject());
    cJSON_AddStringToObject(data, "type", "configuration.item");
    cJSON_AddItemToObject(data, "items", items = cJSON_CreateObject());
    char *JsonObjKey[11] = {"index", "use", "name", "localization", "nomPowerKVA", "fluid", "nomPowerKW", "isAvailable", "type", "unitScale", "unit"};
    char *JsonObjValue[11] = {
        "0",
        "0",
        "",
        "",
        "200000",
        "0",
        "0",
        "true",
        "11",
        "0",
        "0"};

    for (i = 0; i < (sizeof(JsonObjKey) / sizeof(*JsonObjKey)); i++)
    {
        if (cJSON_AddStringToObject(items, JsonObjKey[i], JsonObjValue[i]) == NULL)
        {
            printf("cJSON Error(%s)\n", JsonObjKey[i]);
        };
    }

    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_CDCommands.\n");
    }
    else
    {
#ifdef DEBUG
        printf("%s\n", Str_myCommands);
#endif
    }

    cJSON_Delete(myCommands);
    return Str_myCommands;
}

char *ListDeviceConfigsJSONObject()
{
    cJSON *myCommands = cJSON_CreateObject();
    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_myCommands.\n");
    }
    else
    {
#ifdef DEBUG
        printf("%s\n", Str_myCommands);
#endif
    }
    cJSON_Delete(myCommands);
    return Str_myCommands;
}

char *CheckProfileJSONObject()
{
    //int i;
    cJSON *myCommands = cJSON_CreateObject();
    cJSON *data;

    cJSON_AddStringToObject(myCommands, "protocol", "v2");
    cJSON_AddStringToObject(myCommands, "device", "9201_SN_DGP_NetVision_Installation@florian.petit.netvision.netvision_site");
    cJSON_AddStringToObject(myCommands, "at", "1525427348");
    cJSON_AddItemToObject(myCommands, "data", data = cJSON_CreateObject());
    cJSON_AddStringToObject(data, "profile", "1");
    cJSON_AddStringToObject(data, "firmware", "0");

    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_CDCommands.\n");
    }
    else
    {
#ifdef DEBUG
        printf("%s\n", Str_myCommands);
#endif
    }
    cJSON_Delete(myCommands);
    return Str_myCommands;
}

char *PushDataJSONObject()
{
    int i;
    cJSON *myCommands = cJSON_CreateObject();
    cJSON *data, *items;

    char *tmp_buf = NULL;
    tmp_buf = malloc(0x400);

    char CheckSum[256] = {'\0'};
    int CheckSum_len = 0;

    cJSON_AddStringToObject(myCommands, "at", "1525427384");
    cJSON_AddItemToObject(myCommands, "data", data = cJSON_CreateObject());
    cJSON_AddStringToObject(data, "type", "Digital");
    cJSON_AddStringToObject(data, "index", "0");
    cJSON_AddItemToObject(data, "values", items = cJSON_CreateObject());
    cJSON_AddStringToObject(items, "q", "192");
    cJSON_AddStringToObject(items, "s", "20005");
    cJSON_AddStringToObject(items, "v", "0");

    //HMAC
    char *Str_myCommands = cJSON_PrintUnformatted(data);
    sprintf(tmp_buf, "1525427384%s", Str_myCommands); //Get From Web
    HMAC(EVP_sha1(), g_NV_data.UpsChecksum, strlen(g_NV_data.UpsChecksum), tmp_buf, strlen(tmp_buf), CheckSum, &CheckSum_len);
    char mdString[41] = {'\0'};
    for (i = 0; i < 20; i++)
        sprintf(&mdString[i * 2], "%02x", (int)CheckSum[i]);
    //HMAC

    cJSON_AddStringToObject(myCommands, "protocol", "v3");
    cJSON_AddStringToObject(myCommands, "device", "9201_SN_DGP_NetVision_Installation@florian.petit.netvision.netvision_site");
    cJSON_AddStringToObject(myCommands, "checksum", mdString);
    Str_myCommands = cJSON_PrintUnformatted(myCommands); //cJSON_Print
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_CDCommands.\n");
    }
    else
    {
#ifdef DEBUG
        printf("%s\n", Str_myCommands);
#endif
    }

    cJSON_Delete(myCommands);
    return Str_myCommands;
}

char *DownloadProfileJSONObject()
{
    cJSON *myCommands = cJSON_CreateObject();
    char *Str_myCommands = cJSON_Print(myCommands);
    if (Str_myCommands == NULL)
    {
        fprintf(stderr, "Failed to print Str_myCommands.\n");
    }
    else
    {
#ifdef DEBUG
        printf("%s\n", Str_myCommands);
#endif
    }
    cJSON_Delete(myCommands);
    return Str_myCommands;
}

int *myCurlExecution(char *Str_UniCurlCommands, int type)
{
    char *myStr_Commands = Str_UniCurlCommands;
    CURL *curl = NULL;
    CURLcode res;
    char headerItem[256];
    memset(headerItem, 0, 256);
    strcpy(headerItem, "Carriots.apikey: ");

    char UrlId[128];
    memset(UrlId, 0, 128);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl == NULL)
    {
        return CURLE_FAILED_INIT;
    }

    struct curl_slist *headerlist = NULL; // Format appropriate headers.
    headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

    switch (type)
    {
    case GET_ACTIVATION_KEY_JSON:
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        curl_easy_setopt(curl, CURLOPT_URL, "https://iotactivate-rect.socomec.com/api/ProcessActivationKeys");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        break;
    case CREATE_DEV_GATEWAY_JSON:
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        curl_easy_setopt(curl, CURLOPT_URL, "https://iotactivate-rect.socomec.com/api/devices");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        break;
    case CREATE_DEV_UPS_JSON:
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        curl_easy_setopt(curl, CURLOPT_URL, "https://iotactivate-rect.socomec.com/api/devices");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        break;
    case PUSH_CONFIGURE_JSON:
        strcat(headerItem, g_NV_data.ApiKey);
        headerlist = curl_slist_append(headerlist, headerItem);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.iotagora.net/streams/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        break;
    case CHECK_PROFILE_JSON:
        strcat(headerItem, g_NV_data.ApiKey);
        headerlist = curl_slist_append(headerlist, headerItem);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.iotagora.net/status");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        break;
    case LIST_DEV_CONFIGS_JSON:
        strcat(headerItem, g_NV_data.ApiKey);
        headerlist = curl_slist_append(headerlist, headerItem);
        headerlist = curl_slist_append(headerlist, "File_unset: 1");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        sprintf(UrlId, "https://api.iotagora.net/devices/%s/deviceconfigs/", g_NV_data.id_developer_gateway);
        curl_easy_setopt(curl, CURLOPT_URL, UrlId);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        break;
    case PUSH_DATA_JSON:
        strcat(headerItem, g_NV_data.ApiKey);
        headerlist = curl_slist_append(headerlist, headerItem);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, myStr_Commands);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.iotagora.net/streams/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        break;
    case DOWNLOAD_PROFILE_JSON:
        strcat(headerItem, g_NV_data.ApiKey);
        headerlist = curl_slist_append(headerlist, headerItem);
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.iotagora.net/devices/9299_NetVisionCardSN@florian.petit.netvision.netvision_site/deviceconfigs/profile.json/1/"); //Note the slash "/" at the end, must be included
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        break;
    default:
        printf("No such function");
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Switch on full protocol/debug output //
#endif

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }
    else
    {
        if (type == DOWNLOAD_PROFILE_JSON)
        {
            static const char *pagefilename = "/mnt/param/Device_Profile_JSON.txt";
            FILE *pagefile = fopen(pagefilename, "w");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); //write_data

            if (pagefile)
            {
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                {
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }
                else
                {
                    long code;
                    res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                    if (!res)
                    {
                        printf("The CONNECT response code: %03ld\n", code);
                    }
                }
                fclose(pagefile);
            }
        }
        else if (type == LIST_DEV_CONFIGS_JSON)
        {
            static const char *pagefilename = "/mnt/param/Device_Configs.txt";
            FILE *pagefile = fopen(pagefilename, "w");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); //write_data

            if (pagefile)
            {
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                {
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }
                else
                {
                    long code;
                    res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                    if (!res)
                    {
                        printf("The CONNECT response code: %03ld\n", code);
                    }
                }
                fclose(pagefile);
            }
        }
        else
        {
            long code;
            res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
            if (!res)
            {
                printf("The CONNECT response code: %03ld\n", code);
            }
        }
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headerlist);
    curl_global_cleanup();
    return 1; //success
}

int UniCurlFunction(int type)
{
    char *Str_UniCurlCommands = NULL;

    switch (type)
    {
    case GET_ACTIVATION_KEY_JSON:
        Str_UniCurlCommands = ActivationKeyJSONObject();
        break;
    case CREATE_DEV_GATEWAY_JSON:
        Str_UniCurlCommands = DeviceGatewayJSONObject();
        break;
    case CREATE_DEV_UPS_JSON:
        Str_UniCurlCommands = DeviceUPSJSONObject();
        break;
    case PUSH_CONFIGURE_JSON:
        Str_UniCurlCommands = PushConfigureJSONObject();
        break;
    case LIST_DEV_CONFIGS_JSON:
        Str_UniCurlCommands = ListDeviceConfigsJSONObject();
        break;
    case CHECK_PROFILE_JSON:
        Str_UniCurlCommands = CheckProfileJSONObject();
        break;
    case PUSH_DATA_JSON:
        Str_UniCurlCommands = PushDataJSONObject();
        break;
    case DOWNLOAD_PROFILE_JSON:
        Str_UniCurlCommands = DownloadProfileJSONObject();
        break;
    default:
        printf("No such function");
    }

    if (Str_UniCurlCommands != NULL) //Str_UniCurlCommands Error Handle
    {
        int curlExe = myCurlExecution(Str_UniCurlCommands, type);
        free(Str_UniCurlCommands);
        if (curlExe)
        {
            return 1; //success
        }
        else
        {
            printf("myCurlExecution wrong: type = %d!\n", type);
            return 0; //fail
        }
    }
    else
    {
        printf("Str_UniCurlCommands: type = %d!\n");
        return 0; //fail
    }
}

void *NVCurl_API(void *arg)
{
    pthread_detach(pthread_self());
    int UniCurl;

    while (1)
    {
        switch (api_status)
        {
        case INIT_STATE:
            api_status = INIT_STATE;
            printf("api_status = INIT_STATE\n\n\n");
            //Delete all info
            //Log
            break;
        case PROCEESS_ACTIVATION_KEY:
            UniCurl = UniCurlFunction(GET_ACTIVATION_KEY_JSON);
            if (UniCurl == 1)
            {
                if (CheckDeviceCreate() == 1) //Check device is created or not, could be a file or object
                {
                    api_status = CHECK_PROFILE; // yes
                    printf("PROCEESS_ACTIVATION_KEY Done!\n");
                    printf(">>>>>>There are some existing device\n\n\n");
                }
                else
                {
                    api_status = CREATE_DEVICE_GATEWAY;
                    printf("PROCEESS_ACTIVATION_KEY Done!\n");
                    printf(">>>>>>No device exist, Create now\n\n\n");
                }
            }
            else
            {
                //log
                printf("PROCEESS_ACTIVATION_KEY CURL failed\n");
                api_status = INIT_STATE;
            }
            break;
        case CREATE_DEVICE_GATEWAY:
            UniCurl = UniCurlFunction(CREATE_DEV_GATEWAY_JSON);
            if (UniCurl == 1)
            {
                printf("CREATE_DEVICE_GATEWAY Done!\n\n\n");
                api_status = CREATE_DEVICE_UPS;
            }
            else
            {
                printf("CREATE_DEVICE_GATEWAY CURL failed\n");
                api_status = INIT_STATE;
            }
            break;
        case CREATE_DEVICE_UPS:
            UniCurl = UniCurlFunction(CREATE_DEV_UPS_JSON);
            if (UniCurl == 1)
            {
                api_status = PUSH_CONFIGURE;
                g_NV_data.CreateDeviceFlag = true;
                printf("CREATE_DEVICE_UPS Done!\n");
                printf(">>>>>>CreateDeviceFlag set to true\n\n\n");
            }
            else
            {
                api_status = INIT_STATE;
            }
            break;
        case PUSH_CONFIGURE:
            UniCurl = UniCurlFunction(PUSH_CONFIGURE_JSON);
            if (UniCurl == 1)
            {
                printf("PUSH_CONFIGURE Done!\n\n\n");
                api_status = LIST_DEVICE_CONFIGS;
            }
            else
            {
                printf("PUSH_CONFIGURE failed!\n");
                api_status = INIT_STATE;
            }
            break;

        case LIST_DEVICE_CONFIGS:
            UniCurl = UniCurlFunction(LIST_DEV_CONFIGS_JSON); //follow profile
            if (UniCurl == 1)
            {
                printf("LIST_DEVICE_CONFIGS Done!\n\n\n");
                api_status = CHECK_PROFILE;
            }
            else
            {
                printf("LIST_DEVICE_CONFIGS failed!\n");
                api_status = INIT_STATE;
            }
            break;
        case CHECK_PROFILE:
            if (true) //check profile every 120 seconds, //120 seconds
            {
                UniCurl = UniCurlFunction(CHECK_PROFILE_JSON);
                if (UniCurl == 1)
                {
                    if (1)
                    { //if there is new file need to download;
                        printf("CHECK_PROFILE Done!\n");
                        printf(">>>>>>There is new file need to be download!\n\n\n");
                        api_status = DOWNLOAD_PROFILE;
                    }
                    else
                    {
                        printf("CHECK_PROFILE Done!\n");
                        printf(">>>>>>No updated data, try push data!\n\n\n");
                        api_status = PUSH_DATA;
                    }
                }
                else
                {
                    printf("CHECK_PROFILE failed!\n");
                    api_status = INIT_STATE;
                }
            }

            break;

        case PUSH_DATA:
            UniCurl = UniCurlFunction(PUSH_DATA_JSON); //follow profile
            if (UniCurl == 1)
            {
                printf("PUSH_DATA Done!\n\n\n");
                api_status = CHECK_PROFILE;
            }
            else
            {
                printf("PUSH_DATA failed!\n");
                api_status = INIT_STATE;
            }

            break;

        case DOWNLOAD_PROFILE:
            UniCurl = UniCurlFunction(DOWNLOAD_PROFILE_JSON); //follow profile
            if (UniCurl == 1)
            {
                printf("DOWNLOAD_PROFILE Done!\n\n\n");
                api_status = PUSH_DATA;
            }
            else
            {
                printf("DOWNLOAD_PROFILE failed!\n");
                api_status = INIT_STATE;
            }
            break;
        }
        sleep(1);
    }
}

int BugToBeDebug(void)
{
    return 0;
}

int main()
{
    pthread_t t1, t2;
    void *ret;

    g_NV_data.CreateDeviceFlag = false;
    g_NV_data.SynchronizeFlag = false;

    IOT_CLOUD_APP_MESSAGE iotMsg;
    long CmdID = 0;
    long ProcID = 0;
    char buf[32];
    int buf_len;

    // Stream Thread;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&t1, NULL, NVCurl_API, (void *)&g_NV_data); //NVCurl_API

#ifdef DEBUG0723
    CmdID = USHA_CMD_IOT_CLOUD_SYNC_REQUIRED;
#endif

    while (1)
    {
        // if (ReceiveCommand(USHA_MD_IOT_CLOUD_APP, &CmdID, &ProcID, &iotMsg, &buf_len, 0))
        // {
        //     printf("ERROR: Failure in ReceiveCommand()");
        //     continue;
        // }
        switch (CmdID)
        {
            // ************ 1. Msg_q From Web **************
        case USHA_CMD_IOT_CLOUD_SYNC_REQUIRED: //SYNC_REQUIRED
            printf("SYNC_REQUIRED\n");
            api_status = PROCEESS_ACTIVATION_KEY;
            //For Debug Usage
            CmdID = 1245;
            //For Debug Usage
            break;

            // ************ 2. Msg_q From Driver************
        case USHA_CMD_IOT_CLOUD_ALARM_PUSH_DATA:
            printf("ALARM_PUSH_DATA\n");
            break;

        case USHA_CMD_IOT_CLOUD_STATUS_PUSH_DATA:
            printf("ALARM_PUSH_DATA\n");
            break;

        case ToBeDebug:
            BugToBeDebug();
            printf("BugToBeDebug\n");
            break;

        default:
            break;
        }
        sleep(1);
    }

    return 0;
}
