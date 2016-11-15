#ifndef _RSAMPLE
#define _RSAMPLE

#include "RSoundTimer.h"
#include "fmod.hpp"
#include <string>
using namespace std;



/// Type of sound
enum
{
	RSD_SOUNDTYPE_NONE			= 0,				///< Unknown type	: do not play
	RSD_SOUNDTYPE_GENERIC,							///< Generic type	: sample sound	, generic channel group
	RSD_SOUNDTYPE_BGM,								///< BGM type		: streaming		, BGM channel group
	RSD_SOUNDTYPE_EFFECT,							///< Effect type	: sample sound	, effect channel group	, with reverb and compressor
};





/// RSampleInfo
struct RSampleInfo
{
	// Variables
	string				strFileName;
	int					nType;
	bool				bLoop;
	float				fVolume;
	int					nPriority;
	float				fMinDist;
	float				fMaxDist;
	bool				bUnique;


	// Functions
	void Init()
	{
		strFileName.clear();
		nType = RSD_SOUNDTYPE_NONE;
		bLoop = false;
		fVolume = 1.0f;
		nPriority = 128;
		fMinDist = 100.0f;
		fMaxDist = 1000.0f;
		bUnique = false;
	}
};





/// class : RSample
class RSample
{
	friend class RSound;
	friend class RealSound3;


	/// Member variables
protected:
	FMOD::Sound*		m_pSound;					///< Pointer of FMOD sound
	string				m_strFileName;				///< File name
	unsigned char		m_nType;					///< Type
	bool				m_bLoop;					///< Looping
	float				m_fVolume;					///< Volume
	unsigned char		m_nPriority;				///< Priority
	float				m_fMinDist;					///< Min distance
	float				m_fMaxDist;					///< Max distance
	bool				m_bUnique;					///< Unique sound
	unsigned char		m_nReference;				///< Reference count
	TIMEVAL				m_tLastAccessTime;			///< Last access time


	/// Member functions
public:
	// Constructor/destructor
	RSample();
	RSample( const RSampleInfo& _info);
	virtual ~RSample();

protected:
	// Create/destroy sound
	bool _create_sound( FMOD::System* pSystem, int nMode);
	bool _release_sound();

	// Increase/decrease reference count
	void _inc_reference();
	void _dec_reference();

	// Update sample
	void _update_sample();


	/// Get functions
public:
	// Get sound pointer
	FMOD::Sound* GetSound() const;

	// Get data pointer and size
	virtual const char* GetDataPtr();
	virtual const unsigned int GetDataSize();

	// Get file name
	const char* GetFileName() const;

	// Get type
	int GetType() const;

	// Get loop
	bool GetLoop() const;

	// Get volume
	float GetVolume() const;

	// Get priority
	int GetPriority() const;

	// Get min/max distance
	float GetMinDist() const;
	float GetMaxDist() const;

	// Get unique sound
	bool GetUnique() const;

	// Get reference count
	int GetReference() const;

	// Get access time
	TIMEVAL GetLastAccessTime() const;


	/// Event handler
protected:
	virtual void OnPreCreateSound( int* pMode)		{}
	virtual void OnCreateSound()					{}
	virtual void OnReleaseSound()					{}
	virtual void OnLoadData()						{}
	virtual void OnReleaseData()					{}
};

#endif
