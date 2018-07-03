
#ifndef _realloc_h_
#define _realloc_h_

#ifndef REALLOC_SIZE
#define REALLOC_SIZE 10
#endif

template <class T> 
int glwRealloc(T** ppBuffer, int requestedSize, int currentSize, int copySize)
{
	int allocSize;
	T* ptr;
	int i;
	if (requestedSize >= currentSize)
	{
		ptr = new T[requestedSize + REALLOC_SIZE];
		allocSize = requestedSize + REALLOC_SIZE;
		// Warning: do not substiture memcpy for this loop. 
		// memcpy() will not call copy constructors like this will.
		for (i = 0; i < copySize; i++)
		{
			ptr[i] = (*ppBuffer)[i];
		}
		delete [] *ppBuffer;
		*ppBuffer = ptr;
	}
	else 
	{
		allocSize = currentSize;
	}
	return allocSize;
}


#endif