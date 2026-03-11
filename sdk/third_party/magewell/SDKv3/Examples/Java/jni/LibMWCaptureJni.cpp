// LibMWCapture_JAVA.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibMWCaptureJni.h"

JavaVM  *g_pVM = NULL;

char* WCHARToChar(WCHAR *wchar)
{
	int nLength = WideCharToMultiByte(CP_ACP, 0, wchar, -1, NULL, 0, NULL, NULL);
	if(nLength == 0){
		return NULL;
	}
	char *str = (char *)malloc(nLength * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, wchar, -1, str, nLength, NULL, NULL);

	return str;
}

WCHAR* CharToWCHAR(char *str)
{
	int nLength = MultiByteToWideChar(CP_ACP, NULL, str, -1, NULL, 0);
	if (nLength == 0){
		return NULL;
	}
	WCHAR* wchar = (WCHAR *)malloc(nLength * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, str, -1, wchar, nLength);

	return wchar;
}

char* jstringToChar(JNIEnv *env,jstring jstr)
{
	int length = (env)->GetStringLength(jstr );
	const jchar* jcstr = (env)->GetStringChars(jstr, 0);
	char* rtn = (char*)malloc(length * 2 + 1);
	int size = 0;
	size = WideCharToMultiByte(CP_ACP, 0,(LPCWSTR)jcstr, length, rtn,(length * 2 + 1), NULL, NULL);
	if(size <= 0)
		return NULL;
	(env)->ReleaseStringChars(jstr, jcstr);
	rtn[size] = 0;
	return rtn;
}

jstring CharTojstring(JNIEnv* env, char* str)
{
	jstring rtn = 0;
	int slen = strlen(str);
	unsigned short * buffer = 0;
	if(slen == 0)
		rtn = (env)->NewStringUTF(str); 
	else{
		int length = MultiByteToWideChar(CP_ACP, 0,(LPCSTR)str, slen, NULL, 0);
		buffer = (unsigned short *)malloc(length * 2 + 1);
		if( MultiByteToWideChar(CP_ACP, 0,(LPCSTR)str, slen,(LPWSTR)buffer, length ) >0)
			rtn = (env)->NewString((jchar*)buffer, length);
	}
	if(buffer)
		free(buffer);
	return rtn;
}


JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_InitNative
	(JNIEnv *env, jclass)
{
	env->GetJavaVM(&g_pVM);

	return true;
}

