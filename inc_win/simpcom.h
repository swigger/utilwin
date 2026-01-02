#pragma once

template <class T>
class CRefImpl : public T
{
private:
	DWORD m_ref;

public:
	CRefImpl() : m_ref(0){};
	ULONG WINAPI AddRef(){return ++ m_ref;}
	ULONG WINAPI Release()
	{
		DWORD dw = -- m_ref;
		if (dw == 0)
		{
			delete this;
		}
		return dw;
	}
	HRESULT WINAPI QueryInterface(REFIID riid, void **ppvObject)
	{
		return ImpQueryInterface(riid, ppvObject);
	}
};

struct INondelegatingUnknown
{
	virtual HRESULT __stdcall NondelegatingQueryInterface(REFIID riid, void** ppv) = 0;
	virtual ULONG __stdcall NondelegatingAddRef() = 0;
	virtual ULONG __stdcall NondelegatingRelease() = 0;
};

template <class T>
class COuterCOImpl : public T, public INondelegatingUnknown
{
private:
	IUnknown * m_pOuter;
	DWORD m_ref;

public:
	void Init(IUnknown * outer) { m_pOuter = outer; }
	COuterCOImpl() : m_ref(0){};
	ULONG WINAPI AddRef() { return m_pOuter->AddRef(); }
	ULONG WINAPI Release() { return m_pOuter->Release(); }
	HRESULT WINAPI QueryInterface(REFIID riid, void **ppvObject)
	{
		return m_pOuter->QueryInterface(riid, ppvObject);
	}

	ULONG WINAPI NondelegatingAddRef() { return ++ m_ref; }
	ULONG WINAPI NondelegatingRelease()
	{
		DWORD dw = -- m_ref;
		if (dw == 0)
		{
			delete this;
		}
		return dw;
	}
	HRESULT WINAPI NondelegatingQueryInterface(REFIID riid, void ** ppv)
	{
		if (riid == IID_IUnknown)
		{
			*ppv = (INondelegatingUnknown*)this;
			((IUnknown*)(*ppv))->AddRef();
			return S_OK;
		}
		return ImpQueryInterface(riid, ppv);
	}
};

template <class T>
class CSimpComFactory : public IClassFactory
{
public:
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
	{
		if (pUnkOuter)
		{
			if (riid != IID_IUnknown)
				return CLASS_E_NOAGGREGATION;
			COuterCOImpl<T> * newt = new COuterCOImpl<T>;
			newt->Init(pUnkOuter);
			newt->NondelegatingAddRef();
			HRESULT hr = newt->NondelegatingQueryInterface(riid, ppvObject);
			newt->NondelegatingRelease();
			return hr;
		}
		else
		{
			CRefImpl<T> * newt = new CRefImpl<T>;
			newt->AddRef();
			HRESULT hr = newt->QueryInterface(riid, ppvObject);
			newt->Release();
			return hr;
		}
	}
    
    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock)
	{
		return E_NOTIMPL;
	}
	HRESULT ImpQueryInterface(REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_IClassFactory)
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
};

template <class T>
struct DispInfoRowT
{
	LPCTSTR name;
	bool isProperty;
	HRESULT (T::*func)(DISPPARAMS * arg, VARIANT * ret);
	HRESULT (T::*func_put)(DISPPARAMS * arg, VARIANT * ret);
};

#define SIMPCOM_FUNCP_yes(func) &this_class::func
#define SIMPCOM_FUNCP_no(func) NULL
#define SIMPCOM_FUNCP_(a,b,c) a(b,c)
#define SIMPCOM_FUNCP(func, has) SIMPCOM_FUNCP_(SIMPCOM_FUNCP_##has , func)

