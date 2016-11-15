#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

#include "MCapsule.h"
#include "XReActionAnimationInfo.h"

namespace Character_NET {

	using namespace System;
	using namespace System::Windows::Forms;

	[Serializable]
	public ref class TVector
	{
	public: float x;
	public: float y;
	public: float z;
	};

	[Serializable]
	public ref class TMatrix
	{
	public: float _11, _12, _13, _14;
	public: float _21, _22, _23, _24;
	public: float _31, _32, _33, _34;
	public: float _41, _42, _43, _44;
	};

	[Serializable]
	public ref class TCapsule
	{
	public: TVector top;
	public: TVector bottom;
	public: float height;
	public: float radius;
	};
	
	[Serializable]
	public ref class TBox
	{
	public: TVector vmin;
	public: TVector vmax;
	public: TMatrix matLocal;
	};

	[Serializable]
	public ref class TClipboardCapsuleData : public Object
	{
	private:
		TCapsule	m_Capsule;

	public:
		// 탤런트 판정
		float	fCheckTime;

		// 메쉬 충돌
		String^ strBoneTop;
		String^ strBoneBottom;

		// 메쉬 판정
		float	fActiveTimeBegin;
		float	fActiveTimeEnd;

		bool	bHitFirst;

		int		nLinkedBPart;
		int		nHitEffectCapsuleID;

	public:

		// Creates a default constructor for the class.
		TClipboardCapsuleData() : fCheckTime(0.0f), fActiveTimeBegin(0.0f), fActiveTimeEnd(0.0f), bHitFirst(false), nLinkedBPart(0), nHitEffectCapsuleID(-1)
		{
			//m_Capsule = gcnew TCapsule;
			//myValue = "This is the value of the class";

//			m_pCapsule = new MCapsule();
		}
		virtual ~TClipboardCapsuleData()
		{
//			delete m_pCapsule;
		}

		property MCapsule Capsule 
		{

			// Creates a property to retrieve or set the value.
			MCapsule get()
			{
				MCapsule cap;

				cap.radius = m_Capsule.radius;
				cap.height = m_Capsule.height;
				cap.bottom.x = m_Capsule.bottom.x;
				cap.bottom.y = m_Capsule.bottom.y;
				cap.bottom.z = m_Capsule.bottom.z;
				cap.top.x = m_Capsule.top.x;
				cap.top.y = m_Capsule.top.y;
				cap.top.z = m_Capsule.top.z;

				return cap;
			}

			void set( MCapsule cap )
			{
				m_Capsule.radius = cap.radius;
				m_Capsule.height = cap.height;
				m_Capsule.bottom.x = cap.bottom.x;
				m_Capsule.bottom.y = cap.bottom.y;
				m_Capsule.bottom.z = cap.bottom.z;
				m_Capsule.top.x = cap.top.x;
				m_Capsule.top.y = cap.top.y;
				m_Capsule.top.z = cap.top.z;
			}
		}

	};