JNIEXPORT void JNICALL Java_com_magewell_libmwcapture_LibMWCapture_ExitNative
	(JNIEnv *, jclass)
{

}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_GetJavaWnd
	(JNIEnv *env, jclass clazz, jobject objWindow)
{
	HWND hwnd = NULL;
	JAWT_DrawingSurface *ds;
	JAWT_DrawingSurfaceInfo *dsi;
	JAWT_Win32DrawingSurfaceInfo *win;

	JAWT awt;
	awt.version = JAWT_VERSION_1_3;
	jboolean result = JAWT_GetAWT(env, &awt);

	if(result == JNI_TRUE){
		ds = awt.GetDrawingSurface(env, objWindow);
		jint lock = ds->Lock(ds);
		if(lock != JAWT_LOCK_ERROR){
			dsi = ds->GetDrawingSurfaceInfo(ds);
			win = (JAWT_Win32DrawingSurfaceInfo *)dsi->platformInfo;   
			hwnd = win->hwnd;
			ds->FreeDrawingSurfaceInfo(dsi);
			ds->Unlock(ds);
			awt.FreeDrawingSurface(ds);
		}
	}

	return (jlong)hwnd;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCaptureInitInstance
	(JNIEnv *, jclass)
{
	return (jboolean)MWCaptureInitInstance();
}

JNIEXPORT void JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCaptureExitInstance
	(JNIEnv *, jclass)
{
	MWCaptureExitInstance();
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetFourcc
	(JNIEnv *env, jclass, jstring strColorFormat)
{
	char *ch = new char[4];
	ch = jstringToChar(env, strColorFormat);
	DWORD dwFourcc = ((DWORD)(BYTE)(ch[0]) | ((DWORD)(BYTE)(ch[1]) << 8) | ((DWORD)(BYTE)(ch[2]) << 16) | ((DWORD)(BYTE)(ch[3]) << 24));
	delete[] ch;
	return (jlong)dwFourcc;
}

JNIEXPORT jint JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetChannelCount
	(JNIEnv *, jclass)
{
	return (jint)MWGetChannelCount();
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWOpenChannelByIndex
	(JNIEnv *, jclass, jint jint_nChannelIndex)
{
	TCHAR szDevicePath[MAX_PATH];
	MWGetDevicePath(jint_nChannelIndex, szDevicePath);

	HCHANNEL hChannel = MWOpenChannelByPath(szDevicePath);
	return (jlong)hChannel;
}

JNIEXPORT void JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCloseChannel
	(JNIEnv *, jclass, jlong jlong_hChannel)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	MWCloseChannel(hChannel);
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWStartVideoCapture
	(JNIEnv *, jclass, jlong jlong_hChannel, jlong jlong_hEvent)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	HANDLE hEvent = (HANDLE)jlong_hEvent;

	return (jboolean)(MW_SUCCEEDED == MWStartVideoCapture(hChannel, hEvent));
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWStopVideoCapture
	(JNIEnv *, jclass, jlong jlong_hChannel)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;

	return (jboolean)(MW_SUCCEEDED == MWStopVideoCapture(hChannel));
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWStartAudioCapture
	(JNIEnv *, jclass, jlong jlong_hChannel)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;

	return (jboolean)(MW_SUCCEEDED == MWStartAudioCapture(hChannel));
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWStopAudioCapture
	(JNIEnv *, jclass, jlong jlong_hChannel)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;

	return (jboolean)(MW_SUCCEEDED == MWStopAudioCapture(hChannel));
}

//JNIEXPORT jobject JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetVideoSignalStatus
//	(JNIEnv *, jclass, jlong)
//{
//
//}
//
//JNIEXPORT jobject JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetVideoBufferInfo
//	(JNIEnv *, jclass, jlong)
//{
//
//}
//
//JNIEXPORT jobject JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetVideoFrameInfo
//	(JNIEnv *, jclass, jlong, jint)
//{
//
//}

JNIEXPORT jobject JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetVideoCaptureStatus
	(JNIEnv *env, jclass clazz, jlong jlong_hChannel)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	MWCAP_VIDEO_CAPTURE_STATUS status;
	if (MW_SUCCEEDED != MWGetVideoCaptureStatus(hChannel, &status))
		return NULL;

	jclass jc_captureStatus = env->FindClass("com/magewell/libmwcapture/VIDEO_CAPTURE_STATUS");
	if (jc_captureStatus == NULL)
		return NULL;

	jmethodID jm_initCaptureStatus = env->GetMethodID(jc_captureStatus, "<init>", "(JZIZII)V");
	if (jm_initCaptureStatus == NULL)
		return NULL;

	jobject objCaptureStatus = env->NewObject(jc_captureStatus, jm_initCaptureStatus, (jlong)status.pvContext, status.bPhysicalAddress, (jint)status.iFrame, 
		status.bFrameCompleted, (jint)status.cyCompleted, (jint)status.cyCompletedPrev);

	env->DeleteLocalRef(objCaptureStatus);

	return objCaptureStatus;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCaptureVideoFrameToVirtualAddress
	(JNIEnv *env, jclass, jlong jlong_hChannel, jint nFrameIndex, jobject objBuffer, jlong cbFrame, jlong cbStride, jboolean bBottomTop, jlong dwFourcc, jint nWidth, jint nHeight)
{
	jboolean bRet = false;
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	BYTE *pbyImage = (BYTE *)env->GetDirectBufferAddress(objBuffer);
	if (pbyImage == NULL)
		return bRet;

	bRet = (MW_SUCCEEDED == MWCaptureVideoFrameToVirtualAddress(hChannel, nFrameIndex, pbyImage, (DWORD)cbFrame, (DWORD)cbStride, bBottomTop, NULL, (DWORD)dwFourcc, nWidth, nHeight));
	return bRet;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCaptureAudioFrame
	(JNIEnv *env, jclass, jlong jlong_hChannel, jobject objeFrame)
{
	jboolean bRet = false;

	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	MWCAP_AUDIO_CAPTURE_FRAME audioFrame;

	if (MWCaptureAudioFrame(hChannel, &audioFrame) == MW_SUCCEEDED) {
		jclass objectClass = (env)->FindClass("com/magewell/libmwcapture/AUDIO_CAPTURE_FRAME");
		
		jfieldID cFrameCount = (env)->GetFieldID(objectClass, "cFrameCount", "I");
		jfieldID iFrame = (env)->GetFieldID(objectClass, "iFrame", "I");
		jfieldID dwSyncCode = (env)->GetFieldID(objectClass, "dwSyncCode", "I");
		jfieldID dwReserved = (env)->GetFieldID(objectClass, "dwReserved", "I");
		jfieldID llTimestamp = (env)->GetFieldID(objectClass, "llTimestamp", "J");
		jfieldID adwSamples = (env)->GetFieldID(objectClass, "adwSamples", "[I");

		(env)->SetIntField(objeFrame, cFrameCount, audioFrame.cFrameCount);
		(env)->SetIntField(objeFrame, iFrame, audioFrame.iFrame);
		(env)->SetIntField(objeFrame, dwSyncCode, audioFrame.dwSyncCode);
		(env)->SetIntField(objeFrame, dwReserved, audioFrame.dwFlags);
		(env)->SetLongField(objeFrame, llTimestamp, audioFrame.llTimestamp);
		
		jintArray jsamples = env->NewIntArray(sizeof(audioFrame.adwSamples)/sizeof(audioFrame.adwSamples[0])); 
		//jint* sample = env->GetIntArrayElements(jsamples,0);
		jint samples[sizeof(audioFrame.adwSamples)/sizeof(audioFrame.adwSamples[0])];
		for (int i = 0; i < sizeof(audioFrame.adwSamples)/sizeof(audioFrame.adwSamples[0]); i++) {
			samples[i] = (jint)audioFrame.adwSamples[i];
		}  

		//printf("%d %d\n", samples[0], samples[1]);
		
		env->SetIntArrayRegion(jsamples, 0, sizeof(audioFrame.adwSamples)/sizeof(audioFrame.adwSamples[0]), samples); 
		env->SetObjectField(objeFrame, adwSamples, jsamples);
		env->DeleteLocalRef(jsamples);
		
		env->DeleteLocalRef(objectClass);

		bRet = true;
	}

	return bRet;
}

static void OnVideoCaptureCallback(BYTE *pbFrame, int cbFrame, UINT64 u64TimeStamp, void* objCallback) {
	JNIEnv* env = NULL;  
	if(g_pVM->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
		printf("AttachCurrentThread() failed."); 
		return; 
	}

	jclass callbackClass = env->FindClass("com/magewell/libmwcapture/MWCaptureCallback");
	jmethodID method = env->GetMethodID(callbackClass, "OnVideoCapture", "([BIJ)V");

	//printf("video callback :%d env:%lld method:%lld pParam:%lld\n data:%d", cbFrame, env, method, pParam, pbFrame[100]);
	
	jbyteArray jsamples = env->NewByteArray(cbFrame);
	env->SetByteArrayRegion(jsamples , 0, cbFrame, (jbyte*)pbFrame);

	if (method) {
		env->CallVoidMethod((jobject)objCallback, method, jsamples, cbFrame, u64TimeStamp);
	}

	env->DeleteLocalRef(callbackClass);
	env->DeleteLocalRef(jsamples);

	if(g_pVM->DetachCurrentThread() != JNI_OK) { 
		printf("DetachCurrentThread() failed.");
	}
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCreateVideoCapture
	(JNIEnv *env, jclass, jlong jlong_hChannel, jint nWidth, jint nHeight, jint nFourcc, jint nFrameDuration, jobject callback)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;

	jobject objCallback = env->NewGlobalRef(callback);

	HANDLE hVideo = MWCreateVideoCapture(hChannel, nWidth, nHeight, nFourcc, nFrameDuration, OnVideoCaptureCallback, (void*)objCallback);

	return (jlong)hVideo;
}


JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWDestoryVideoCapture
	(JNIEnv *, jclass, jlong hVideo)
{
	MW_RESULT ret = MW_FAILED;
	if (hVideo != NULL) {
		ret = MWDestoryVideoCapture((HANDLE)hVideo);
		hVideo = NULL;
	}

	return (jboolean)(MW_SUCCEEDED == ret);
}

static void OnAudioCaptureCallback(const BYTE * pbFrame, int cbFrame, UINT64 u64TimeStamp, void* objCallback) {
	JNIEnv* env = NULL;  
	if(g_pVM->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
		printf("AttachCurrentThread() failed."); 
		return; 
	}

	jclass callbackClass = env->FindClass("com/magewell/libmwcapture/MWCaptureCallback");
	jmethodID method = env->GetMethodID(callbackClass, "OnAudioCapture", "([BIJ)V");

	//printf("audio callback :%d env:%lld method:%lld pParam:%lld\n data:%d\n", cbFrame, env, method, pParam);

	jbyteArray jsamples = env->NewByteArray(cbFrame);
	env->SetByteArrayRegion(jsamples , 0, cbFrame, (jbyte*)pbFrame);

	if (method) {
		env->CallVoidMethod((jobject)objCallback, method, jsamples, cbFrame, u64TimeStamp);
	}

	env->DeleteLocalRef(callbackClass);
	env->DeleteLocalRef(jsamples);

	if(g_pVM->DetachCurrentThread() != JNI_OK) { 
		printf("DetachCurrentThread() failed.");
	}
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCreateAudioCapture
	(JNIEnv *env, jclass, jlong jlong_hChannel, jint captureNode, jint dwSamplesPerSec, jint wBitsPerSample, jint wChannels, jobject callback)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;

	jobject objCallback = env->NewGlobalRef(callback);

	HANDLE hAudio = MWCreateAudioCapture(hChannel, (MWCAP_AUDIO_CAPTURE_NODE)captureNode, dwSamplesPerSec, wBitsPerSample, wChannels, OnAudioCaptureCallback, (void*)objCallback);

	return (jlong)hAudio;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWDestoryAudioCapture
	(JNIEnv *, jclass, jlong hAudio)
{
	MW_RESULT ret = MW_FAILED;
	if (hAudio != NULL) {
		ret = MWDestoryAudioCapture((HANDLE)hAudio);
		hAudio = NULL;
	}

	return (jboolean)(MW_SUCCEEDED == ret);
}


JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWGetDeviceTime
	(JNIEnv *, jclass, jlong jlong_hChannel)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	LONGLONG llBegin = 0LL;
	MWGetDeviceTime(hChannel, &llBegin);

	return (jlong)llBegin;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWScheduleTimer
	(JNIEnv *, jclass, jlong jlong_hChannel, jlong jlong_hTimer, jlong llExpireTime)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	HTIMER hTimer = (HTIMER)jlong_hTimer;

	return (jboolean)(MW_SUCCEEDED == MWScheduleTimer(hChannel, hTimer, llExpireTime));
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWRegisterTimer
	(JNIEnv *, jclass, jlong jlong_hChannel, jlong jlong_hEvent)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	HANDLE hEvent = (HANDLE)jlong_hEvent;

	HTIMER hTimer = MWRegisterTimer(hChannel, hEvent);
	return (jlong)hTimer;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWUnregisterTimer
	(JNIEnv *, jclass, jlong jlong_hChannel, jlong jlong_hTimer)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	HTIMER hTimer = (HTIMER)jlong_hTimer;

	return (jboolean)(MW_SUCCEEDED == MWUnregisterTimer(hChannel, hTimer));
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWRegisterNotify
	(JNIEnv *, jclass, jlong jlong_hChannel, jlong jlong_hEvent, jlong dwEnableBits)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	HANDLE hEvent = (HANDLE)jlong_hEvent;

	HNOTIFY hNotify = MWRegisterNotify(hChannel, hEvent, (DWORD)dwEnableBits);
	return (jlong) hNotify;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWUnregisterNotify
	(JNIEnv *, jclass, jlong jlong_hChannel, jlong jlong_hNotify)
{
	HCHANNEL hChannel = (HCHANNEL)jlong_hChannel;
	HNOTIFY hNotify = (HNOTIFY)jlong_hNotify;

	return (jboolean)(MW_SUCCEEDED == MWUnregisterNotify(hChannel, hNotify));
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_FOURCC_1CalcImageSize
	(JNIEnv *, jclass, jlong dwFourcc, jint nWidth, jint nHeight, jlong cbStride)
{
	DWORD cbImageSize = FOURCC_CalcImageSize((DWORD)dwFourcc, nWidth, nHeight, (DWORD)cbStride);

	return (jlong)cbImageSize;
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_FOURCC_1CalcMinStride
	(JNIEnv *, jclass, jlong dwFourcc, jint nWidth, jlong dwAlign)
{
	DWORD cbStride = FOURCC_CalcMinStride((DWORD)dwFourcc, nWidth, (DWORD)dwAlign);

	return (jlong)cbStride;
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_CreateEvent
	(JNIEnv *, jclass)
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return (jlong)hEvent;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_SetEvent
	(JNIEnv *, jclass, jlong jlong_hEvent)
{
	HANDLE hEvent = (HANDLE)jlong_hEvent;

	return (jboolean)SetEvent(hEvent);
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_CloseHandle
	(JNIEnv *, jclass, jlong jlong_hEvent)
{
	HANDLE hEvent = (HANDLE)jlong_hEvent;

	return (jboolean)CloseHandle(hEvent);
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_WaitForMultipleObjects
	(JNIEnv *env, jclass clazz, jint nEventIndex, jlongArray jlong_hEventArr, jboolean bWaitAll, jlong dwMilliseconds)
{
	jlong* pHandle = env->GetLongArrayElements(jlong_hEventArr, NULL);
	jsize len = env->GetArrayLength(jlong_hEventArr);

	HANDLE hEventArr[32];
	for (int i = 0; i < len; i ++){
		//		printf("C++ HANDLE == %d\n", pHandle[i]);
		hEventArr[i] = (HANDLE)pHandle[i];
	}
	env->ReleaseLongArrayElements(jlong_hEventArr, pHandle, 0);

	return (jlong)WaitForMultipleObjects((DWORD)nEventIndex, hEventArr, bWaitAll, (DWORD)dwMilliseconds);
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_WaitForSingleObject
	(JNIEnv *, jclass, jlong jlong_hEvent, jlong dwMilliseconds)
{
	HANDLE hEvent = (HANDLE)jlong_hEvent;

	return (jlong)WaitForSingleObject(hEvent, (DWORD)dwMilliseconds);
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCreateD3DRenderer
	(JNIEnv *, jclass, jint nWidth, jint nHeight, jlong dwFourcc, jboolean bReverse, jboolean bMirror,jlong jlong_hWnd)
{
	HWND hWnd = (HWND)jlong_hWnd;
	HD3DRENDERER hD3dRenderer = MWCreateD3DRenderer(nWidth, nHeight, (DWORD)dwFourcc,bReverse,bMirror,hWnd);

	return (jlong)hD3dRenderer;
}

JNIEXPORT void JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWDestroyD3DRenderer
	(JNIEnv *, jclass, jlong jlong_D3dRenderer)
{
	HD3DRENDERER hD3dRenderer = (HD3DRENDERER)jlong_D3dRenderer;
	MWDestroyD3DRenderer(hD3dRenderer);
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWD3DRendererPushFrame
	(JNIEnv *env, jclass, jlong jlong_D3dRenderer, jobject objBuffer, jint cbStride)
{
	jboolean bRet = false;
	HD3DRENDERER hD3dRenderer = (HD3DRENDERER)jlong_D3dRenderer;
	BYTE *pbyImage = (BYTE *)env->GetDirectBufferAddress(objBuffer);
	if (pbyImage == NULL) {
		printf("GetDirectBufferAddress failed\n");
		return bRet;
	}

	bRet = MWD3DRendererPushFrame(hD3dRenderer, pbyImage, (DWORD)cbStride);

	return bRet;
}

JNIEXPORT jlong JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWCreateDSoundRenderer(
	JNIEnv *, jclass, jint nSamplesPerSec, jint nChannels, jint nSamplesPerFrame, jint nBufferFrameCount)
{
	HDSoundRENDERER hDSoundRenderer = MWCreateDSoundRenderer(nSamplesPerSec, nChannels, nSamplesPerFrame, nBufferFrameCount);

	return (jlong)hDSoundRenderer;
}

JNIEXPORT void JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWDestroyDSoundRenderer
	(JNIEnv *, jclass, jlong jlong_DSoundRenderer)
{
	HDSoundRENDERER hDSoundRendererr = (HDSoundRENDERER)jlong_DSoundRenderer;
	MWDestroyDSoundRenderer(hDSoundRendererr);
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWDSoundRendererPushFrame
	(JNIEnv *env, jclass, jlong jlong_DSoundRenderer, jintArray jSamples)
{
	jboolean bRet = false;
	HDSoundRENDERER hDSoundRendererr = (HDSoundRENDERER)jlong_DSoundRenderer;
	
	DWORD adwSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS];


	jint* pSamples = env->GetIntArrayElements(jSamples, NULL);
	jsize len = env->GetArrayLength(jSamples);

	for (int i = 0; i < len; i++){
		adwSamples[i] = (DWORD)pSamples[i];
	}

	env->ReleaseIntArrayElements(jSamples, pSamples, 0);

	short asAudioSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * 2];
	for (int i = 0; i < MWCAP_AUDIO_SAMPLES_PER_FRAME; i++) {
		short sLeft = (short)(adwSamples[i * 8] >> 16);
		short sRight = (short)(adwSamples[i * 8 + 4] >> 16);

		asAudioSamples[i * 2] = sLeft;
		asAudioSamples[i * 2 + 1] = sRight;
	}


	bRet = MWDSoundRendererPushFrame(hDSoundRendererr,(const BYTE*)asAudioSamples, MWCAP_AUDIO_SAMPLES_PER_FRAME * 2 * sizeof(short));

	return bRet;
}

JNIEXPORT jboolean JNICALL Java_com_magewell_libmwcapture_LibMWCapture_MWDSoundRendererPushFrame2
	(JNIEnv *env, jclass, jlong jlong_DSoundRenderer, jbyteArray jSamples)
{
	jboolean bRet = false;
	HDSoundRENDERER hDSoundRendererr = (HDSoundRENDERER)jlong_DSoundRenderer;
	jbyte *bytes = env->GetByteArrayElements(jSamples, 0);
	int length = env->GetArrayLength(jSamples);

	//printf("MWDSoundRendererPushFrame2 length:%d \n",  length);

	bRet = MWDSoundRendererPushFrame(hDSoundRendererr,(const BYTE*)bytes, length);

	env->ReleaseByteArrayElements(jSamples, bytes, 0);

	return bRet;
}
