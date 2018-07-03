    /*
     *    Compresch - compression library
     *  Copyright 2008 Disch
     *  See license.txt for details
     */
    
    #include <list>
    
    #include "compresch_types.h"
    
    #include "block.h"
    #include "blocklist.h"
    #include "crunchtree.h"

    CrunchTree::CrunchTree()
    {
        top = 0;
    }

    CrunchTree::~CrunchTree()
    {
        Destroy();
    }

    void CrunchTree::Destroy()
    {
        if(top)
        {
            delete top;
        }

        top = 0;
        ends.clear();
    }

    int CrunchTree::CalcBlockBytes(Block* blk)
    {
        int coarse = blk->len / pTypeLimits[blk->type].lenmax;
        int fine   = blk->len % pTypeLimits[blk->type].lenmax;
        
        // ---------------------------------
        
        coarse *= blk->GetBodySize() + blk->HasExtraByte() + pTypeLimits[blk->type].blocksize + 1;
        
        if(fine)
        {
            coarse += blk->GetBodySize() + blk->HasExtraByte() + pTypeLimits[blk->type].blocksize + (fine > pTypeLimits[blk->type].extrabyteafter);
        }
        
        return coarse;
    }

    int CrunchTree::CalcRawBytes(int len)
    {
        int coarse = len / pRawLimits->lenmax;
        int fine   = len % pRawLimits->lenmax;
        
        // --------------------------------------
        
        coarse *= pRawLimits->lenmax + pRawLimits->blocksize + 1;
        
        if(fine)
        {
            coarse += fine + pRawLimits->blocksize + (fine > pRawLimits->extrabyteafter);
        }
        
        return coarse;
    }

    void CrunchTree::Crunch(List& lst,const SIZELIMITS* typelimits,const SIZELIMITS* rawlimits,int endofdata)
    {
        // empty list is empty
        if(lst.empty())
            return;
        
        pTypeLimits = typelimits;
        pRawLimits  = typelimits;
        
        // clear the tree (if one exists -- it shouldn't, but just in case)
        Destroy();
        
        // build tree from top down!
        top = new CrunchNode(0);
        ends.push_back(top);
        
        iter i;
        
        for(i = lst.begin(); i != lst.end(); ++i)
        {
            // pad and clean chains up to this block's starting point
            PadAndCleanChains((*i)->start);
            
            // then try putting this block in the chains
            ChainBlock(*i);
            
            Test(lst);
        }
        
        // after you do that with all the blocks, pad them to the end of data and clean
        PadAndCleanChains(endofdata);
        
        Test(lst);
        
        // here, search 'ends' for the best chain
        NodeIter j    = ends.begin();
        NodeIter best = j;

        for(++j; j != ends.end(); ++j)
        {
            if((*j)->dat.padbytes < (*best)->dat.padbytes)
                best = j;
            else if((*j)->dat.padbytes == (*best)->dat.padbytes)
            {
                if((*j)->dat.deep < (*best)->dat.deep)
                    best = j;
            }
        }
        
        Test(lst);
        
        List bestList;
        BuildBestList(bestList, *best);
        
        Test(bestList);

        // bestlist is now compiled -- kill the original list and replace it
        BlockList::EmptyList(lst);
        lst.swap(bestList);
        
        Test(lst);
        
        // and that's all she wrote!
    }

    void Advance(NodeIter &i, NodeList &list)
    {
        if(i != list.end())
        {
            ++i;
        }
    }

    void CrunchTree::PadAndCleanChains(int start)
    {
        // this function takes the given starting point and checks it against each chain's ending point
        //   since the block list is stored in ascending order of starting points -- once we reach any given
        //   starting point... any chains that are not to the current starting point can only reach the point
        //   with a raw block.
        //
        // since raw blocks are big and ugly, this is where most chains will reach their dead end and be removed
        //   from the 'ends' list.
        
        NodeIter i, j;
        
        // first... pad them all to starting point
        for(i = ends.begin(); i != ends.end(); ++i)
        {
            if((*i)->dat.padstop >= start)        // does not need padding
                continue;
            
            (*i)->dat.padbytes = (*i)->dat.bytes + CalcRawBytes(start - (*i)->dat.stop) - 1;
            (*i)->dat.padstop  = start;
        }


        // now run through and clean bad chains
        //  a chain is bad if another chain reaches as far or further in as many or fewer bytes

        for
        (
            i = ends.begin();
            i != ends.end();
            Advance(i, ends)
        )
        {
            for(j = ends.begin(); (j != i) && (j != ends.end()); Advance(j, ends))
            {
                if((*i)->dat.stop == (*j)->dat.stop)
                {
                    if((*i)->dat.bytes < (*j)->dat.bytes)
                    {
                        j = ends.erase(j);
                        
                        if(j != ends.begin()) { --j; }
                    }
                    else if((*j)->dat.bytes < (*i)->dat.bytes)
                    {
                        i = ends.erase(i);
                        
                        if(i != ends.begin()) { --i; }
                        
                        break;
                    }
                    else
                    {
                        if((*i)->dat.deep <= (*j)->dat.deep)
                        {
                            j = ends.erase(j);
                            
                            if(j != ends.begin()) { --j; }
                        }
                        else
                        {
                            i = ends.erase(i);
                            
                            if(i != ends.begin()) { --i; }
                            
                            break;
                        }
                    }
                }
                else
                {
                    if((*i)->dat.padstop != (*j)->dat.padstop)        continue;
                    
                    // Debug code
                    // {
                    if(ends.empty())
                        continue;
                    else if(i == ends.end())
                        continue;
                    else if(j == ends.end())
                        continue;
                    
                    CrunchNode *c = (*j);
                    
                    if(!c->dat.blk)
                    {
                        j = ends.erase(j);
                        
                        if(j != ends.begin()) { --j; }
                        
                        continue;
                    }
                    
                    int blockType = c->dat.blk->type;
                    
                    if((*i)->dat.padbytes <= ((*j)->dat.padbytes - pTypeLimits[blockType].blocksize - 1))
                    {
                        j = ends.erase(j);
                        
                        if(j != ends.begin()) { --j; }
                    }
                    else if((*j)->dat.padbytes <= ((*i)->dat.padbytes - pTypeLimits[blockType].blocksize - 1))
                    {
                        i = ends.erase(i);
                        
                        if(i != ends.begin()) { --i; }
                        
                        break;
                    }
                }
            }
        }
    }

    void CrunchTree::ChainBlock(Block* blk)
    {
        // this is where we try putting 'blk' in each chain (or rather where each chain is split by 'blk')
        // note we only have to test against existing chains
        
        Block* tmp;
        
        CRUNCH cr;
        int rem;
        
        cr.blk =        blk;
        
        NodeIter i;
        
        for(i = ends.begin(); i != ends.end(); ++i)
        {
            // does this chain go further than this block does?  If so -- no point in adding this block to
            //  the chain
            if((*i)->dat.padstop >= blk->stop)        continue;

            // copy the block
            tmp = blk->Dup();

            // does this chain go *into* this block?  If so -- try pushing this block forward so it starts where
            //  the chain left off
            if((*i)->dat.padstop > blk->start)
            {
                if(tmp->Shrink((*i)->dat.padstop,tmp->stop))        // shrink failed, can't add to this chain
                {
                    delete tmp;
                    continue;
                }
            }
            cr.start =    tmp->start;

            // now we're going to potentially fork several chains here.  There are many ways we need to attempt to apply
            // this block:
            //  
            //    1)  In full (full len)
            //    2)  Just below extra byte boundaries (assuming it's above those bounds)
            //    3)  Just below extra block boundaries (assuming it's above those)

            //  testing for all 3 can be done in this neat little while loop:

            while(tmp->len > 0)
            {
                // in full, or just below extra block bound
                cr.stop  = tmp->stop;
                cr.bytes = CalcBlockBytes(tmp) + (*i)->dat.padbytes;
                
                AddNode(*i,cr);            // AddNode adds to the front of the list, so as not to disturb this for loop
                
                rem = tmp->len % pTypeLimits[tmp->type].lenmax;
                if(!rem)        rem = pTypeLimits[tmp->type].lenmax;
                
                if(rem > pTypeLimits[tmp->type].extrabyteafter)
                {
                    // just below extra byte bound
                    tmp->DropLen(tmp->len - rem + pTypeLimits[tmp->type].extrabyteafter);
                    cr.stop =    tmp->stop;
                    cr.bytes =    CalcBlockBytes(tmp) + (*i)->dat.padbytes;
                    AddNode(*i,cr);
                    
                    rem = tmp->len % pTypeLimits[tmp->type].lenmax;
                    
                    if(!rem)
                    {
                        rem = pTypeLimits[tmp->type].lenmax;
                    }
                }
                
                tmp->DropLen(tmp->len - rem);
            }
            
            delete tmp;
        }
    }

    void CrunchTree::AddNode(CrunchNode* parent,CRUNCH& cr)
    {
        CrunchNode* nd = new CrunchNode(parent);
        nd->dat = cr;
        nd->dat.padbytes = nd->dat.bytes;
        nd->dat.padstop = nd->dat.stop;
        nd->dat.deep = parent->dat.deep + 1;

        parent->dn.push_front(nd);
        ends.push_front(nd);
    }

    void CrunchTree::BuildBestList(List& lst,CrunchNode* node)
    {
        Block* blk;
        Block* tmp;
        int rem;

        while(node)
        {
            if(node->dat.blk)
            {
                blk = node->dat.blk->Dup();
                blk->Shrink(node->dat.start,node->dat.stop);

                while(blk->len > pTypeLimits[blk->type].lenmax)
                {
                    rem = blk->len % pTypeLimits[blk->type].lenmax;
                    if(!rem)    rem = pTypeLimits[blk->type].lenmax;

                    tmp = blk->Dup();

                    blk->DropLen(blk->len - rem);
                    tmp->Shrink(blk->stop,tmp->stop);

                    lst.push_front(tmp);
                }
                
                if( blk->len >= blk->GetMinLength() )
                {
                    lst.push_front(blk);
                }
            }

            node = node->up;
        }
    }
