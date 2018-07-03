#include <wx/dc.h>
class           BackBuffer
{
public:
	BackBuffer();
	~BackBuffer();

	void            Create(int width, int height);
	void            Destroy();

	void            Clear(unsigned long clr = RGB(0, 0, 0));
	wxDC             DC()
	{
		return mDC;
	}
	int             GetWd()
	{
		return nWidth;
	}
	int             GetHt()
	{
		return nHeight;
	}
	void            SaveToFile(FILE * file);
protected:
	void            Zero();
	BITMAPINFO      bi;
	HDC             mDC;
	HBITMAP         hBmp;
	HBITMAP         hBmpOld;

	int             nWidth;
	int             nHeight;
	int             nPitch;
};
