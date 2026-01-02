#pragma once

/*/////// usage example //////////////////////////
	static const SIZES2::ItemRect s_szs[] =
	{
		//rela, out, w, h, {margin:l t r b}
		{-1, 1, -1, -1, {2,2,2,25}},
		{-1, 1, 80,23, {2,2}},
		{-1, 0, -1,23, {2,2,172}},
		{-1, 1, 80,23, {2,2}},
		{-1, 1, 80,23, {2+6,2}},
	};

	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(2,2,2,1);
	RECT out[_countof(s_szs)];
	SIZES2::GetSizes(s_szs, _countof(s_szs), rc, out, 0);

	std::vector<HWND> vh = SIZES2::getChildren(m_hWnd);
	for (size_t i=0,mi=vh.size(); i<mi; ++i)
	{
		::MoveWindow(vh[i], out[i].left, out[i].top, out[i].right-out[i].left, out[i].bottom-out[i].top, false);
	}
	InvalidateRect(NULL, true);
///////////////*//////////////////////////////////
namespace SIZES2
{
	struct ItemMargin
	{
		USHORT l, t, r, b;
	};
	 //TODO:bit mask: 4:for percentage width. 8for percentage h. 16 for callback.
	enum FLAGS
	{
		SZFL_OUTITEM=1,
		SZFL_VIRTUAL=2,
		SZFL_W_PERCENT=4,
		SZFL_H_PERCENT=8,
		SZFL_CALLBACK=16,
	};
	struct ItemRect
	{
		USHORT rela;
		USHORT flags;
		USHORT w,h;
		ItemMargin margin;
		void * userdata;
		void * callback;
	};
	struct ItemTmpData
	{
		RECT rc;
		int h,maxh;
		int w;
	};
	enum {MAXVALUE = 0xffff};

	inline void GetSizes(const ItemRect * ss, int count, const RECT & parentrc, ItemTmpData * td)
	{
		int ww_1, hh_1;
		ItemTmpData root;
		root.rc = parentrc;
		root.h=root.maxh=root.w=0;
		ItemTmpData * sel = &root;

		for (int i=0; i<count; ++i)
		{
			sel = (ss[i].rela == MAXVALUE) ? &root : td+ss[i].rela;
			if (ss[i].w==MAXVALUE)
				ww_1 = sel->rc.right - sel->rc.left - sel->w - ss[i].margin.r;
			else
				ww_1 = ss[i].margin.l + ss[i].w;
			if (ss[i].h==MAXVALUE)
				hh_1 = sel->rc.bottom - sel->rc.top - sel->h - ss[i].margin.b;
			else
				hh_1 = ss[i].margin.t + ss[i].h;
				
			if ((ww_1 > sel->rc.right - sel->rc.left - sel->w - ss[i].margin.r || ww_1<0) && sel->w > 0)
			{
				sel->w = 0;
				sel->h += sel->maxh;
				sel->maxh = 0;
				--i;
				continue;
			}
			RECT & outrect = td[i].rc;
			outrect.left   = sel->rc.left + sel->w + ss[i].margin.l;
			outrect.right  = outrect.left + ww_1 - ss[i].margin.l;
			outrect.top    = sel->rc.top + sel->h + ss[i].margin.t;
			outrect.bottom = outrect.top + hh_1 - ss[i].margin.t;
			sel->w = outrect.right - sel->rc.left;
			if (sel->maxh < hh_1) sel->maxh = hh_1;
			if (sel->w + sel->rc.left >= sel->rc.right)
			{
				sel->w = 0;
				sel->h += sel->maxh;
				sel->maxh = 0;
			}
		}
	}
	inline int GetSizes(const ItemRect * ss, int count, const RECT & parentrc, RECT * out, RECT * virtual_)
	{
		ItemTmpData * td = (ItemTmpData*) _alloca(count * sizeof(ItemTmpData));
		memset(td, 0, count * sizeof(ItemTmpData));
		GetSizes(ss, count, parentrc, td);
		int r = 0;
		for (int i=0,j=0; i<count; ++i)
		{
			if (ss[i].flags & SZFL_OUTITEM)
			{
				out[j++] = td[i].rc;
				++r;
			}
			if (ss[i].flags & SZFL_VIRTUAL)
			{
				*virtual_++ = td[i].rc;
			}
		}
		return r;
	}
	constexpr USHORT RELA_PARENT = (USHORT)-1;
	constexpr USHORT AUTO_WH = (USHORT)-1;
#define SIZES2_USELINE {SIZES2::RELA_PARENT,0,(USHORT)-1,0,{0,0,0,0}}
#define SIZES2_HLINE(h) {SIZES2::RELA_PARENT,0,(USHORT)-1,h}

	inline std::vector<HWND> getChildren(HWND hParent)
	{
		std::vector<HWND> vh;
		HWND hwnd1 = ::GetWindow(hParent, GW_CHILD);
		while (hwnd1)
		{
			vh.push_back(hwnd1);
			hwnd1 = ::GetNextWindow(hwnd1, GW_HWNDNEXT);
		}
		return vh;
	}
};

class CDlgSizes2
{
public:
	SIZES2::ItemRect* m_rc;
	int m_nrc;
	uint16_t m_dpi;
	bool m_inited = false;

	CDlgSizes2(const SIZES2::ItemRect* irc, size_t nrc)
	{
		m_rc = (SIZES2::ItemRect*)irc;
		m_nrc = (int)nrc;
	}
	~CDlgSizes2() {
		if (m_inited) {
			free(m_rc);
			m_rc = 0;
		}
	}
	int OnSize(HWND hWnd)
	{
		if (!m_inited) {
			init(hWnd);
		}
		RECT rc, * out = (RECT*)_alloca(m_nrc * sizeof(RECT));
		::GetClientRect(hWnd, &rc);
		rc.left += 5;
		rc.top += 5;
		rc.bottom -= 5;
		rc.right -= 5;
		SIZES2::GetSizes(m_rc, m_nrc, rc, out, 0);

		::LockWindowUpdate(hWnd);
		vector<HWND> vh = SIZES2::getChildren(hWnd);
		for (size_t i = 0, mi = vh.size(); i < mi; ++i)
		{
			::MoveWindow(vh[i], out[i].left, out[i].top, out[i].right - out[i].left, out[i].bottom - out[i].top, false);
		}
		::LockWindowUpdate(0);
		::InvalidateRect(hWnd, NULL, true);
		return 1;
	}
protected:
	void init(HWND hWnd) {
		auto np = (SIZES2::ItemRect*)malloc(sizeof(SIZES2::ItemRect) * m_nrc);
		if (!np) return;
		memcpy(np, m_rc, sizeof(SIZES2::ItemRect) * m_nrc);
		HDC hDC = ::GetDC(hWnd);
		m_dpi = (uint16_t)GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC(hWnd, hDC);
		m_inited = true;
		if (m_dpi != 96)
		{
			for (int i = 0; i < m_nrc; ++i)
			{
				if (np[i].w != SIZES2::MAXVALUE) np[i].w = np[i].w * m_dpi / 96;
				if (np[i].h != SIZES2::MAXVALUE) np[i].h = np[i].h * m_dpi / 96;
				if (np[i].margin.l) np[i].margin.l = np[i].margin.l * m_dpi / 96;
				if (np[i].margin.t) np[i].margin.t = np[i].margin.t * m_dpi / 96;
				if (np[i].margin.r) np[i].margin.r = np[i].margin.r * m_dpi / 96;
				if (np[i].margin.b) np[i].margin.b = np[i].margin.b * m_dpi / 96;
			}
		}
		m_rc = np;
	}
};
