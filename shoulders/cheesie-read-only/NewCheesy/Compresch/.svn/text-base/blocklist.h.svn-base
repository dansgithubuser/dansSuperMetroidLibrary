/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

#ifndef			__BLOCKLLIST_H__

    #define			__BLOCKLLIST_H__
	#include "sizelimits.h"

    typedef		std::list<Block*>			List;
    typedef		List::iterator				iter;

    class BlockList
    {
    public:
                            BlockList();
                            ~BlockList();

        void				InsertBlock(Block* blk);
        void				EmptyInternalList();

        Block*				PopFront();
        
        static void			EmptyList(List& lst);

        void				CrunchList(const SIZELIMITS* typelimits,const SIZELIMITS* rawlimits,int endofdata);
        
        bool Test()
        {
            bool problem = false;
            
        #if(DESPERATE)
            for(iter d = mLst.begin(); d != mLst.end(); ++d)
            {
                Block *blk = (*d);
                
                if(blk->len == 1)
                {
                    problem = true;
                }
            }
        #endif
            
            return problem;
        }

    protected:
        List				mLst;
        void				CopyList(List& dest,List& src);

        void				KillPointlessBlocks();
    };

#endif