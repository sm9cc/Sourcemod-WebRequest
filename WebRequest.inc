#if defined _webrequest_included
#endinput
#endif
#define _webrequest_included

#if defined REQUIRE_EXTENSIONS
#undef REQUIRE_EXTENSIONS
#endif

#include <steamtools>
#include <SteamWorks>

bool g_bExtensions = false;
bool g_bSteamTools = false;
bool g_bSteamWorks = false;

enum EHTTPMethodInternal
{
	GET, 
	POST, 
	HEAD
}

methodmap WebRequest < Handle {
	public WebRequest() {
		StringMap smWebRequest = new StringMap();
		
		if (!g_bExtensions) {
			WebRequest_CheckExtensions();
		}
		
		if (!g_bSteamWorks && !g_bSteamTools) {
			delete smWebRequest;
			ThrowNativeError(SP_ERROR_NATIVE, "SteamWorks or SteamTools is required to make a WebRequest!");
			return null;
		}
		
		return view_as<WebRequest>(smWebRequest);
	}
	
	public bool SetUrl(const char[] szUrl) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetString("url", szUrl, true);
	}
	
	public bool SetMethod(EHTTPMethodInternal eMethod) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetValue("method", eMethod, true);
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
	
	public bool SetParamFloat(const char[] szKey, float fValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smParams;
		
		if (!smWebRequest.GetValue("params", smParams)) {
			smParams = new StringMap();
			smWebRequest.SetValue("params", smParams, false);
		}
		
		return smParams.SetValue(szKey, fValue, true);
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
	
	public bool SetPath(const char[] szPath) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.SetString("path", szPath, true);
	}
	
	public bool SetCallBack(Function fCallBack) {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		//Hack.
		int iCallBack = view_as<int>(fCallBack);
		
		return smWebRequest.SetValue("callback", iCallBack);
	}
	
	public Function GetCallBack() {
		StringMap smWebRequest = view_as<StringMap>(this);
		
		//Hack.
		int iCallBack;
		
		if (!smWebRequest.GetValue("callback", iCallBack)) {
			return view_as<Function>(INVALID_FUNCTION);
		}
		
		return view_as<Function>(iCallBack);
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
	
	public bool SetContextString(const char[] szKey, const char[] szValue) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smContext == null) {
			smContext = new StringMap();
			smWebRequest.SetValue("context", smContext);
		}
		
		return smContext.SetString(szKey, szValue);
	}
	
	public bool SetClientContext(int iClient) {
		return this.SetContextValue("client", GetClientSerial(iClient));
	}
	
	public bool SetEntityContext(int iEntity) {
		return this.SetContextValue("entity", EntIndexToEntRef(iEntity));
	}
	
	public bool GetUrl(char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.GetString("url", szReturn, iSize);
	}
	
	public bool GetPath(char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		return smWebRequest.GetString("path", szReturn, iSize);
	}
	
	public EHTTPMethodInternal GetMethod() {
		StringMap smWebRequest = view_as<StringMap>(this);
		EHTTPMethodInternal eMethod; smWebRequest.GetValue("method", eMethod);
		
		return eMethod;
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
	
	public bool GetContextString(const char[] szKey, char[] szReturn, int iSize) {
		StringMap smWebRequest = view_as<StringMap>(this);
		StringMap smContext; smWebRequest.GetValue("context", smContext);
		
		if (smContext == null) {
			return false;
		}
		
		return smContext.GetString(szKey, szReturn, iSize);
	}
	
	public int GetClientContext() {
		return GetClientFromSerial(this.GetContextValue("client"));
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
		StringMap smContext;
		StringMap smParams;
		
		char szUrl[256];
		char szPath[PLATFORM_MAX_PATH];
		
		EHTTPMethodInternal eMethod;
		
		int iCallBack = INVALID_FUNCTION;
		
		if (!smWebRequest.GetString("url", szUrl, sizeof(szUrl))) {
			LogError("You must set a WebRequest url before sending!");
			return false;
		}
		
		if (!smWebRequest.GetValue("method", eMethod)) {
			LogError("You must set a WebRequest method before sending!");
			return false;
		}
		
		if(!smWebRequest.GetValue("callback", iCallBack) || iCallBack == INVALID_FUNCTION) {
			smWebRequest.SetValue("callback", INVALID_FUNCTION);
		}
		
		smWebRequest.GetValue("context", smContext);
		smWebRequest.GetString("path", szPath, sizeof(szPath));
		smWebRequest.GetValue("params", smParams);
		
		Handle hSteamWorksRequest;
		HTTPRequestHandle hSteamToolsRequest;
		
		if (g_bSteamWorks) {
			EHTTPMethod swMethod;
			
			switch (eMethod) {
				case GET: { swMethod = k_EHTTPMethodGET; }
				case POST: { swMethod = k_EHTTPMethodPOST; }
				case HEAD: { swMethod = k_EHTTPMethodHEAD; }
			}
			
			hSteamWorksRequest = SteamWorks_CreateHTTPRequest(swMethod, szUrl);
			
			if (hSteamWorksRequest == null) {
				if(!SteamTools_FallBack(this)) {
					LogError("Failed to create WebRequest handle!");
					return false;
				}
				
				return true;
			}
			
		} else if (g_bSteamTools) {
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
		}
		
		char szParamKey[64];
		char szParamValue[64];
		
		StringMapSnapshot smParamKeys = smParams.Snapshot();
		
		bool bParamFailure = false;
		
		for (int i = 0; i < smParamKeys.Length; i++) {
			smParamKeys.GetKey(i, szParamKey, sizeof(szParamKey));
			smParams.GetString(szParamKey, szParamValue, sizeof(szParamValue));
			
			if (g_bSteamWorks && !SteamWorks_SetHTTPRequestGetOrPostParameter(hSteamWorksRequest, szParamKey, szParamValue)) {
				bParamFailure = true;
				break;
			} else if (g_bSteamTools) {
				if (!Steam_SetHTTPRequestGetOrPostParameter(hSteamToolsRequest, szParamKey, szParamValue)) {
					Steam_ReleaseHTTPRequest(hSteamToolsRequest);
					LogError("Failed to set WebRequest params!");
					return false;
				}
			}
		}
		
		delete smParamKeys;
		
		if (bParamFailure) {
			if (!SteamTools_FallBack(this)) {
				LogError("Failed to set WebRequest params!");
				return false;
			}
			
			return true;
		}
		
		if (g_bSteamWorks) {
			if (!SteamWorks_SetHTTPRequestContextValue(hSteamWorksRequest, smWebRequest)) {
				delete hSteamWorksRequest;
				
				if (!SteamTools_FallBack(this)) {
					LogError("Failed to set WebRequest context!");
					return false;
				}
				
				return true;
			}
			
			if (!SteamWorks_SetHTTPCallbacks(hSteamWorksRequest, WebRequest_SteamWorksComplete)) {
				delete hSteamWorksRequest;
				
				if (!SteamTools_FallBack(this)) {
					LogError("Failed to set WebRequest callback!");
					return false;
				}
				
				return true;
			}
			
			if (!SteamWorks_SendHTTPRequest(hSteamWorksRequest)) {
				delete hSteamWorksRequest;
				
				if (!SteamTools_FallBack(this)) {
					LogError("Failed to send WebRequest!");
					return false;
				}
				
				return true;
			}
			
		} else if (g_bSteamTools) {
			if (!Steam_SendHTTPRequest(hSteamToolsRequest, WebRequest_SteamToolsComplete, this)) {
				Steam_ReleaseHTTPRequest(hSteamToolsRequest);
				LogError("Failed to send WebRequest!");
				return false;
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
	
	g_bSteamWorks = false;
	bool bResult = wRequest.Send();
	g_bSteamWorks = true;
	
	return bResult;
}

public int WebRequest_SteamWorksComplete(Handle hSteamWorksRequest, bool bFailure, bool bSuccessful, EHTTPStatusCode httpStatusCode, WebRequest wRequest)
{
	Function fCallBack = wRequest.GetCallBack();
	
	if (!bSuccessful || bFailure) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Unknown) HTTP status: %d", httpStatusCode);
		return;
	}
	
	int iBodySize = 0;
	
	if (!SteamWorks_GetHTTPResponseBodySize(hSteamWorksRequest, iBodySize)) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Unable to retrieve body size) HTTP status: %d", httpStatusCode);
		return;
	}
	
	if (iBodySize < 1) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Body Empty) HTTP status: %d", httpStatusCode);
		return;
	}
	
	char[] szBodyData = new char[iBodySize + 1];
	
	if (!SteamWorks_GetHTTPResponseBodyData(hSteamWorksRequest, szBodyData, iBodySize) || strlen(szBodyData) < 1) {
		delete hSteamWorksRequest;
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Could not retrieve body data) HTTP status: %d", httpStatusCode);
		return;
	}
	
	char szPath[PLATFORM_MAX_PATH];
	
	if (wRequest.GetPath(szPath, sizeof(szPath))) {
		if (BuildPathRecursively(szPath)) {
			File fFile = OpenFile(szPath, "w+");
			
			if (fFile != null) {
				fFile.WriteString(szBodyData, false);
				fFile.Close();
			}
		}
	}
	
	if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
		Call_StartFunction(INVALID_HANDLE, fCallBack);
		Call_PushCell(wRequest);
		Call_PushCell(true);
		Call_PushCell(view_as<int>(httpStatusCode));
		Call_PushString(szBodyData);
		Call_Finish();
	}
	
	delete hSteamWorksRequest;
}

