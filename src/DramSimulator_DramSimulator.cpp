#include "DramSimulator_DramSimulator.h"
#include "cstdio"
#include "cosimulation.h"
#include "string.h"
#include "iostream"

ComplexCoDRAMsim3 *dramsim;


JNIEXPORT void JNICALL Java_DramSimulator_DramSimulator_init
(JNIEnv *env , jobject obj, jstring configFile, jstring outputDir, jint channelNum){
    const char *configFileName = env->GetStringUTFChars(configFile, NULL);
    const char *outputDIrName = env->GetStringUTFChars(outputDir,NULL);

    dramsim = new ComplexCoDRAMsim3(configFileName,outputDIrName,channelNum);

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
    jboolean is_write, last, enable, writeback;
    jlong address, id;
    jint length, uid;


    if(response == NULL){
        is_write = false;
        last = false;
        enable = false;
        writeback = false;
        address = 0;
        id = 0 ;
        length = 0;
        uid = 0;
    } else{
        is_write = response->is_write;
        address = response->address;
        id = response->id;
        last = response->last;
        enable = response->enable;
        writeback = response->writeback;
        length = response->length;
        uid = response->uid;
        delete response;
    }
    jclass thisClass = env->GetObjectClass(obj);
    jmethodID ConstructResponseId = env->GetMethodID(thisClass,"constructResponse","(JZIIIZZZ)LDramSimulator/DramTrans;");//TODO
    jobject responseObj = env->CallObjectMethod(obj,ConstructResponseId,
                                                address,is_write,id,uid,length, last, enable, writeback);

    return responseObj;
}

JNIEXPORT jobject JNICALL Java_DramSimulator_DramSimulator_get_1write_1response
        (JNIEnv * env, jobject obj, jint id){
    CoDRAMTrans* response = dramsim->get_write_response((int)id);
    return (jobject) constructResponse(env, obj, response);
}

JNIEXPORT jobject JNICALL Java_DramSimulator_DramSimulator_get_1read_1response
        (JNIEnv * env, jobject obj,jint id){
    CoDRAMTrans* response = dramsim->get_read_response((int)id);
    return (jobject) constructResponse(env, obj, response);
}


JNIEXPORT jlong JNICALL Java_DramSimulator_DramSimulator_get_1clock_1ticks
        (JNIEnv *, jobject){
    return (jlong)(dramsim->get_clock_ticks());
}


JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_check_1write_1response
        (JNIEnv *env, jobject obj, jint id){
    return (jboolean) dramsim->check_write_response((int)id);
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_check_1read_1response
        (JNIEnv *env, jobject obj, jint id){
    return (jboolean) dramsim->check_read_response((int)id);
}

JNIEXPORT jboolean JNICALL Java_DramSimulator_DramSimulator_add_1request
        (JNIEnv *env, jobject obj, jobject req){

    jclass requestClass = env->GetObjectClass(req);


    jfieldID addressFieldId = env->GetFieldID(requestClass,"address","J");
    jlong address = env->GetLongField(req,addressFieldId);

    jfieldID is_writeFieldId = env->GetFieldID(requestClass,"is_write","Z");
    jboolean is_write = env->GetBooleanField(req,is_writeFieldId);

    jfieldID idFieldId = env->GetFieldID(requestClass,"id","I");
    jint id = env->GetIntField(req,idFieldId);

    jfieldID uidFieldId = env->GetFieldID(requestClass,"uid","I");
    jint uid = env->GetIntField(req,uidFieldId);

    jfieldID lengthFieldId = env->GetFieldID(requestClass,"length","I");
    jint length = env->GetIntField(req,lengthFieldId);

    jfieldID lastFieldId = env->GetFieldID(requestClass,"last","Z");
    jboolean last = env->GetBooleanField(req,lastFieldId);

    jfieldID enableFieldId = env->GetFieldID(requestClass,"enable","Z");
    jboolean enable = env->GetBooleanField(req,enableFieldId);

    jfieldID writebackFieldId = env->GetFieldID(requestClass,"writeback","Z");
    jboolean writeback = env->GetBooleanField(req,writebackFieldId);

    CoDRAMTrans* DramRequest = new CoDRAMTrans(address,
    is_write,id,uid,
    length, last, enable, writeback);

    return (jboolean) dramsim->add_request(DramRequest);

}

JNIEXPORT jint JNICALL Java_DramSimulator_DramSimulator_clock_1period
  (  JNIEnv *env, jobject obj)
  {
    return (jint) dramsim->clock_period();
  }