	[Serializable]
	public ref class TClipboardBoxData : public Object
	{
	private:
		TBox	m_Box;

	public:

	public:

		// Creates a default constructor for the class.
		TClipboardBoxData()
		{
			//m_Capsule = gcnew TCapsule;
			//myValue = "This is the value of the class";

			//			m_pCapsule = new MCapsule();
		}
		virtual ~TClipboardBoxData()
		{
			//			delete m_pCapsule;
		}

		property CSColBox colBox 
		{

			// Creates a property to retrieve or set the value.
			CSColBox get()
			{
				CSColBox box;

				box.vmin.x	= m_Box.vmin.x;
				box.vmin.y	= m_Box.vmin.y;
				box.vmin.z	= m_Box.vmin.z;
				box.vmax.x	= m_Box.vmax.x;
				box.vmax.y	= m_Box.vmax.y;
				box.vmax.z	= m_Box.vmax.z;
				
				box.m_matWorld._11 = m_Box.matLocal._11;
				box.m_matWorld._12 = m_Box.matLocal._12;
				box.m_matWorld._13 = m_Box.matLocal._13;
				box.m_matWorld._14 = m_Box.matLocal._14;
				box.m_matWorld._21 = m_Box.matLocal._21;
				box.m_matWorld._22 = m_Box.matLocal._22;
				box.m_matWorld._23 = m_Box.matLocal._23;
				box.m_matWorld._24 = m_Box.matLocal._24;
				box.m_matWorld._31 = m_Box.matLocal._31;
				box.m_matWorld._32 = m_Box.matLocal._32;
				box.m_matWorld._33 = m_Box.matLocal._33;
				box.m_matWorld._34 = m_Box.matLocal._34;
				box.m_matWorld._41 = m_Box.matLocal._41;
				box.m_matWorld._42 = m_Box.matLocal._42;
				box.m_matWorld._43 = m_Box.matLocal._43;
				box.m_matWorld._44 = m_Box.matLocal._44;

				return box;
			}

			void set( CSColBox box )
			{
				m_Box.vmin.x =  box.vmin.x;
				m_Box.vmin.y = 	box.vmin.y;
				m_Box.vmin.z = 	box.vmin.z;
				m_Box.vmax.x = 	box.vmax.x;
				m_Box.vmax.y = 	box.vmax.y;
				m_Box.vmax.z = 	box.vmax.z;

				m_Box.matLocal._11 = 	box.m_matWorld._11;
				m_Box.matLocal._12 = 	box.m_matWorld._12;
				m_Box.matLocal._13 = 	box.m_matWorld._13;
				m_Box.matLocal._14 = 	box.m_matWorld._14;
				m_Box.matLocal._21 = 	box.m_matWorld._21;
				m_Box.matLocal._22 = 	box.m_matWorld._22;
				m_Box.matLocal._23 = 	box.m_matWorld._23;
				m_Box.matLocal._24 = 	box.m_matWorld._24;
				m_Box.matLocal._31 = 	box.m_matWorld._31;
				m_Box.matLocal._32 = 	box.m_matWorld._32;
				m_Box.matLocal._33 = 	box.m_matWorld._33;
				m_Box.matLocal._34 = 	box.m_matWorld._34;
				m_Box.matLocal._41 = 	box.m_matWorld._41;
				m_Box.matLocal._42 = 	box.m_matWorld._42;
				m_Box.matLocal._43 = 	box.m_matWorld._43;
				m_Box.matLocal._44 = 	box.m_matWorld._44;
			}
		}

	};

	[Serializable]
	public ref class TClipboardGroupDataForCapsule : public Object
	{
	public:
		ArrayList^						m_arrCapsule;

	public:

		// Creates a default constructor for the class.
		TClipboardGroupDataForCapsule()
		{
			m_arrCapsule = gcnew ArrayList();
		}
		virtual ~TClipboardGroupDataForCapsule()
		{
		}

		void CopyTo(int nIndex, TClipboardCapsuleData^ pTarEffectInfo)
		{
			Array^ arrCopy = m_arrCapsule->ToArray();

			TClipboardCapsuleData^ capsuleInfo = nullptr;
			int nIndexCount = 0;
			System::Collections::IEnumerator^ myEnumerator = arrCopy->GetEnumerator();
			while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
			{
				if(nIndexCount == nIndex)
				{
					capsuleInfo = safe_cast<TClipboardCapsuleData^>(myEnumerator->Current);
					break;
				}

				++nIndexCount;
			}

			if(capsuleInfo != nullptr)
			{
				pTarEffectInfo->fCheckTime				= capsuleInfo->fCheckTime;

				pTarEffectInfo->strBoneTop				= capsuleInfo->strBoneTop;
				pTarEffectInfo->strBoneBottom			= capsuleInfo->strBoneBottom;

				pTarEffectInfo->fActiveTimeBegin		= capsuleInfo->fActiveTimeBegin;
				pTarEffectInfo->fActiveTimeEnd			= capsuleInfo->fActiveTimeEnd;

				pTarEffectInfo->bHitFirst				= capsuleInfo->bHitFirst;

				pTarEffectInfo->nLinkedBPart			= capsuleInfo->nLinkedBPart;

				pTarEffectInfo->Capsule					= capsuleInfo->Capsule;
				pTarEffectInfo->nHitEffectCapsuleID		= capsuleInfo->nHitEffectCapsuleID;
			}
		}

		void CopyFrom(TClipboardCapsuleData^ pTarEffectInfo)
		{
			TClipboardCapsuleData^ capsuleInfo = gcnew TClipboardCapsuleData;

			capsuleInfo->fCheckTime					= pTarEffectInfo->fCheckTime;				

			capsuleInfo->strBoneTop					= pTarEffectInfo->strBoneTop;				
			capsuleInfo->strBoneBottom				= pTarEffectInfo->strBoneBottom;	

			capsuleInfo->fActiveTimeBegin			= pTarEffectInfo->fActiveTimeBegin;	
			capsuleInfo->fActiveTimeEnd				= pTarEffectInfo->fActiveTimeEnd;			

			capsuleInfo->bHitFirst					= pTarEffectInfo->bHitFirst;	

			capsuleInfo->nLinkedBPart				= pTarEffectInfo->nLinkedBPart;
			capsuleInfo->nHitEffectCapsuleID		= pTarEffectInfo->nHitEffectCapsuleID;

			capsuleInfo->Capsule					= pTarEffectInfo->Capsule;		

			m_arrCapsule->Add(capsuleInfo);
		}
	};

