#if defined _webrequest_included
#endinput
#endif
#define _webrequest_included

#if defined REQUIRE_EXTENSIONS
#undef REQUIRE_EXTENSIONS
#endif

#include <steamtools>
#include <SteamWorks>

#define MAX_RESPONSE_BYTES 104857600
#pragma dynamic MAX_RESPONSE_BYTES

bool g_bExtensions = false;
bool g_bSteamTools = false;
bool g_bSteamWorks = false;

enum EHTTPMethodInternal
{
	GET, 
	POST, 
	HEAD
}

enum EHTTPExtension
{
	STEAMTOOLS, 
	STEAMWORKS
}

EHTTPExtension g_EHTTPExtension;

typeset WebRequest_Completed {
	function void(WebRequest wRequest, bool bSuccess, int iStatusCode, const char[] szBodyData, int iBodySize);
};

typeset WebRequest_Downloaded {
	function void(WebRequest wRequest, const char[] szPath, int iBytesWritten);
}

methodmap WebRequest < Handle {
	public WebRequest() {
		StringMap smWebRequest = new StringMap();
		
		if (!g_bExtensions) {
			WebRequest_CheckExtensions();
		}
		
		if (!g_bSteamWorks && !g_bSteamTools) {
			delete smWebRequest;
			LogError("SteamWorks or SteamTools is required to make a WebRequest!");
			return null;
		}
		
		return view_as<WebRequest>(smWebRequest);
	}
	
	public bool SetUrl(const char[] szUrl) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetString("url", szUrl, true);
	}
	
	public bool GetUrl(char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.GetString("url", szReturn, iSize);
	}
	
	public bool SetMethod(EHTTPMethodInternal eMethod) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetValue("method", eMethod, true);
	}
	
	public EHTTPMethodInternal GetMethod() {
		StringMap smWebRequest = view_as<StringMap>(this);
		EHTTPMethodInternal eMethod; smWebRequest.GetValue("method", eMethod);
		
		return eMethod;
	}
	
	public bool SetTimeOut(int iTimeOut) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetValue("timeout", iTimeOut, true);
	}
	
	public int GetTimeOut() {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		int iTimeOut = -1;
		
		if (!smWebRequest.GetValue("timeout", iTimeOut)) {
			return -1;
		}
		
		return iTimeOut;
	}
	
	public bool SetHeader(const char[] szName, const char[] szValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		StringMap smHeaders;
		
		if (!smWebRequest.GetValue("headers", smHeaders)) {
			smHeaders = new StringMap();
			smWebRequest.SetValue("headers", smHeaders, false);
		}
		
		return smHeaders.SetString(szName, szValue, true);
	}
	
	public bool GetHeader(const char[] szName, char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		EHTTPExtension eHTTPExtension;
		any aRequestHandle;
		
		if (smWebRequest.GetValue("requestextension", eHTTPExtension) && smWebRequest.GetValue("requesthandle", aRequestHandle)) {
			switch (eHTTPExtension) {
				case STEAMWORKS: {
					Handle hSteamWorksRequest = view_as<Handle>(aRequestHandle);
					
					if (hSteamWorksRequest == null) {
						return false;
					}
					
					return SteamWorks_GetHTTPResponseHeaderValue(hSteamWorksRequest, szName, szReturn, iSize);
				}
				
				case STEAMTOOLS: {
					HTTPRequestHandle hSteamToolsRequest = view_as<HTTPRequestHandle>(aRequestHandle);
					
					if (hSteamToolsRequest == INVALID_HTTP_HANDLE) {
						return false;
					}
					
					Steam_GetHTTPResponseHeaderValue(hSteamToolsRequest, szName, szReturn, iSize);
					
					return strlen(szReturn) > 0;
				}
			}
		}
		
		StringMap smHeaders;
		
		if (!smWebRequest.GetValue("headers", smHeaders)) {
			return false;
		}
		
		return smHeaders.GetString(szName, szReturn, iSize);
	}
	
	public bool SetReferrer(const char[] szValue) {
		return this.SetHeader("referrer", szValue);
	}
	
	public bool GetReferrer(char[] szReturn, int iSize) {
		return this.GetHeader("referrer", szReturn, iSize);
	}
	
	public bool SetUserAgent(const char[] szValue) {
		return this.SetHeader("User-Agent", szValue);
	}
	
	public bool GetUserAgent(char[] szReturn, int iSize) {
		return this.GetHeader("User-Agent", szReturn, iSize);
	}
	
	public bool SetParamString(const char[] szKey, const char[] szValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			smParams = new StringMap();
			smWebRequest.SetValue("params", smParams, false);
		}
		
		return smParams.SetString(szKey, szValue, true);
	}
	
	public bool GetParamString(const char[] szKey, char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			return false;
		}
		
		return smParams.GetString(szKey, szReturn, iSize);
	}
	
	public bool SetParamFloat(const char[] szKey, float fValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			smParams = new StringMap();
			smWebRequest.SetValue("params", smParams, false);
		}
		
		return smParams.SetValue(szKey, fValue, true);
	}
	
	public float GetParamFloat(const char[] szKey) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			return -1.0;
		}
		
		float fReturn = -1.0;
		
		if (!smParams.GetValue(szKey, fReturn)) {
			return -1.0;
		}
		
		return fReturn;
	}
	
	public bool SetParamInt(const char[] szKey, int iValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			smParams = new StringMap();
			smWebRequest.SetValue("params", smParams, false);
		}
		
		return smParams.SetValue(szKey, iValue, true);
	}
	
	public int GetParamInt(const char[] szKey) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			return false;
		}
		
		int iReturn = -1;
		
		if (!smParams.GetValue(szKey, iReturn)) {
			return -1;
		}
		
		return iReturn;
	}
	
	public bool SetPath(const char[] szPath) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetString("path", szPath, true);
	}
	
	public bool GetPath(char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.GetString("path", szReturn, iSize);
	}
	
	public bool SetCallBack(WebRequest_Completed fCallBack) {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		//Hack.
		int iCallBack = view_as<int>(fCallBack);
		
		return smWebRequest.SetValue("callback", iCallBack);
	}
	
	public WebRequest_Completed GetCallBack() {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		//Hack.
		int iCallBack;
		
		if (!smWebRequest.GetValue("callback", iCallBack)) {
			return view_as<WebRequest_Completed>(INVALID_FUNCTION);
		}
		
		return view_as<WebRequest_Completed>(iCallBack);
	}
	
	public bool SetDownloadCallBack(WebRequest_Downloaded fCallBack) {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		//Hack.
		int iCallBack = view_as<int>(fCallBack);
		
		return smWebRequest.SetValue("downloadcallback", iCallBack);
	}
	
	public WebRequest_Downloaded GetDownloadCallBack() {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		//Hack.
		int iCallBack;
		
		if (!smWebRequest.GetValue("downloadcallback", iCallBack)) {
			return view_as<WebRequest_Downloaded>(INVALID_FUNCTION);
		}
		
		return view_as<WebRequest_Downloaded>(iCallBack);
	}
	
	public bool SetContextValue(const char[] szKey, any aContextValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smContext == null) {
			smContext = new StringMap();
			smWebRequest.SetValue("context", smContext);
		}
		
		return smContext.SetValue(szKey, aContextValue);
	}
	
	public any GetContextValue(const char[] szKey) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smContext == null) {
			return -1;
		}
		
		any aContextValue;
		
		if (!smContext.GetValue(szKey, aContextValue)) {
			return -1;
		}
		
		return aContextValue;
	}
	
	public bool SetContextString(const char[] szKey, const char[] szValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smContext == null) {
			smContext = new StringMap();
			smWebRequest.SetValue("context", smContext);
		}
		
		return smContext.SetString(szKey, szValue);
	}
	
	public bool GetContextString(const char[] szKey, char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smContext == null) {
			return false;
		}
		
		return smContext.GetString(szKey, szReturn, iSize);
	}
	
	public bool SetClientContext(int iClient) {
		return this.SetContextValue("client", GetClientSerial(iClient));
	}
	
	public int GetClientContext() {
		return GetClientFromSerial(this.GetContextValue("client"));
	}
	
	public bool SetEntityContext(int iEntity) {
		return this.SetContextValue("entity", EntIndexToEntRef(iEntity));
	}
	
	public int GetEntityContext() {
		return EntRefToEntIndex(this.GetContextValue("entity"));
	}
	
	public void Clear() {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		StringMap smParams; smWebRequest.GetValue("params", smParams);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smParams != null) {
			smParams.Clear();
		}
		
		if (smContext != null) {
			smContext.Clear();
		}
	}
	
	public void Dispose() {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		StringMap smParams; smWebRequest.GetValue("params", smParams);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		delete smContext;
		delete smParams;
	}
	
	public void Destroy() {
		this.Dispose();
		delete this;
	}
	
	public bool Send() {
		if (!g_bSteamWorks && !g_bSteamTools) {
			LogError("SteamWorks or SteamTools is required to make a WebRequest!");
			return false;
		}
		
		StringMap smWebRequest = view_as<StringMap>(this);
		any aRequestHandle;
		
		if (smWebRequest.GetValue("requesthandle", aRequestHandle)) {
			LogError("You have already sent this request!");
			return false;
		}
		
		StringMap smContext;
		StringMap smParams;
		StringMap smHeaders;
		
		char szUrl[256];
		char szPath[PLATFORM_MAX_PATH];
		
		EHTTPMethodInternal eMethod;
		
		int iCallBack = INVALID_FUNCTION;
		int iDownloadCallBack = INVALID_FUNCTION;
		
		if (!smWebRequest.GetString("url", szUrl, sizeof(szUrl))) {
			LogError("You must set a WebRequest url before sending!");
			return false;
		}
		
		if (!smWebRequest.GetValue("method", eMethod)) {
			LogError("You must set a WebRequest method before sending!");
			return false;
		}
		
		if (!smWebRequest.GetValue("callback", iCallBack) || iCallBack == INVALID_FUNCTION) {
			smWebRequest.SetValue("callback", INVALID_FUNCTION);
		}
		
		if (!smWebRequest.GetValue("downloadcallback", iDownloadCallBack) || iDownloadCallBack == INVALID_FUNCTION) {
			smWebRequest.SetValue("downloadcallback", INVALID_FUNCTION);
		}
		
		smWebRequest.GetValue("context", smContext);
		smWebRequest.GetString("path", szPath, sizeof(szPath));
		smWebRequest.GetValue("params", smParams);
		smWebRequest.GetValue("headers", smHeaders);
		
		Handle hSteamWorksRequest;
		HTTPRequestHandle hSteamToolsRequest;
		
		switch (g_EHTTPExtension) {
			case STEAMWORKS: {
				EHTTPMethod swMethod;
				
				switch (eMethod) {
					case GET: { swMethod = k_EHTTPMethodGET; }
					case POST: { swMethod = k_EHTTPMethodPOST; }
					case HEAD: { swMethod = k_EHTTPMethodHEAD; }
				}
				
				hSteamWorksRequest = SteamWorks_CreateHTTPRequest(swMethod, szUrl);
				
				if (hSteamWorksRequest == null) {
					if (!SteamTools_FallBack(this)) {
						LogError("Failed to create WebRequest handle!");
						return false;
					}
					
					return true;
				}
				
				smWebRequest.SetValue("requestextension", STEAMWORKS, true);
				smWebRequest.SetValue("requesthandle", hSteamWorksRequest, true);
			}
			
			case STEAMTOOLS: {
				HTTPMethod stMethod;
				
				switch (eMethod) {
					case GET: { stMethod = HTTPMethod_GET; }
					case POST: { stMethod = HTTPMethod_POST; }
					case HEAD: { stMethod = HTTPMethod_HEAD; }
				}
				
				hSteamToolsRequest = Steam_CreateHTTPRequest(stMethod, szUrl);
				
				if (hSteamToolsRequest == INVALID_HTTP_HANDLE) {
					LogError("Failed to create WebRequest handle!");
					return false;
				}
				
				smWebRequest.SetValue("requestextension", STEAMTOOLS, true);
				smWebRequest.SetValue("requesthandle", hSteamToolsRequest, true);
			}
		}
		
		if (smParams != null) {
			char szParamKey[64];
			char szParamValue[64];
			
			StringMapSnapshot smParamKeys = smParams.Snapshot();
			
			bool bParamFailure = false;
			
			for (int i = 0; i < smParamKeys.Length; i++) {
				smParamKeys.GetKey(i, szParamKey, sizeof(szParamKey));
				smParams.GetString(szParamKey, szParamValue, sizeof(szParamValue));
				
				switch (g_EHTTPExtension) {
					case STEAMWORKS: {
						if (!SteamWorks_SetHTTPRequestGetOrPostParameter(hSteamWorksRequest, szParamKey, szParamValue)) {
							bParamFailure = true;
							break;
						}
					}
					
					case STEAMTOOLS: {
						if (!Steam_SetHTTPRequestGetOrPostParameter(hSteamToolsRequest, szParamKey, szParamValue)) {
							Steam_ReleaseHTTPRequest(hSteamToolsRequest);
							smWebRequest.Remove("requesthandle");
							LogError("Failed to set WebRequest params!");
							return false;
						}
					}
				}
			}
			
			delete smParamKeys;
			
			if (bParamFailure) {
				delete hSteamWorksRequest;
				smWebRequest.Remove("requesthandle");
				
				if (!SteamTools_FallBack(this)) {
					smWebRequest.Remove("requesthandle");
					LogError("Failed to set WebRequest params!");
					return false;
				}
				
				return true;
			}
		}
		
		if (smHeaders != null) {
			char szHeaderName[256];
			char szHeaderValue[256];
			
			StringMapSnapshot smHeaderKeys = smHeaders.Snapshot();
			
			bool bHeaderFailure = false;
			
			for (int i = 0; i < smHeaderKeys.Length; i++) {
				smHeaderKeys.GetKey(i, szHeaderName, sizeof(szHeaderName));
				smHeaders.GetString(szHeaderName, szHeaderValue, sizeof(szHeaderValue));
				
				switch (g_EHTTPExtension) {
					case STEAMWORKS: {
						if (!SteamWorks_SetHTTPRequestHeaderValue(hSteamWorksRequest, szHeaderName, szHeaderValue)) {
							bHeaderFailure = true;
							break;
						}
					}
					
					case STEAMTOOLS: {
						Steam_SetHTTPRequestHeaderValue(hSteamToolsRequest, szHeaderName, szHeaderValue);
					}
				}
			}
			
			delete smHeaderKeys;
			
			if (bHeaderFailure) {
				delete hSteamWorksRequest;
				smWebRequest.Remove("requesthandle");
				
				if (!SteamTools_FallBack(this)) {
					smWebRequest.Remove("requesthandle");
					LogError("Failed to set WebRequest headers!");
					return false;
				}
				
				return true;
			}
		}
		
		switch (g_EHTTPExtension) {
			case STEAMWORKS: {
				if (!SteamWorks_SetHTTPRequestContextValue(hSteamWorksRequest, smWebRequest)) {
					delete hSteamWorksRequest;
					smWebRequest.Remove("requesthandle");
					
					if (!SteamTools_FallBack(this)) {
						smWebRequest.Remove("requesthandle");
						LogError("Failed to set WebRequest context!");
						return false;
					}
					
					return true;
				}
				
				if (!SteamWorks_SetHTTPCallbacks(hSteamWorksRequest, WebRequest_SteamWorksComplete)) {
					delete hSteamWorksRequest;
					smWebRequest.Remove("requesthandle");
					
					if (!SteamTools_FallBack(this)) {
						smWebRequest.Remove("requesthandle");
						LogError("Failed to set WebRequest callback!");
						return false;
					}
					
					return true;
				}
				
				if (!SteamWorks_SendHTTPRequest(hSteamWorksRequest)) {
					delete hSteamWorksRequest;
					smWebRequest.Remove("requesthandle");
					
					if (!SteamTools_FallBack(this)) {
						smWebRequest.Remove("requesthandle");
						LogError("Failed to send WebRequest!");
						return false;
					}
					
					return true;
				}
			}
			
			case STEAMTOOLS: {
				if (!Steam_SendHTTPRequest(hSteamToolsRequest, WebRequest_SteamToolsComplete, this)) {
					Steam_ReleaseHTTPRequest(hSteamToolsRequest);
					smWebRequest.Remove("requesthandle");
					LogError("Failed to send WebRequest!");
					return false;
				}
			}
		}
		
		return true;
	}
};

