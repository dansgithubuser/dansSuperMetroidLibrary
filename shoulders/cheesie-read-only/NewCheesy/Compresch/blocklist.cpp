/*
 *	Compresch - compression library
 *  Copyright 2008 Disch
 *  See license.txt for details
 */

    #include <vector>
    #include <list>
    #include "compresch_types.h"
    #include "block.h"
    #include "blocklist.h"
    
    #include "crunchtree.h"

    BlockList::BlockList()
    {
        
    }

// **************************************

    BlockList::~BlockList()
    {
        EmptyInternalList();
    }

// **************************************

    void BlockList::EmptyInternalList()
    {
        EmptyList(mLst);
    }

// **************************************

    void BlockList::EmptyList(List& lst)
    {
        while(!lst.empty())
        {
            delete lst.front();
            lst.pop_front();
        }
    }

// **************************************

    Block* BlockList::PopFront()
    {
        if(mLst.empty())
            return 0;

        Block* ret = mLst.front();
        mLst.pop_front();
        return ret;
    }

// **************************************

    void BlockList::CopyList(List& dest,List& src)
    {
        if(src.empty())
        {
            return;
        }
        
        iter i;
        
        for(i = src.begin(); i != src.end(); ++i)
            dest.push_back((*i)->Dup());
    }

// **************************************

    void BlockList::InsertBlock(Block* blk)
    {
        iter i;
        
        // -------------------------------
        
        if(mLst.empty())
        {
            mLst.push_back(blk);
            
            return;
        }
        
        for(i = mLst.begin(); i != mLst.end(); ++i)
        {
            if(blk->start <= (*i)->start)
                break;
        }
	    
        mLst.insert(i,blk);
    }

// **************************************

    void BlockList::CrunchList(const SIZELIMITS* typelimits,const SIZELIMITS* rawlimits,int endofdata)
    {
        KillPointlessBlocks();
        
        CrunchTree tree;

	    tree.Crunch(mLst, typelimits, rawlimits, endofdata);
	    
        Test();
    }

// **************************************

    void BlockList::KillPointlessBlocks()
    {
        // block A is truely pointless if another block B covers the same area equally.
        // 
        // This is true when all of the following are true:
        //      
        //    1)  A begins and ends inside B
        //    2)  B can be moved to start at the same point A starts at
        //    3)  B's body size is less than or equal to A's
        //
        // if any of those are false, then A may still be needed
        
        iter i, j;
        
        // ----------------------------------
        
        for(i = mLst.begin(); i != mLst.end(); ++i){
            for(j = mLst.begin(); j != mLst.end(); ++j){
                if(i == j) continue;
                else if((*j)->start >= (*i)->stop) break;// j has passed up what i covers, no need to continue
                // now see if i makes j pointless
                if((*j)->start <  (*i)->start) continue;// j starts before i, not pointless
                else if((*j)->stop  >  (*i)->stop) continue;// j ends after i
                else if(
									(*j)->GetBodySize()+(*j)->HasExtraByte()
                  <
									(*i)->GetBodySize()+(*j)->HasExtraByte()
                ) continue;// j has smaller body than i
                else if(!(*i)->CanShrink((*j)->start,(*i)->stop)) continue;// i can't start where j starts
                // if we got here, j is pointless!
                delete *j;
                j=mLst.erase(j);
                if(j!=mLst.begin()) --j;
            }
        }
    }

// ***********************************************************
