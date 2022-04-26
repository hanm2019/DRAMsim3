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






//    jclass responseClass = env->FindClass("LDramSimulator/Response");
//    jmethodID RespConstructMethodId = env->GetMethodID(responseClass,"<init>","(ZLDramSimulator/Request;JJJ)V");
//    printf("get ResponseClass and MethodId\n");
//    jclass requestClass = env->FindClass("LDramSimulator/Request");
//    jmethodID ReqConstructMethodId = env->GetMethodID(responseClass,"<init>","(ZJZIIII[B)V");
//    printf("get RequestClass and MethodId\n");

//    if(response != NULL){
//        jlong req_time = response->req_time;
//        jlong resp_time = response->resp_time;
//        jlong finish_time = response->finish_time;
//        jboolean valid = true;
//
//        jboolean req_valid = true;
//        jlong req_address = response->req->address;
//        jboolean req_is_write = response->req->is_write;
//
//        struct dramsim3_meta* meta = (struct dramsim3_meta*) response->req->meta;
//
//        jint meta_len = 0;
//        jint meta_size = 0;
//        jint meta_offset = 0;
//        jint meta_id = 0;
//        jbyteArray meta_data = env->NewByteArray(64);
//
//        if(meta != NULL){
//            meta_len = meta->len;
//            meta_size = meta->size;
//            meta_offset = meta->offset;
//            meta_id = meta->id;
//
//            uint64_t* data = meta->data;
//            jbyte data_byte[64];
//            memcpy(data_byte,data,64);
//            env->SetByteArrayRegion(meta_data,0,64,data_byte);
//        }
//        jobject reqObj = env->NewObject(requestClass, ReqConstructMethodId, req_valid,req_address,req_is_write,
//                                        meta_len,meta_size,meta_offset,meta_id,meta_data);
//        printf("create reqObj\n");
//        jobject respObj = env->NewObject(responseClass,RespConstructMethodId,valid, reqObj, req_time, resp_time, finish_time);
//        printf("create respObj\n");
//        return  respObj;
//    } else{
//        jobject reqObj = env->NewObject(requestClass, ReqConstructMethodId, false, 0, false,
//                                        0,0,0,0,NULL);
//        printf("create reqObj\n");
//
//        jobject respObj = env->NewObject(responseClass,RespConstructMethodId,false, reqObj, 0, 0, 0);
//        printf("create respObj\n");
//        return respObj;
//    }
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