bool SteamTools_FallBack(WebRequest wRequest)
{
	if (!g_bSteamTools) {
		return false;
	}
	
	g_EHTTPExtension = STEAMTOOLS;
	bool bResult = wRequest.Send();
	g_EHTTPExtension = STEAMWORKS;
	
	return bResult;
}

public int WebRequest_SteamWorksComplete(Handle hSteamWorksRequest, bool bFailure, bool bSuccessful, EHTTPStatusCode httpStatusCode, WebRequest wRequest)
{
	WebRequest_Completed fCallBack = wRequest.GetCallBack();
	WebRequest_Downloaded fDownloadCallBack = wRequest.GetDownloadCallBack();
	
	if (!bSuccessful || bFailure) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(0);
			Call_Finish();
		}
		
		LogError("Request failed. (Unknown) HTTP status: %d", httpStatusCode);
		return;
	}
	
	int iBodySize = 0;
	
	if (!SteamWorks_GetHTTPResponseBodySize(hSteamWorksRequest, iBodySize)) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(iBodySize);
			Call_Finish();
		}
		
		LogError("Request failed. (Unable to retrieve body size) HTTP status: %d", httpStatusCode);
		return;
	}
	
	if (iBodySize < 1) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(iBodySize);
			Call_Finish();
		}
		
		LogError("Request failed. (Body Empty) HTTP status: %d", httpStatusCode);
		return;
	}
	
	if (iBodySize + 1 > MAX_RESPONSE_BYTES) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(iBodySize);
			Call_Finish();
		}
		
		LogError("MAX_RESPONSE_BYTES needs to be increased to %d or more.", iBodySize + 1);
		return;
	}
	
	char[] szBodyData = new char[iBodySize + 1];
	
	if (!SteamWorks_GetHTTPResponseBodyData(hSteamWorksRequest, szBodyData, iBodySize)) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(strlen(szBodyData));
			Call_Finish();
		}
		
		LogError("Request failed. (Could not retrieve body data) HTTP status: %d", httpStatusCode);
		return;
	}
	
	char szPath[PLATFORM_MAX_PATH];
	
	if (wRequest.GetPath(szPath, sizeof(szPath)) && BuildPathRecursively(szPath)) {
		File fFile = OpenFile(szPath, "w+");
		
		if (fFile != null) {
			if (fFile.WriteString(szBodyData, false) && fDownloadCallBack != view_as<WebRequest_Downloaded>(INVALID_FUNCTION)) {
				Call_StartFunction(null, fDownloadCallBack);
				Call_PushCell(wRequest);
				Call_PushString(szPath);
				Call_PushCell(FileSize(szPath));
				Call_Finish();
			}
			
			fFile.Close();
		}
	}
	
	if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
		Call_StartFunction(null, fCallBack);
		Call_PushCell(wRequest);
		Call_PushCell(true);
		Call_PushCell(view_as<int>(httpStatusCode));
		Call_PushString(szBodyData);
		Call_PushCell(strlen(szBodyData));
		Call_Finish();
	}
	
	delete hSteamWorksRequest;
}