	[Serializable]
	public ref class TClipboardGroupDataForBox : public Object
	{
	public:
		ArrayList^						m_arrBox;

	public:

		// Creates a default constructor for the class.
		TClipboardGroupDataForBox()
		{
			m_arrBox = gcnew ArrayList();
		}
		virtual ~TClipboardGroupDataForBox()
		{
		}

		void CopyTo(int nIndex, TClipboardBoxData^ pTarEffectInfo)
		{
			Array^ arrCopy = m_arrBox->ToArray();

			TClipboardBoxData^ BoxInfo = nullptr;
			int nIndexCount = 0;
			System::Collections::IEnumerator^ myEnumerator = arrCopy->GetEnumerator();
			while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
			{
				if(nIndexCount == nIndex)
				{
					BoxInfo = safe_cast<TClipboardBoxData^>(myEnumerator->Current);
					break;
				}

				++nIndexCount;
			}

			if(BoxInfo != nullptr)
			{
				pTarEffectInfo->colBox					= BoxInfo->colBox;
			}
		}

		void CopyFrom(TClipboardBoxData^ pTarEffectInfo)
		{
			TClipboardBoxData^ boxInfo = gcnew TClipboardBoxData;

			boxInfo->colBox					= pTarEffectInfo->colBox;		

			m_arrBox->Add(boxInfo);
		}
	};


	[Serializable]
	public ref class TClipboardCollisionData : public Object
	{
	public:
		TClipboardGroupDataForCapsule				m_Capsule;
		TClipboardGroupDataForBox					m_Box;

	public:

		TClipboardCollisionData() 
		{
		}
		virtual ~TClipboardCollisionData() {}

	};


	[Serializable]
	public ref class TClipboardGroupCapsuleBoxData : public TClipboardCollisionData
	{
	public:
		ArrayList^							m_arrGroup;

	public:

		TClipboardGroupCapsuleBoxData() 
		{
			m_arrGroup = gcnew ArrayList();
		}
		virtual ~TClipboardGroupCapsuleBoxData() {}

		void AddGroupData(TClipboardCollisionData^ pSegData)
		{
			m_arrGroup->Add(pSegData);
		}

		TClipboardCollisionData^ GetGroupData(int nIndex)
		{
			Array^ arrCopy = m_arrGroup->ToArray();

			int nIndexCount = 0;
			System::Collections::IEnumerator^ myEnumerator = arrCopy->GetEnumerator();
			while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
			{
				if(nIndexCount == nIndex)
				{
					return  safe_cast<TClipboardCollisionData^>(myEnumerator->Current);
				}

				++nIndexCount;
			}

			return nullptr;
		}
	};


