#include "DramSimulator_DramSimulator.h"
#include "cstdio"
#include "cosimulation.h"
#include "string.h"
#include "iostream"

ComplexCoDRAMsim3 *dramsim;

struct dramsim3_meta{
    uint8_t len;
    uint8_t size;
    uint8_t offset;
    uint8_t id;
    uint64_t data[8];
};

JNIEXPORT void JNICALL Java_DramSimulator_DramSimulator_init
(JNIEnv *env , jobject obj, jstring configFile, jstring outputDir){
    const char *configFileName = env->GetStringUTFChars(configFile, NULL);
    const char *outputDIrName = env->GetStringUTFChars(outputDir,NULL);


    dramsim = new ComplexCoDRAMsim3(configFileName,outputDIrName);
    env->ReleaseStringUTFChars(configFile,configFileName);
    env->ReleaseStringUTFChars(outputDir,outputDIrName);
}

JNIEXPORT void JNICALL Java_DramSimulator_DramSimulator_delete
        (JNIEnv *env, jobject obj){
    delete dramsim;
}

JNIEXPORT void JNICALL Java_DramSimulator_DramSimulator_tick
        (JNIEnv *, jobject){
    dramsim->tick();
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_will_1accept
        (JNIEnv *env, jobject object, jlong address, jboolean is_write){
   bool result =  dramsim->will_accept(address,is_write);
   return (jboolean)result;
}


JNIEXPORT jlong JNICALL Java_DramSimulator_DramSimulator_get_1clock_1ticks
        (JNIEnv *, jobject){
    return (jlong)(dramsim->get_clock_ticks());
}

jobject constructResponse(JNIEnv *env, jobject obj, CoDRAMResponse* response){
    jboolean req_valid,resp_valid,is_write;
    jlong address, req_time, resp_time, finish_time;
    jint len,size,offset,id;
    jbyte data[64];
    jbyteArray metaData = env->NewByteArray(64);

    if(response == NULL){
        req_valid = false;
        resp_valid = false;
        is_write = false;
        address = 0;
        req_time = 0;
        resp_time = 0;
        finish_time = 0;
        len = 0 ;
        size = 0 ;
        offset = 0 ;
        id = 0 ;
        memset(data,0,64);
    } else{
        resp_valid = true;
        req_time = response->req_time;
        resp_time = response->resp_time;
        finish_time = response->finish_time;
        const CoDRAMRequest* request = response->req;
        if(request == NULL){
            req_valid = false;
            address = 0;
            is_write = 0;
            len = 0;
            size = 0;
            offset = 0;
            id = 0;
            memset(data, 0, 64);
        }else{
            req_valid = true;
            address = request->address;
            is_write = request->is_write;
            struct dramsim3_meta* meta = (struct dramsim3_meta*) request->meta;
            if(meta == NULL){
                len = 0;
                size = 0;
                offset = 0;
                id = 0;
                memset(data, 0, 64);
            }else{
                len = meta->len;
                size = meta->size;
                offset = meta->offset;
                id = meta->id;
                memcpy(data,meta->data,64);
            }
        }
    }
    env->SetByteArrayRegion(metaData,0,64,data);
    jclass thisClass = env->GetObjectClass(obj);
    jmethodID ConstructResponseId = env->GetMethodID(thisClass,"constructResponse","(ZZJZIIII[BJJJ)LDramSimulator/DramResponse;");
    jobject responseObj = env->CallObjectMethod(obj,ConstructResponseId,
                                                req_valid,resp_valid,
                                                address, is_write,
                                                len,size,offset,id,metaData,
                                                req_time,resp_time,finish_time);

    return responseObj;
}
JNIEXPORT jobject JNICALL Java_DramSimulator_DramSimulator_check_1write_1response
        (JNIEnv *env, jobject obj){
    CoDRAMResponse* response = dramsim->check_write_response();
    return constructResponse(env,obj,response);
}


JNIEXPORT jobject JNICALL Java_DramSimulator_DramSimulator_check_1read_1response
        (JNIEnv *env, jobject obj){
    CoDRAMResponse* response = dramsim->check_read_response();
    return constructResponse(env,obj,response);
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_add_1request
        (JNIEnv *env, jobject obj, jobject req){

    jclass requestClass = env->GetObjectClass(req);

    jfieldID validFieldId = env->GetFieldID(requestClass,"valid","Z");
    jboolean valid = env->GetBooleanField(req,validFieldId);

    jfieldID addressFieldId = env->GetFieldID(requestClass,"address","J");
    jlong address = env->GetLongField(req,addressFieldId);

    jfieldID is_writeFieldId = env->GetFieldID(requestClass,"is_write","Z");
    jboolean is_write = env->GetBooleanField(req,is_writeFieldId);

    jfieldID lenFieldId = env->GetFieldID(requestClass,"len","I");
    jint len = env->GetIntField(req,lenFieldId);

    jfieldID sizeFieldId = env->GetFieldID(requestClass,"size","I");
    jint size = env->GetIntField(req,sizeFieldId);

    jfieldID offsetFieldId = env->GetFieldID(requestClass,"offset","I");
    jint offset = env->GetIntField(req,offsetFieldId);

    jfieldID idFieldId = env->GetFieldID(requestClass,"id","I");
    jint id = env->GetIntField(req,idFieldId);

    CoDRAMRequest* DramRequest = new CoDRAMRequest(address,is_write);

    return (jboolean) dramsim->add_request(DramRequest);

}