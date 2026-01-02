#pragma once

enum
{
	TAB_1 = 1,
	TAB_2,
	TAB_3,
	TAB_4,
	TAB_5,
	TAB_6,
	TAB_7,
	TAB_8,
	TAB_9,
	TAB_10,
	TAB_11,
	TAB_12,
	TAB_13,
	TAB_14,
	TAB_15,
	TAB_16,
	TAB_17,
	TAB_18,
	TAB_19,
};
namespace SIZES
{
	struct ItemPos
	{
		WORD id; //-1:absolute. 0:parent,else:id
		short pos;//position.
	};
	struct ItemSize
	{
		WORD id;
		short width;  //-1 for compute.else use this one.
		short height; //-1 for compute.
/*		union
		{
			struct 
			{
				ItemPos left;
				ItemPos top;
				ItemPos right;
				ItemPos bottom;
			};
			ItemPos pos[4];
		};*/
		ItemPos pos[4];
	};

	inline void ComputeIt(ItemSize * ss,int count,const RECT & parentrect,int indx,char * masks,short * itemposs[4])
	{
		if(masks[indx]) return; //already computed.
		masks[indx] = true; //mask it computed.
		short p[4] = {(short)parentrect.left,(short)parentrect.top,(short)parentrect.right,(short)parentrect.bottom};

		for(int which=0;which<4;++which)
		{
			switch(ss[indx].pos[which].id)
			{
			case -1://absolute.
				itemposs[which][indx] = ss[indx].pos[which].pos;
				break;
			case 0:
				itemposs[which][indx] = ss[indx].pos[which].pos + p[which];
				break;
			default:
				{
					short _indx = -1;
					const short _id = ss[indx].pos[which].id;
					for(int i=0;i<count;++i)
					{
						if(ss[i].id == _id)
						{
							_indx = i;
							break;
						}
					}
					if(_indx == -1 || _indx == indx)
					{
						itemposs[which][indx] = ss[indx].pos[which].pos;
					}
					else
					{
						ComputeIt(ss,count,parentrect,_indx,masks,itemposs);
						itemposs[which][indx] = ss[indx].pos[which].pos + itemposs[which][_indx];
					}
				}
				break;
			}
		}
		if(ss[indx].width > 0)
			itemposs[2][indx] = itemposs[0][indx] + ss[indx].width;
		else if(ss[indx].width < 0)
			itemposs[0][indx] = itemposs[2][indx] + ss[indx].width;

		if(ss[indx].height > 0)
			itemposs[3][indx] = itemposs[1][indx] + ss[indx].height;
		else if(ss[indx].height < 0)
			itemposs[1][indx] = itemposs[3][indx] + ss[indx].height;
	}

	inline void GetSizes(ItemSize * ss,int count,const RECT & parentrect,RECT * outrect)
	{
		char * masks = new char [count];
		typedef short xx[4];
		short* itemposs[4];
		itemposs[0] = new short [count];
		itemposs[1] = new short [count];
		itemposs[2] = new short [count];
		itemposs[3] = new short [count];

		memset(masks,0,count);
		for(int i=0;i<count;++i)
		{
			ComputeIt(ss,count,parentrect,i,masks,itemposs);
		}
		delete [] masks;
		for(int i=0;i<count;++i)
		{
			outrect[i].left   = itemposs[0][i];
			outrect[i].top    = itemposs[1][i];
			outrect[i].right  = itemposs[2][i];
			outrect[i].bottom = itemposs[3][i];
		}
		delete [] itemposs[0];
		delete [] itemposs[1];
		delete [] itemposs[2];
		delete [] itemposs[3];
	}

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