#define SIMPCOM_DISP_FUNC_ENTRY(name) {_CRT_WIDE(#name), false, &this_class::name, 0},
#define SIMPCOM_DISP_PROP_ENTRY(name, hasget, hasput)					\
{ _CRT_WIDE(#name), true, SIMPCOM_FUNCP(get_##name,hasget), SIMPCOM_FUNCP(put_##name,hasput)},
#define SIMPCOM_DISP_TABLE_BEGIN(thisclsname) \
	typedef thisclsname this_class; typedef DispInfoRowT<this_class> DispInfoRow; \
	DispInfoRow * getDispTable() { static DispInfoRow tbl[] = {
#define SIMPCOM_DISP_TABLE_END()  {0}}; return tbl;}

template <class T>
class CDispImpl : public T
{
private:
	DWORD m_ref;

public:
	CDispImpl() : m_ref(0){};
	ULONG WINAPI AddRef(){return ++ m_ref;}
	ULONG WINAPI Release()
	{
		DWORD dw = -- m_ref;
		if (dw == 0)
		{
			delete this;
		}
		return dw;
	}
	HRESULT WINAPI QueryInterface(REFIID riid, void **ppvObject)
	{
		return ImpQueryInterface(riid, ppvObject);
	}
public:
	HRESULT WINAPI GetTypeInfoCount(UINT *pctinfo)
	{
		return E_NOTIMPL;
	}
	HRESULT WINAPI GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}

	HRESULT WINAPI GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		if (cNames == 0) return E_INVALIDARG;
		DispInfoRow * tbl = getDispTable();

		for (UINT idx=0; idx<cNames; ++idx)
		{
			LPOLESTR name = rgszNames[idx];
			for (int i=0; tbl[i].name; ++i)
			{
				if (wcscmp(tbl[i].name, name) == 0)
				{
					rgDispId[idx] = i;
					goto g_pos;
				}
			}
			return E_FAIL;
g_pos:;
		}
		return S_OK;
	}

	HRESULT WINAPI Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		DispInfoRow * tbl = getDispTable();
		int nc = 0;
		for (;tbl[nc].name;++nc);
		if (dispIdMember >= nc || dispIdMember < 0) return E_INVALIDARG;
		DispInfoRow * row = & tbl[dispIdMember];
		if (row->isProperty)
		{
			if (wFlags == DISPATCH_PROPERTYGET || wFlags == (DISPATCH_METHOD|DISPATCH_PROPERTYGET))
			{
				if (! row->func) return E_UNEXPECTED;
				return (this->*row->func)(pDispParams, pVarResult);
			}
			else if (wFlags == DISPATCH_PROPERTYPUT || wFlags == (DISPATCH_METHOD|DISPATCH_PROPERTYPUT))
			{
				if (! row->func_put) return E_UNEXPECTED;
				return (this->*row->func_put)(pDispParams, pVarResult);
			}
		}
		else
		{
			if (wFlags == DISPATCH_METHOD || wFlags == (DISPATCH_METHOD|DISPATCH_PROPERTYGET))
			{
				return (this->*row->func)(pDispParams, pVarResult);
			}
		}
		return E_UNEXPECTED;
	}
};

template <class T, class I>
void CreateRefInst2(I ** pobj, T ** pInst = 0)
{
	CRefImpl<T> * pt = new CRefImpl<T>;
	*pobj = pt;
	(*pobj)->AddRef();
	if (pInst) *pInst = pt;
}

#define QI_IF(iid, ppv, the_iid, Type) if (iid==the_iid){ Type * t = static_cast<Type*>(this); *ppv=t; t->AddRef(); return S_OK; }
#define IMPLEMENT_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

inline HRESULT dllCreateObj(HMODULE mod, REFCLSID rclsid, REFIID riid, void ** ppvObj)
{
	HRESULT (STDAPICALLTYPE *proc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 
	(FARPROC&)proc = GetProcAddress(mod, "DllGetClassObject");
	if (proc)
	{
		IClassFactory* pfact = NULL;
		HRESULT hr = proc(rclsid, IID_IClassFactory, (void**)&pfact);
		if (SUCCEEDED(hr) && pfact)
		{
			hr = pfact->CreateInstance(NULL, riid, ppvObj);
			pfact->Release();
		}
		return hr;
	}
	return E_FAIL;
}

inline HRESULT dllCreateObj(REFCLSID rclsid, REFIID riid, void ** ppvObj)
{
	CComBSTR name(rclsid);
	CRegKey key;
	CString skey;
	skey.Format(L"CLSID\\%s\\InprocServer32", name.m_str);
	key.Open(HKEY_CLASSES_ROOT, skey, KEY_READ);

	TCHAR value[300] = {0};
	ULONG nv = _countof(value);
	key.QueryStringValue(NULL, value, &nv);
	HMODULE mod = 0;

	//TCHAR * pname = wcsrchr(value, '\\');
	//if (pname)
	//{
	//	mod = GetModuleHandle(pname+1);
	//}
	mod = LoadLibrary(value);
	return dllCreateObj(mod, rclsid, riid, ppvObj);
}

class CDISPOP
{
public:
	int m_argc;
	VARIANT * m_argv;
private:
	int m_idx;
	HRESULT m_hr;
	std::vector<VARIANT> m_vars;
public:
	CDISPOP(DISPPARAMS * arg)
	{
		m_argc = arg->cArgs - arg->cNamedArgs;
		m_argv = arg->rgvarg + arg->cNamedArgs;
		m_idx = 0;
		m_hr = S_OK;
	}
	~CDISPOP()
	{
		for (size_t i=0; i<m_vars.size(); ++i) VariantClear(&m_vars[i]);
		m_vars.clear();
	}
	HRESULT res(){return m_hr;}
public:
	operator bool()
	{
		VARIANT ret; ret.vt = VT_EMPTY;
		bool defret = false;
		VARIANT * r = m_argv+m_idx++;
		if (FAILED(m_hr)) return defret;
		if (m_idx>=m_argc) {m_hr = E_INVALIDARG; return defret;}
		m_hr = VariantChangeType(&ret, r, 0, VT_BOOL);
		if (FAILED(m_hr)) return defret;
		m_vars.push_back(ret);
		return !! ret.boolVal;
	}
	operator BSTR()
	{
		VARIANT ret; ret.vt = VT_EMPTY;
		BSTR defret = 0;
		VARIANT * r = m_argv+m_idx++;
		if (FAILED(m_hr)) return defret;
		if (m_idx>m_argc) {m_hr = E_INVALIDARG; return defret;}
		m_hr = VariantChangeType(&ret, r, 0, VT_BSTR);
		if (FAILED(m_hr)) return defret;
		m_vars.push_back(ret);
		return ret.bstrVal;
	}
	operator int ()
	{
		VARIANT ret; ret.vt = VT_EMPTY;
		int defret = 0;
		VARIANT * r = m_argv+m_idx++;
		if (FAILED(m_hr)) return defret;
		if (m_idx>=m_argc) {m_hr = E_INVALIDARG; return defret;}
		m_hr = VariantChangeType(&ret, r, 0, VT_INT);
		if (FAILED(m_hr)) return defret;
		m_vars.push_back(ret);
		return ret.intVal;
	}
	operator long long ()
	{
		VARIANT ret; ret.vt = VT_EMPTY;
		long long defret = 0;
		VARIANT * r = m_argv+m_idx++;
		if (FAILED(m_hr)) return defret;
		if (m_idx>=m_argc) {m_hr = E_INVALIDARG; return defret;}
		m_hr = VariantChangeType(&ret, r, 0, VT_I8);
		if (FAILED(m_hr)) return defret;
		m_vars.push_back(ret);
		return ret.llVal;
	}
	operator double ()
	{
		VARIANT ret; ret.vt = VT_EMPTY;
		double defret = 0.0;
		VARIANT * r = m_argv+m_idx++;
		if (FAILED(m_hr)) return defret;
		if (m_idx>=m_argc) {m_hr = E_INVALIDARG; return defret;}
		m_hr = VariantChangeType(&ret, r, 0, VT_R8);
		if (FAILED(m_hr)) return defret;
		m_vars.push_back(ret);
		return ret.dblVal;
	}
};