public int WebRequest_SteamToolsComplete(HTTPRequestHandle hSteamToolsRequest, bool bSuccessful, HTTPStatusCode httpStatusCode, WebRequest wRequest)
{
	WebRequest_Completed fCallBack = wRequest.GetCallBack();
	WebRequest_Downloaded fDownloadCallBack = wRequest.GetDownloadCallBack();
	
	if (!bSuccessful) {
		Steam_ReleaseHTTPRequest(hSteamToolsRequest);
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(0);
			Call_Finish();
		}
		
		LogError("Request failed. (Unknown) HTTP status: %d", httpStatusCode);
		return;
	}
	
	int iBodySize = Steam_GetHTTPResponseBodySize(hSteamToolsRequest);
	
	if (iBodySize < 1) {
		Steam_ReleaseHTTPRequest(hSteamToolsRequest);
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(iBodySize);
			Call_Finish();
		}
		
		LogError("Request failed. (Body Empty) HTTP status: %d", httpStatusCode);
		return;
	}
	
	if (iBodySize + 1 > MAX_RESPONSE_BYTES) {
		Steam_ReleaseHTTPRequest(hSteamToolsRequest);
		
		if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
			Call_StartFunction(null, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_PushCell(iBodySize);
			Call_Finish();
		}
		
		LogError("MAX_RESPONSE_BYTES needs to be increased to %d or more.", iBodySize + 1);
		return;
	}
	
	char[] szBodyData = new char[iBodySize + 1];
	Steam_GetHTTPResponseBodyData(hSteamToolsRequest, szBodyData, iBodySize);
	
	char szPath[PLATFORM_MAX_PATH];
	
	if (wRequest.GetPath(szPath, sizeof(szPath)) && BuildPathRecursively(szPath)) {
		File fFile = OpenFile(szPath, "w+");
		
		if (fFile != null) {
			if (fFile.WriteString(szBodyData, false) && fDownloadCallBack != view_as<WebRequest_Downloaded>(INVALID_FUNCTION)) {
				Call_StartFunction(null, fDownloadCallBack);
				Call_PushCell(wRequest);
				Call_PushString(szPath);
				Call_PushCell(FileSize(szPath));
				Call_Finish();
			}
			
			fFile.Close();
		}
	}
	
	if (fCallBack != view_as<WebRequest_Completed>(INVALID_FUNCTION)) {
		Call_StartFunction(null, fCallBack);
		Call_PushCell(wRequest);
		Call_PushCell(true);
		Call_PushCell(view_as<int>(httpStatusCode));
		Call_PushString(szBodyData);
		Call_PushCell(strlen(szBodyData));
		Call_Finish();
	}
	
	Steam_ReleaseHTTPRequest(hSteamToolsRequest);
}