public int WebRequest_SteamToolsComplete(HTTPRequestHandle hSteamToolsRequest, bool bSuccessful, HTTPStatusCode httpStatusCode, WebRequest wRequest)
{
	Function fCallBack = wRequest.GetCallBack();
	
	if (!bSuccessful) {
		Steam_ReleaseHTTPRequest(hSteamToolsRequest);
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Unknown) HTTP status: %d", httpStatusCode);
		return;
	}
	
	int iBodySize = Steam_GetHTTPResponseBodySize(hSteamToolsRequest);
	
	if (iBodySize < 1) {
		Steam_ReleaseHTTPRequest(hSteamToolsRequest);
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Body Empty) HTTP status: %d", httpStatusCode);
		return;
	}
	
	char[] szBodyData = new char[iBodySize + 1];
	Steam_GetHTTPResponseBodyData(hSteamToolsRequest, szBodyData, iBodySize);
	
	if (strlen(szBodyData) < 1) {
		Steam_ReleaseHTTPRequest(hSteamToolsRequest);
		
		if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
			Call_StartFunction(INVALID_HANDLE, fCallBack);
			Call_PushCell(wRequest);
			Call_PushCell(false);
			Call_PushCell(view_as<int>(httpStatusCode));
			Call_PushString("");
			Call_Finish();
		}
		
		LogError("Request failed. (Could not retrieve body data) HTTP status: %d", httpStatusCode);
		return;
	}
	
	char szPath[PLATFORM_MAX_PATH];
	
	if (wRequest.GetPath(szPath, sizeof(szPath))) {
		if (BuildPathRecursively(szPath)) {
			File fFile = OpenFile(szPath, "w+");
			
			if (fFile != null) {
				fFile.WriteString(szBodyData, false);
				fFile.Close();
			}
		}
	}
	
	if (fCallBack != view_as<Function>(INVALID_FUNCTION)) {
		Call_StartFunction(INVALID_HANDLE, fCallBack);
		Call_PushCell(wRequest);
		Call_PushCell(true);
		Call_PushCell(view_as<int>(httpStatusCode));
		Call_PushString(szBodyData);
		Call_Finish();
	}
	
	Steam_ReleaseHTTPRequest(hSteamToolsRequest);
}

stock void WebRequest_CheckExtensions()
{
	g_bExtensions = true;
	g_bSteamTools = (GetExtensionFileStatus("steamtools.ext") == 1);
	g_bSteamWorks = (GetExtensionFileStatus("SteamWorks.ext") == 1);
}

stock bool BuildPathRecursively(const char[] szInput)
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
	
	if(strcopy(szPath, sizeof(szPath), szInput) < 1) {
		return false;
	}
	
	ReplaceString(szPath, sizeof(szPath), szFile, "", false);
	ReplaceString(szPath, sizeof(szPath), "/", "\\", false);
	
	char szPieces[32][PLATFORM_MAX_PATH];
	int iNumPieces = ExplodeString(szPath, "\\", szPieces, sizeof(szPieces), sizeof(szPieces[]));
	
	if(iNumPieces < 1) {
		return false;
	}
	
	char szTempPath[PLATFORM_MAX_PATH]; 
	
	for (int i = 0; i < iNumPieces; i++) {
		Format(szTempPath, PLATFORM_MAX_PATH, "%s\\%s", szTempPath, szPieces[i]);
		
		if (DirExists(szTempPath)) {
			continue;
		}
		
		CreateDirectory(szTempPath, 511);
	}
	
	return DirExists(szPath);
}