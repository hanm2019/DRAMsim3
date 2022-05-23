#include "DramSimulator_DramSimulator.h"
#include "cstdio"
#include "cosimulation.h"
#include "string.h"
#include "iostream"

ComplexCoDRAMsim3 *dramsim;


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

JNIEXPORT void JNICALL Java_DramSimulator_DramSimulator_debug_1on
        (JNIEnv *, jobject){
    dramsim->debug_on();
}

JNIEXPORT void JNICALL Java_DramSimulator_DramSimulator_debug_1off
        (JNIEnv *, jobject){
    dramsim->debug_off();
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_will_1accept
        (JNIEnv *env, jobject object, jlong address, jboolean is_write){
   bool result =  dramsim->will_accept(address,is_write);
   return (jboolean)result;
}
jobject constructResponse(JNIEnv *env, jobject obj, CoDRAMTrans* response){
    jboolean is_write;
    jlong address, id;
    jint burstLength, burstType, dramBurstLength, dramBurstOffset;

    if(response == NULL){
        is_write = false;
        address = 0;
        id = 0 ;
        burstLength = 0;
        burstType = 0;
        dramBurstOffset = 0;
        dramBurstLength = 0;
    } else{
        is_write = response->is_write;
        address = response->address;
        id = response->id;
        burstLength = response->burstLength;
        burstType = response->burstType;
        dramBurstLength = response->dramBurstLength;
        dramBurstOffset = response->dramBurstOffset;
    }
    jclass thisClass = env->GetObjectClass(obj);
    jmethodID ConstructResponseId = env->GetMethodID(thisClass,"constructResponse","(JZJIIII)LDramSimulator/DramTrans;");
    jobject responseObj = env->CallObjectMethod(obj,ConstructResponseId,
                                                address,is_write,id,burstLength,burstType,dramBurstLength, dramBurstOffset);

    return responseObj;
}

JNIEXPORT jobject JNICALL Java_DramSimulator_DramSimulator_get_1write_1response
        (JNIEnv * env, jobject obj){
    CoDRAMTrans* response = dramsim->get_write_response();
    return (jobject) constructResponse(env, obj, response);
}

JNIEXPORT jobject JNICALL Java_DramSimulator_DramSimulator_get_1read_1response
        (JNIEnv * env, jobject obj){
    CoDRAMTrans* response = dramsim->get_read_response();
    return (jobject) constructResponse(env, obj, response);
}


JNIEXPORT jlong JNICALL Java_DramSimulator_DramSimulator_get_1clock_1ticks
        (JNIEnv *, jobject){
    return (jlong)(dramsim->get_clock_ticks());
}


JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_check_1write_1response
        (JNIEnv *env, jobject obj){
    return (jboolean) dramsim->check_write_response();
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_check_1read_1response
        (JNIEnv *env, jobject obj){
    return (jboolean) dramsim->check_read_response();
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_add_1request
        (JNIEnv *env, jobject obj, jobject req){

    jclass requestClass = env->GetObjectClass(req);


    jfieldID addressFieldId = env->GetFieldID(requestClass,"address","J");
    jlong address = env->GetLongField(req,addressFieldId);

    jfieldID is_writeFieldId = env->GetFieldID(requestClass,"is_write","Z");
    jboolean is_write = env->GetBooleanField(req,is_writeFieldId);

    jfieldID idFieldId = env->GetFieldID(requestClass,"id","J");
    jlong id = env->GetLongField(req,idFieldId);

    jfieldID burstLengthFieldId = env->GetFieldID(requestClass,"burstLength","I");
    jint burstLength = env->GetIntField(req,burstLengthFieldId);

    jfieldID burstTypeFieldId = env->GetFieldID(requestClass,"burstType","I");
    jint burstType = env->GetIntField(req,burstTypeFieldId);

    jfieldID dramBurstOffsetFieldId = env->GetFieldID(requestClass,"dramBurstOffset","I");
    jint dramBurstOffset = env->GetIntField(req,dramBurstOffsetFieldId);

    jfieldID dramBurstLengthFieldId = env->GetFieldID(requestClass,"dramBurstLength","I");
    jint dramBurstLength = env->GetIntField(req,dramBurstLengthFieldId);


    CoDRAMTrans* DramRequest = new CoDRAMTrans(address,is_write,id,burstLength,burstType,dramBurstLength,dramBurstOffset);

    return (jboolean) dramsim->add_request(DramRequest);

}