void WebRequest_CheckExtensions()
{
	g_bExtensions = true;
	g_bSteamTools = (GetExtensionFileStatus("steamtools.ext") == 1);
	g_bSteamWorks = (GetExtensionFileStatus("SteamWorks.ext") == 1);
	
	if (g_bSteamWorks) {
		g_EHTTPExtension = STEAMWORKS;
	}
}

bool BuildPathRecursively(const char[] szInput)
{
	char szFile[PLATFORM_MAX_PATH];
	
	if (strcopy(szFile, sizeof(szFile), szInput) < 1) {
		return false;
	}
	
	int iPos = -1;
	
	while ((iPos = StrContains(szFile, "\\", false)) != -1) {
		strcopy(szFile, sizeof(szFile), szFile[iPos+1]);
	}
	
	while((iPos = StrContains(szFile, "/", false)) != -1) {
		strcopy(szFile, sizeof(szFile), szFile[iPos+1]);
	}
	
	char szPath[PLATFORM_MAX_PATH]; 
	
	strcopy(szPath, sizeof(szPath), szInput);
	
	ReplaceString(szPath, sizeof(szPath), szFile, "", false);
	ReplaceString(szPath, sizeof(szPath), "/", "\\", false);
	
	char szPieces[32][PLATFORM_MAX_PATH];
	int iNumPieces = ExplodeString(szPath, "\\", szPieces, sizeof(szPieces), sizeof(szPieces[]));
	
	if(iNumPieces < 1) {
		return false;
	}
	
	char szTempPath[PLATFORM_MAX_PATH]; 
	
	for (int i = 0; i < iNumPieces; i++) {
		if(strlen(szPieces[i]) < 1) {
			break;
		}
		
		if(i == 0) {
			FormatEx(szTempPath, PLATFORM_MAX_PATH, "%s", szPieces[i]);
		} else {
			FormatEx(szTempPath, PLATFORM_MAX_PATH, "%s\\%s", szTempPath, szPieces[i]);
		}
		
		CreateDirectory(szTempPath, 511);
	}
	
	return true;
}