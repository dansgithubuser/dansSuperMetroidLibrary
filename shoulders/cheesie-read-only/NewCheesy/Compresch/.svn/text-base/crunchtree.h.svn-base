/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */


#ifndef			__CRUNCHTREE_H__
#define			__CRUNCHTREE_H__

#include "sizelimits.h"


struct CRUNCH
{
	Block*		blk;
	int			start;
	int			stop;
	int			bytes;

	int			padstop;	// stop after some raw padding
	int			padbytes;	// bytes after raw padding

	int			deep;
};

class CrunchNode;
typedef std::list<CrunchNode*>		NodeList;
typedef NodeList::iterator			NodeIter;

class CrunchNode
{
public:
	CrunchNode(CrunchNode* parent)
	{
		up =			parent;
		dat.blk =		0;
		dat.start =		0;
		dat.stop =		0;
		dat.bytes =		0;
		dat.padstop =	0;
		dat.padbytes =	0;
		dat.deep =		0;
	}
	~CrunchNode()
	{
		while(!dn.empty())
		{
			delete dn.front();
			dn.pop_front();
		}
	}

	CrunchNode*		up;
	NodeList		dn;
	CRUNCH			dat;
};


class CrunchTree
{
    
public:
    
	CrunchTree();
	~CrunchTree();

	void			Crunch(List& lst,const SIZELIMITS* typelimits,const SIZELIMITS* rawlimits,int endofdata);
	void			Destroy();
	
	bool Test(List &lst)
	{
	    bool problem = false;
	    
	#if(DESPERATE)
	    for(iter i = lst.begin(); i != lst.end(); ++i)
	    {
	        Block *blk = (*i);
	        
	        if(blk->len == 1)
	        {
	            problem = true;
	        }
	    }
	#endif
	    
	    return problem;
	}

protected:

	CrunchNode*		top;
	NodeList		ends;
	
	const SIZELIMITS*	pTypeLimits;
	const SIZELIMITS*	pRawLimits;

	int					CalcBlockBytes(Block* blk);
	int					CalcRawBytes(int len);

	void				PadAndCleanChains(int start);
	void				ChainBlock(Block* blk);
	void				BuildBestList(List& lst,CrunchNode* node);

	inline void			AddNode(CrunchNode* parent,CRUNCH& cr);
};

#endif