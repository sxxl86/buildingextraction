
// dib.h

#ifndef _INC_DIB
#define _INC_DIB

/* DIB constants */
#define PALVERSION   0x300

/* Dib Header Marker - used in writing DIBs to files */
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/* DIB Macros*/
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

class CDib : public CObject
{
	DECLARE_DYNAMIC(CDib)

// Constructors
public:
	CDib();

// Attributes
protected:
	
public:	
	LPBYTE m_pBits;
	LPBITMAPINFO m_pBMI;
	CPalette* m_pPalette;

public:
	DWORD Width()     const;
	DWORD Height()    const;
	WORD  NumColors() const;
	BOOL  IsValid()   const { return (m_pBMI != NULL); }

// Operations
public:
	BOOL  Paint(HDC, LPRECT, LPRECT) const;
	HGLOBAL CopyToHandle()           const;
	DWORD Save(CFile& file)          const;
	DWORD Read(CFile& file);
	DWORD ReadFromHandle(HGLOBAL hGlobal);
	LPBYTE RETURN()
	{
		return m_pBits;
	}
	LPBYTE DD(LPBYTE m)
	{
		m_pBits = m;
		return m_pBits;
	}
	void Invalidate() { Free(); }

	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CDib();

protected:
	BOOL  CreatePalette();
	WORD  PaletteSize() const;
	void Free();

public:
	RECT matchedArea;
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CDib& operator = (CDib& dib);
};

#endif //!_INC_DIB