	[Serializable]
	public ref class TTalentEffectInfo
	{
	public:
		TALENT_EFFECT_TYPE					m_nEffectType;

		String^								m_strTalentEffectInfoName;				// 데이터의 이름(툴에 사용하기 위한 이름)

		String^								m_strEffectName;
		String^								m_strBoneName;
		String^								m_strParameter;

		bool								m_bScale;

		IMPACTPOS							m_nImpactPos;
		TALENT_EFFECT_DIRECTION				m_nEffectDirection;
	};

	[Serializable]
	public ref class TClipboardEffectInfo : public Object
	{
	public:
		ArrayList^								m_arrEffectInfo;

	public:

		TClipboardEffectInfo() 
		{
			m_arrEffectInfo = gcnew ArrayList();
		}
		virtual ~TClipboardEffectInfo() {}

		void CopyTo(int nIndex, XTalentEffectInfo* pTarEffectInfo)
		{
			Array^ arrCopy = m_arrEffectInfo->ToArray();

			TTalentEffectInfo^ effectInfo = nullptr;
			int nIndexCount = 0;
			System::Collections::IEnumerator^ myEnumerator = arrCopy->GetEnumerator();
			while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
			{
				if(nIndexCount == nIndex)
					effectInfo = safe_cast<TTalentEffectInfo^>(myEnumerator->Current);

				++nIndexCount;
			}

			if(effectInfo != nullptr)
			{
				pTarEffectInfo->m_nEffectType = effectInfo->m_nEffectType;

				pTarEffectInfo->m_bScale = effectInfo->m_bScale;

				pTarEffectInfo->m_nImpactPos = effectInfo->m_nImpactPos;
				pTarEffectInfo->m_nEffectDirection = effectInfo->m_nEffectDirection;

				pTarEffectInfo->m_strTalentEffectInfoName = MStringToCharPointer(effectInfo->m_strTalentEffectInfoName);
				pTarEffectInfo->m_strEffectName = MStringToCharPointer(effectInfo->m_strEffectName);
				pTarEffectInfo->m_strBoneName = MStringToCharPointer(effectInfo->m_strBoneName);
				pTarEffectInfo->m_strParameter = MStringToCharPointer(effectInfo->m_strParameter);
			}
		}

		void CopyFrom(XTalentEffectInfo* pTarEffectInfo)
		{
			TTalentEffectInfo^ talentEffectInfo = gcnew TTalentEffectInfo;

			talentEffectInfo->m_nEffectType = pTarEffectInfo->m_nEffectType;

			talentEffectInfo->m_bScale = pTarEffectInfo->m_bScale;

			talentEffectInfo->m_nImpactPos = pTarEffectInfo->m_nImpactPos;
			talentEffectInfo->m_nEffectDirection = pTarEffectInfo->m_nEffectDirection;

			talentEffectInfo->m_strTalentEffectInfoName = gcnew String(pTarEffectInfo->m_strTalentEffectInfoName.c_str());
			talentEffectInfo->m_strEffectName = gcnew String(pTarEffectInfo->m_strEffectName.c_str());
			talentEffectInfo->m_strBoneName = gcnew String(pTarEffectInfo->m_strBoneName.c_str());
			talentEffectInfo->m_strParameter = gcnew String(pTarEffectInfo->m_strParameter.c_str());

			m_arrEffectInfo->Add(talentEffectInfo);
		}
	};

	[Serializable]
	public ref class TTalentEventInfo
	{
	public:
		TALENT_EVENT		m_nEvent;
		float				m_fTime1;
		float				m_fTime2;
		String^				m_strCustomEvent;
		String^				m_strParam1;
		String^				m_strParam2;
		int					m_nParam1;
		int					m_nParam2;
		bool				m_bFollow;
		bool				m_bServerDir;

		float				m_vLocal_x;
		float				m_vLocal_y;


		//----------------------------------------------------//
		// 발사체
		TALENT_PROJECTILE_TYPE		m_nType;						// 발사체 타입
		TALENT_PROJECTILE_VISUAL_TYPE m_nVisualType;				// 발사체 비쥬얼 타입

		String^						m_strBoneName;					// 발사 위치를 측정하는 본 이름
		String^						m_strEffectName;				// 발사체 이펙트
		String^						m_strAttackEffectName;			// 발사체 피격 이펙트
		String^						m_strHitGroundEffectName;		// 발사체 지형 피격 이펙트
		String^						m_strDefenceEffectName;			// 발사체 방어 이펙트
		String^						m_strFireEffectName;			// 발사체 발사 이펙트

		float						m_fSpeed;						// 발사체 속도
		float						m_fHitRadius;					// 발사체 판정 크기
		float						m_fRange;						// 발사체의 사정거리(미터, m)

		// 서버
		float						m_vLocalStartPos_x;				// 발사 위치 (여성, 기본)
		float						m_vLocalStartPos_y;				// 발사 위치 (여성, 기본)
		float						m_vLocalStartPos_z;				// 발사 위치 (여성, 기본)

		// 클라
		int							m_nSegmentIndexForHitCapsuleType;				// 판정 세그먼트
		int							m_nCapsuleIndexForHitCapsuleType;				// 판정 캡슐
	};

