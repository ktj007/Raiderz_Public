#ifndef _DDSFILE_H
#define _DDSFILE_H

// from d3dx9types.h
const DWORD DDS_FMT_R32F = 114;

/**
 ddraw.h 의 상수 정의와 동일. 중복 방지를 위해 #ifndef ~ #endif 를 사용하였습니다.
*/


/*
* The RGB data in the pixel format structure is valid.
*/
#ifndef DDPF_RGB
#define DDPF_RGB                                0x00000040l
#endif

/*
* The surface has alpha channel information in the pixel format.
*/
#ifndef DDPF_ALPHAPIXELS
#define DDPF_ALPHAPIXELS                        0x00000001l
#endif

/*
* The FourCC code is valid.
*/
#ifndef DDPF_FOURCC
#define DDPF_FOURCC                             0x00000004l
#endif

/*
* Indicates that this surface contains alpha-only information.
* (To determine if a surface is RGBA/YUVA, the pixel format must be
* interrogated.)
*/
#ifndef DDSCAPS_ALPHA
#define DDSCAPS_ALPHA                           0x00000002l
#endif

/*
* Indicates that this surface can be used as a 3D texture.  It does not
* indicate whether or not the surface is being used for that purpose.
*/
#ifndef DDSCAPS_TEXTURE
#define DDSCAPS_TEXTURE                         0x00001000l
#endif

/*
* ddsCaps field is valid.
*/
#ifndef DDSD_CAPS
#define DDSD_CAPS               0x00000001l     // default
#endif

/*
* dwHeight field is valid.
*/
#ifndef DDSD_HEIGHT
#define DDSD_HEIGHT             0x00000002l
#endif

/*
* dwWidth field is valid.
*/
#ifndef DDSD_WIDTH
#define DDSD_WIDTH              0x00000004l
#endif

/*
* lPitch is valid.
*/
#ifndef DDSD_PITCH
#define DDSD_PITCH              0x00000008l
#endif

/*
* dwBackBufferCount is valid.
*/
#ifndef DDSD_BACKBUFFERCOUNT
#define DDSD_BACKBUFFERCOUNT    0x00000020l
#endif

/*
* dwZBufferBitDepth is valid.  (shouldn't be used in DDSURFACEDESC2)
*/
#ifndef DDSD_ZBUFFERBITDEPTH
#define DDSD_ZBUFFERBITDEPTH    0x00000040l
#endif

/*
* dwAlphaBitDepth is valid.
*/
#ifndef DDSD_ALPHABITDEPTH
#define DDSD_ALPHABITDEPTH      0x00000080l
#endif

/*
* lpSurface is valid.
*/
#define DDSD_LPSURFACE          0x00000800l

/*
* ddpfPixelFormat is valid.
*/
#define DDSD_PIXELFORMAT        0x00001000l

/*
* ddckCKDestOverlay is valid.
*/
#define DDSD_CKDESTOVERLAY      0x00002000l

/*
* ddckCKDestBlt is valid.
*/
#define DDSD_CKDESTBLT          0x00004000l

/*
* ddckCKSrcOverlay is valid.
*/
#define DDSD_CKSRCOVERLAY       0x00008000l

/*
* ddckCKSrcBlt is valid.
*/
#define DDSD_CKSRCBLT           0x00010000l

/*
* dwMipMapCount is valid.
*/
#define DDSD_MIPMAPCOUNT        0x00020000l

/*
* dwRefreshRate is valid
*/
#define DDSD_REFRESHRATE        0x00040000l

/*
* dwLinearSize is valid
*/
#define DDSD_LINEARSIZE         0x00080000l

/*
* dwTextureStage is valid
*/
#define DDSD_TEXTURESTAGE       0x00100000l
/*
* dwFVF is valid
*/
#define DDSD_FVF                0x00200000l
/*
* dwSrcVBHandle is valid
*/
#define DDSD_SRCVBHANDLE        0x00400000l

/*
* dwDepth is valid
*/
#define DDSD_DEPTH              0x00800000l

/*
* All input fields are valid.
*/
#define DDSD_ALL                0x00fff9eel

