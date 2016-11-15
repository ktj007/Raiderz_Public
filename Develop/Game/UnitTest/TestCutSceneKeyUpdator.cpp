#include "stdafx.h"
#include "TestCutSceneCommon.h"

SUITE(CutSceneKeyUpdator)
{
	TEST(CutSceneFrameUpdator)
	{
		struct SKeyInfo
		{
			SKeyInfo(float _fTime, size_t _nID)
				: fTime(_fTime), nID(_nID)
			{

			}

			float fTime;
			size_t nID;
		};

		typedef std::vector<SKeyInfo> VEC_KEYS;

		class TestKeyOwner
		{
		public:
			TestKeyOwner()
			{
				ResetKeyQuery();
			}

			~TestKeyOwner()
			{

			}

			VEC_KEYS& GetKeys(){ return m_vKeys; }
			void AddKey(float fTime)
			{
				size_t nId = m_vKeys.size();
				m_vKeys.push_back( SKeyInfo(fTime, nId) );
			}

			int GetFirstKeyInBetween()
			{
				return m_nBetweenKeyId;
			}

			int GetOverKey()
			{
				return m_nOverKeyId;
			}

			void ResetKeyQuery()
			{
				m_nBetweenKeyId = m_nOverKeyId = -1;
			}

			void OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fInterpolationRatio, SKeyInfo* pPre, SKeyInfo* pNextNext)
			{
				m_nBetweenKeyId = (int)pCurrent->nID;
			}

			void OnUpdateOverKey(SKeyInfo* pProcKey)
			{
				m_nOverKeyId = (int)pProcKey->nID;
			}

		private:
			VEC_KEYS m_vKeys;

			int m_nBetweenKeyId;
			int m_nOverKeyId;

		} key_owner;

		XCutSceneKeyFrameUpdator<SKeyInfo> keyframe_updator;

		// check sort key
		key_owner.AddKey(0.4f); // id:0
		key_owner.AddKey(0.8f); // id:1
		key_owner.AddKey(0.3f); // id:2
		key_owner.AddKey(0.6f); // id:3
		key_owner.AddKey(0.1f); // id:4
		CHECK_CLOSE( 0.8f, key_owner.GetKeys().at(1).fTime, 0.001f);
		CHECK_CLOSE( 0.6f, key_owner.GetKeys().at(3).fTime, 0.001f);

		float fStartTime, fEndTime;
		keyframe_updator.SortKey(key_owner.GetKeys(), &fStartTime, &fEndTime);
		CHECK_CLOSE( 0.3f, key_owner.GetKeys().at(1).fTime, 0.001f);
		CHECK_CLOSE( 0.6f, key_owner.GetKeys().at(3).fTime, 0.001f);
		CHECK_CLOSE( 0.1f, fStartTime, 0.001f);
		CHECK_CLOSE( 0.8f, fEndTime, 0.001f);

		keyframe_updator.UpdateKey(&key_owner, 0.05f, key_owner.GetKeys());
		CHECK_EQUAL(-1, key_owner.GetFirstKeyInBetween());
		CHECK_EQUAL(-1, key_owner.GetOverKey());
		key_owner.ResetKeyQuery();

		keyframe_updator.UpdateKey(&key_owner, 0.1f, key_owner.GetKeys()); //0.15f
		CHECK_EQUAL(4, key_owner.GetFirstKeyInBetween());
		CHECK_EQUAL(-1, key_owner.GetOverKey());
		key_owner.ResetKeyQuery();

		keyframe_updator.Reset();
		keyframe_updator.UpdateKey(&key_owner, 0.3f, key_owner.GetKeys()); //0.3f, 경계일경우
		CHECK_EQUAL(2, key_owner.GetFirstKeyInBetween());
		CHECK_EQUAL(-1, key_owner.GetOverKey());
		key_owner.ResetKeyQuery();

		keyframe_updator.UpdateKey(&key_owner, 0.01f, key_owner.GetKeys()); //0.31f
		CHECK_EQUAL(2, key_owner.GetFirstKeyInBetween());
		CHECK_EQUAL(-1, key_owner.GetOverKey());
		key_owner.ResetKeyQuery();

		keyframe_updator.UpdateKey(&key_owner, 0.3f, key_owner.GetKeys()); //0.61f
		CHECK_EQUAL(3, key_owner.GetFirstKeyInBetween());
		CHECK_EQUAL(-1, key_owner.GetOverKey());
		key_owner.ResetKeyQuery();

		keyframe_updator.UpdateKey(&key_owner, 1.f, key_owner.GetKeys()); //1.61f
		CHECK_EQUAL(-1, key_owner.GetFirstKeyInBetween());
		CHECK_EQUAL(1, key_owner.GetOverKey());
	}
}