	[Serializable]
	public ref class TClipboardEventInfo : public Object
	{
	public:
		ArrayList^								m_arrEventInfo;

	public:

		TClipboardEventInfo() 
		{
			m_arrEventInfo = gcnew ArrayList();
		}
		virtual ~TClipboardEventInfo() {}

		void CopyTo(int nIndex, CSTalentEventInfo* pTalentEventInfo)
		{
			Array^ arrCopy = m_arrEventInfo->ToArray();

			TTalentEventInfo^ eventInfo = nullptr;
			int nIndexCount = 0;
			System::Collections::IEnumerator^ myEnumerator = arrCopy->GetEnumerator();
			while ( (myEnumerator->MoveNext()) && (myEnumerator->Current != nullptr) )
			{
				if(nIndexCount == nIndex)
					eventInfo = safe_cast<TTalentEventInfo^>(myEnumerator->Current);

				++nIndexCount;
			}

			if(eventInfo == nullptr)
				return;

			pTalentEventInfo->m_nEvent													= eventInfo->m_nEvent;			

			pTalentEventInfo->m_fTime1													= eventInfo->m_fTime1;						
			pTalentEventInfo->m_fTime2													= eventInfo->m_fTime2;						
			pTalentEventInfo->m_strCustomEvent											= MStringToCharPointer(eventInfo->m_strCustomEvent);				
			pTalentEventInfo->m_strParam1												= MStringToCharPointer(eventInfo->m_strParam1);					
			pTalentEventInfo->m_strParam2												= MStringToCharPointer(eventInfo->m_strParam2);					
			pTalentEventInfo->m_nParam1													= eventInfo->m_nParam1;						
			pTalentEventInfo->m_nParam2													= eventInfo->m_nParam2;						
			pTalentEventInfo->m_bFollow													= eventInfo->m_bFollow;						
			pTalentEventInfo->m_bServerDir												= eventInfo->m_bServerDir;					

			pTalentEventInfo->m_vLocal.x												= eventInfo->m_vLocal_x;						
			pTalentEventInfo->m_vLocal.y												= eventInfo->m_vLocal_y;						


			// 발사체
			pTalentEventInfo->m_ProjectileInfo.m_nType									= eventInfo->m_nType;						
			pTalentEventInfo->m_ProjectileInfo.m_nVisualType							= eventInfo->m_nVisualType;					

			pTalentEventInfo->m_ProjectileInfo.m_strBoneName							= MStringToCharPointer(eventInfo->m_strBoneName);					
			pTalentEventInfo->m_ProjectileInfo.m_strEffectName							= MStringToCharPointer(eventInfo->m_strEffectName);				
			pTalentEventInfo->m_ProjectileInfo.m_strAttackEffectName					= MStringToCharPointer(eventInfo->m_strAttackEffectName);			
			pTalentEventInfo->m_ProjectileInfo.m_strHitGroundEffectName					= MStringToCharPointer(eventInfo->m_strHitGroundEffectName);		
			pTalentEventInfo->m_ProjectileInfo.m_strDefenceEffectName					= MStringToCharPointer(eventInfo->m_strDefenceEffectName);			
			pTalentEventInfo->m_ProjectileInfo.m_strFireEffectName						= MStringToCharPointer(eventInfo->m_strFireEffectName);			

			pTalentEventInfo->m_ProjectileInfo.m_fSpeed									= eventInfo->m_fSpeed;						
			pTalentEventInfo->m_ProjectileInfo.m_fHitRadius								= eventInfo->m_fHitRadius;					
			pTalentEventInfo->m_ProjectileInfo.m_fRange									= eventInfo->m_fRange;						


			pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.x						= eventInfo->m_vLocalStartPos_x;				
			pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.y						= eventInfo->m_vLocalStartPos_y;				
			pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.z						= eventInfo->m_vLocalStartPos_z;				


			pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType			= eventInfo->m_nSegmentIndexForHitCapsuleType;
			pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType			= eventInfo->m_nCapsuleIndexForHitCapsuleType;
		}

		void CopyFrom(CSTalentEventInfo* pTalentEventInfo)
		{
			TTalentEventInfo^ talentEventInfo = gcnew TTalentEventInfo;

			talentEventInfo->m_nEvent							= pTalentEventInfo->m_nEvent;

			talentEventInfo->m_fTime1							= pTalentEventInfo->m_fTime1;
			talentEventInfo->m_fTime2							= pTalentEventInfo->m_fTime2;
			talentEventInfo->m_strCustomEvent					= gcnew String(pTalentEventInfo->m_strCustomEvent.c_str());
			talentEventInfo->m_strParam1						= gcnew String(pTalentEventInfo->m_strParam1.c_str());
			talentEventInfo->m_strParam2						= gcnew String(pTalentEventInfo->m_strParam2.c_str());
			talentEventInfo->m_nParam1							= pTalentEventInfo->m_nParam1;
			talentEventInfo->m_nParam2							= pTalentEventInfo->m_nParam2;
			talentEventInfo->m_bFollow							= pTalentEventInfo->m_bFollow;
			talentEventInfo->m_bServerDir						= pTalentEventInfo->m_bServerDir;

			talentEventInfo->m_vLocal_x							= pTalentEventInfo->m_vLocal.x;
			talentEventInfo->m_vLocal_y							= pTalentEventInfo->m_vLocal.y;


			// 발사체
			talentEventInfo->m_nType							= pTalentEventInfo->m_ProjectileInfo.m_nType;
			talentEventInfo->m_nVisualType						= pTalentEventInfo->m_ProjectileInfo.m_nVisualType;	

			talentEventInfo->m_strBoneName						= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strBoneName.c_str());		
			talentEventInfo->m_strEffectName					= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strEffectName.c_str());		
			talentEventInfo->m_strAttackEffectName				= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strAttackEffectName.c_str());			
			talentEventInfo->m_strHitGroundEffectName			= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strHitGroundEffectName.c_str());		
			talentEventInfo->m_strDefenceEffectName				= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strDefenceEffectName.c_str());	
			talentEventInfo->m_strFireEffectName				= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strFireEffectName.c_str());			

			talentEventInfo->m_fSpeed							= pTalentEventInfo->m_ProjectileInfo.m_fSpeed;						
			talentEventInfo->m_fHitRadius						= pTalentEventInfo->m_ProjectileInfo.m_fHitRadius;					
			talentEventInfo->m_fRange							= pTalentEventInfo->m_ProjectileInfo.m_fRange;						


			talentEventInfo->m_vLocalStartPos_x					= pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.x;			
			talentEventInfo->m_vLocalStartPos_y					= pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.y;		
			talentEventInfo->m_vLocalStartPos_z					= pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.z;		


			talentEventInfo->m_nSegmentIndexForHitCapsuleType	= pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;
			talentEventInfo->m_nCapsuleIndexForHitCapsuleType	= pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;


			m_arrEventInfo->Add(talentEventInfo);
		}
	};


	[Serializable]
	public ref class TClipboardReActionRowData : public Object
	{
	public:
		ReAction_DATA*			m_pReactionData;

	public:

		TClipboardReActionRowData() 
		{
			m_pReactionData = NULL;
		}
		virtual ~TClipboardReActionRowData() 
		{
			SAFE_DELETE(m_pReactionData);
		}

	};

	public ref class TClipboardHelper
	{
	public: 
		static void Copy(Character_NET::TClipboardCapsuleData^ capsuleData)
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "SoulHuntCapsuleFormat" );

			DataObject^ myDataObject = gcnew DataObject( myFormat->Name, capsuleData );

			// Copies myObject into the clipboard.
			Clipboard::SetDataObject( myDataObject );
		}
		static TClipboardCapsuleData^ Paste()
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "SoulHuntCapsuleFormat" );

			System::Windows::Forms::IDataObject^ myRetrievedObject = Clipboard::GetDataObject();

			TClipboardCapsuleData^ myDereferencedObject = dynamic_cast<Character_NET::TClipboardCapsuleData^>(myRetrievedObject->GetData( myFormat->Name ));

			return myDereferencedObject;
		}


		static void CopyEffectInfo(Character_NET::TClipboardEffectInfo^ effectInfo)
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "SoulHuntEffectInfoFormat" );

			DataObject^ myDataObject = gcnew DataObject( myFormat->Name, effectInfo );

			// Copies myObject into the clipboard.
			Clipboard::SetDataObject( myDataObject );
		}
		static TClipboardEffectInfo^ PasteEffectInfo()
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "SoulHuntEffectInfoFormat" );

			System::Windows::Forms::IDataObject^ myRetrievedObject = Clipboard::GetDataObject();

			TClipboardEffectInfo^ myDereferencedObject = dynamic_cast<Character_NET::TClipboardEffectInfo^>(myRetrievedObject->GetData( myFormat->Name ));

			return myDereferencedObject;
		}

		static void CopyEventInfo(Character_NET::TClipboardEventInfo^ eventInfo)
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "RaiderzEventInfoFormat" );

			DataObject^ myDataObject = gcnew DataObject( myFormat->Name, eventInfo );

			// Copies myObject into the clipboard.
			Clipboard::SetDataObject( myDataObject );
		}
		static TClipboardEventInfo^ PasteEventInfo()
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "RaiderzEventInfoFormat" );

			System::Windows::Forms::IDataObject^ myRetrievedObject = Clipboard::GetDataObject();

			TClipboardEventInfo^ myDereferencedObject = dynamic_cast<Character_NET::TClipboardEventInfo^>(myRetrievedObject->GetData( myFormat->Name ));

			return myDereferencedObject;
		}

		static void CopyGroupCapsuleData(Character_NET::TClipboardGroupCapsuleBoxData^ meshHitData)
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "RaiderzGroupCapsuleDataFormat" );

			DataObject^ myDataObject = gcnew DataObject( myFormat->Name, meshHitData );

			// Copies myObject into the clipboard.
			Clipboard::SetDataObject( myDataObject );
		}
		static TClipboardGroupCapsuleBoxData^ PasteGroupCapsuleData()
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "RaiderzGroupCapsuleDataFormat" );

			System::Windows::Forms::IDataObject^ myRetrievedObject = Clipboard::GetDataObject();

			TClipboardGroupCapsuleBoxData^ myDereferencedObject = dynamic_cast<Character_NET::TClipboardGroupCapsuleBoxData^>(myRetrievedObject->GetData( myFormat->Name ));

			return myDereferencedObject;
		}

		static void CopyRowData(Character_NET::TClipboardReActionRowData^ rowData)
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "RaiderzReActionRowData" );

			DataObject^ myDataObject = gcnew DataObject( myFormat->Name, rowData );

			// Copies myObject into the clipboard.
			Clipboard::SetDataObject( myDataObject );
		}
		static TClipboardReActionRowData^ PasteRowData()
		{
			// 새로운 데이터 포맷을 만든다.
			DataFormats::Format^ myFormat = DataFormats::GetFormat( "RaiderzReActionRowData" );

			System::Windows::Forms::IDataObject^ myRetrievedObject = Clipboard::GetDataObject();

			TClipboardReActionRowData^ myDereferencedObject = dynamic_cast<Character_NET::TClipboardReActionRowData^>(myRetrievedObject->GetData( myFormat->Name ));

			return myDereferencedObject;
		}
	};
}