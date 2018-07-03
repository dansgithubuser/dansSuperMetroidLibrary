
#ifndef _glw_smart_pointer_hdr
#define _glw_smart_pointer_hdr

// For _ASSERT
#ifdef _DEBUG
#include <CRTDBG.H>
#else  // _DEBUG
#ifndef _ASSERT
#define _ASSERT(b) b
#endif
#endif  // _DEBUG
#include <glw/glwGlobals.h>

template <class T>
class glwSmartPtr
{
private:
	T* _ptr;
	unsigned long _size;
	unsigned long _allocSize;
	float _growinc;  // Amount the array will grow each time it runs out of space.
	float _growexp;  // Makes array grow exponentially. Multiplier for _growinc every time array needs more space. Applied after alloc.
	//T garbage;
	void init() { _ptr = NULL; _size = 0; _allocSize = 0; _growinc = 10; _growexp = 1.5f; }
public:
	void clear() { if (_ptr) delete [] _ptr; init(); }
	glwSmartPtr() { init(); }
	glwSmartPtr(glwSmartPtr& other);
	glwSmartPtr& operator=(glwSmartPtr& other);
	~glwSmartPtr() { if (_ptr) delete [] _ptr; }
	T* operator * () { return _ptr; }
	T* operator -> () { return _ptr; }
	T& operator [] (size_t n);
	long size() { return _size; }
	long memoryUsage() { return _allocSize * sizeof(T); }
	T* resize(unsigned long newSize); // May or may not shrink the actual allocated size if new size is smaller, but ensures there is enough room if larger. Returns NULL on fail or a pointer to the new array.
	long setGrowthIncrement(long value) { if (value > 0) _growinc = value; }
	long setGrowthExponent(float value) { if (value >= 1.0f) _growexp = value; }
};

template<class T>
glwSmartPtr<T>::glwSmartPtr(glwSmartPtr<T>& other)
{
	init();
	_ptr = new T[other.size()];
	_allocSize = _size = other.size();
	for (int i = 0; i < other.size(); i++)
	{
		_ptr[i] = other._ptr[i];
	}
}

template <class T>
glwSmartPtr<T>& glwSmartPtr<T>::operator=(glwSmartPtr<T>& other)
{
	clear();
	_ASSERT(NULL != (_ptr = new T[other.size()]));
	_allocSize = _size = other.size();
	for (int i = 0; i < other.size(); i++)
	{
		_ptr[i] = other._ptr[i];
	}
}

template <class T>
T& glwSmartPtr<T>::operator [] (size_t n)
{
	if (n >= _allocSize)
	{
		_ASSERT(resize(n + 1) != NULL);
	}
	//if (n > _size - 1) _size = n + 1;  // resize() handles this
	return _ptr[n];
}

template <class T>
T* glwSmartPtr<T>::resize(unsigned long newSize)
{
	T* tp;
	unsigned long tas;
	unsigned long tsm;
	if (newSize > _allocSize)
	{
		if (_growinc > (newSize - _allocSize))
			tas = _allocSize + (long)_growinc;
		else
			tas = newSize;
		tp = new T[tas];
		if (!tp) return 0;
		//memcpy(tp, _ptr, _size * sizeof(T));
		glwCopyArray(_ptr, tp, _size);
		delete [] _ptr;
		_ptr = tp;
		_growinc *= _growexp;
		_allocSize = tas;
	}
	else
	{
		tsm = (unsigned long)(_allocSize / _growexp);
		if (newSize < tsm)
		{
			tp = new T[newSize];
			if (tp)
			{
				//memcpy(tp, _ptr, newSize * sizeof(T));
				glwCopyArray(_ptr, tp, newSize);
				// TODO: MSVC6: CRT debug assertion failed on delete
				delete [] _ptr;
				_ptr = tp;
				_allocSize = newSize;
				_growinc = _allocSize / _growexp;
				if (_growinc < 1) _growinc = 1;
			}
			else return 0;
		}
	}
	_size = newSize;
	return _ptr;
}

#endif // header