/*
* DDSCAPS2
*/
struct MDDSCAPS2
{
	DWORD       dwCaps;         // capabilities of surface wanted
	DWORD       dwCaps2;
	DWORD       dwCaps3;
	union
	{
		DWORD       dwCaps4;
		DWORD       dwVolumeDepth;
	};
};

/*
* DDCOLORKEY
*/
struct MDDSCOLORKEY
{
	DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to
	// be treated as Color Key, inclusive
	DWORD       dwColorSpaceHighValue;  // high boundary of color space that is
	// to be treated as Color Key, inclusive
};

/*
* DDPIXELFORMAT
*/

struct MDDSPIXELFORMAT
{
	DWORD       dwSize;                 // size of structure
	DWORD       dwFlags;                // pixel format flags
	DWORD       dwFourCC;               // (FOURCC code)
	union
	{
		DWORD   dwRGBBitCount;          // how many bits per pixel
		DWORD   dwYUVBitCount;          // how many bits per pixel
		DWORD   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
		DWORD   dwAlphaBitDepth;        // how many bits for alpha channels
		DWORD   dwLuminanceBitCount;    // how many bits per pixel
		DWORD   dwBumpBitCount;         // how many bits per "buxel", total
		DWORD   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
		// format list and if DDPF_D3DFORMAT is set
	};
	union
	{
		DWORD   dwRBitMask;             // mask for red bit
		DWORD   dwYBitMask;             // mask for Y bits
		DWORD   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
		DWORD   dwLuminanceBitMask;     // mask for luminance bits
		DWORD   dwBumpDuBitMask;        // mask for bump map U delta bits
		DWORD   dwOperations;           // DDPF_D3DFORMAT Operations
	};
	union
	{
		DWORD   dwGBitMask;             // mask for green bits
		DWORD   dwUBitMask;             // mask for U bits
		DWORD   dwZBitMask;             // mask for Z bits
		DWORD   dwBumpDvBitMask;        // mask for bump map V delta bits
		struct
		{
			WORD    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
			WORD    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
		} MultiSampleCaps;

	};
	union
	{
		DWORD   dwBBitMask;             // mask for blue bits
		DWORD   dwVBitMask;             // mask for V bits
		DWORD   dwStencilBitMask;       // mask for stencil bits
		DWORD   dwBumpLuminanceBitMask; // mask for luminance in bump map
	};
	union
	{
		DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
		DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
		DWORD   dwLuminanceAlphaBitMask;// mask for alpha channel
		DWORD   dwRGBZBitMask;          // mask for Z channel
		DWORD   dwYUVZBitMask;          // mask for Z channel
	};
};

struct MDDSDESC2	
{
	DWORD               dwSize;                 // size of the DDSURFACEDESC structure
	DWORD               dwFlags;                // determines what fields are valid
	DWORD               dwHeight;               // height of surface to be created
	DWORD               dwWidth;                // width of input surface
	union
	{
		LONG            lPitch;                 // distance to start of next line (return value only)
		DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
	};
	union
	{
		DWORD           dwBackBufferCount;      // number of back buffers requested
		DWORD           dwDepth;                // the depth if this is a volume texture 
	};
	union
	{
		DWORD           dwMipMapCount;          // number of mip-map levels requestde
		// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
		DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
		DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
	};
	DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
	DWORD               dwReserved;             // reserved
	LPVOID              lpSurface;              // pointer to the associated surface memory
	union
	{
		MDDSCOLORKEY    ddckCKDestOverlay;      // color key for destination overlay use
		DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
	};
	MDDSCOLORKEY        ddckCKDestBlt;          // color key for destination blt use
	MDDSCOLORKEY        ddckCKSrcOverlay;       // color key for source overlay use
	MDDSCOLORKEY        ddckCKSrcBlt;           // color key for source blt use
	union
	{
		MDDSPIXELFORMAT ddpfPixelFormat;        // pixel format description of the surface
		DWORD           dwFVF;                  // vertex format description of vertex buffers
	};
	MDDSCAPS2           ddsCaps;                // direct draw surface capabilities
	DWORD               dwTextureStage;         // stage in multitexture cascade
};

#endif
