#include "stdafx.h"
#include "MathUtils.h"
#include "CrySkinBuilderBase.h"
#include "CrySkinFull.h"
#include "platform.h"

#define FOR_TEST 0

// takes each offset and includes it into the bbox of corresponding bone
/*void CrySkinFull::computeBoneBBoxes(CryBBoxA16* pBBoxes)
{
	CrySkinAuxInt* pAux = &m_arrAux[0];
	Vertex* pVertex = &m_arrVertices[0];
	CryBBoxA16* pBBox = pBBoxes + m_numSkipBones, *pBBoxEnd = pBBoxes + m_numBones;

	for (; pBBox!= pBBoxEnd; ++pBBox)
	{
		// each bone has a group of vertices

		// first process the rigid vertices
		Vertex* pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex)
			pBBox->include(pVertex->pt);

		// process the smooth1 vertices that were the first time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
			pBBox->include(pVertex->pt);

		// process the smooth vertices that were the second time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
			pBBox->include(pVertex->pt);
	}
}*/

//////////////////////////////////////////////////////////////////////////
// does the skinning out of the given array of global matrices
void CrySkinFull::skin (const Matrix44* pBones, Vec3d* pDest)
{
#ifdef DEFINE_PROFILER_FUNCTION
	DEFINE_PROFILER_FUNCTION();
#endif

	//PROFILE_FRAME_SELF(PureSkin);
#if FOR_TEST
	for (int i = 0; i < g_GetCVars()->ca_TestSkinningRepeats(); ++i)
#endif
	{
	
	const Matrix44* pBone			= pBones + m_numSkipBones;
	const Matrix44* pBonesEnd = pBones + m_numBones;

	u32 s = 0;
	u32 t = 0;

#ifdef _DEBUG
	TFixedArray<float> arrW;
	arrW.reinit(m_numDests, 0);
#endif

	for (; pBone!= pBonesEnd; ++pBone)
	{

		Matrix34 m34 = Matrix34( GetTransposed44(*pBone) );

		// first process the rigid vertices
		u32 a0=m_arrAux[t];
		for (u32 i=0; i<a0; i++ )
		{
			//_mm_prefetch( (char*)&m_arrVertices[s+20].pt, _MM_HINT_T0 );
			pDest[m_arrVertices[s].nDest] = m34 * m_arrVertices[s].pt;

			#ifdef _DEBUG
				assert (arrW[m_arrVertices[s].nDest] == 0);
				arrW[m_arrVertices[s].nDest] = 1;
			#endif
			s++;
		}
		t++;

		// process the smooth1 vertices that were the first time met
		u32 a1=m_arrAux[t]; t++;
		for (u32 i=0; i<a1; i++ )
		{
			//_mm_prefetch( (char*)&m_arrVertices[s+20].pt, _MM_HINT_T0 );
			pDest[m_arrAux[t]]= (m34*m_arrVertices[s].pt) * m_arrVertices[s].fWeight;
			
			#ifdef _DEBUG
				assert (arrW[m_arrAux[t]] == 0);
				arrW[m_arrAux[t]] = m_arrVertices[s].fWeight;
			#endif
			s++;
			t++;
		}

		// process the smooth vertices that were the first time met
		u32 a2=m_arrAux[t]; t++;
		for (u32 i=0; i<a2; i++)
		{
			//_mm_prefetch( (char*)&m_arrVertices[s+20].pt, _MM_HINT_T0 );
			pDest[m_arrAux[t]] += (m34*m_arrVertices[s].pt) * m_arrVertices[s].fWeight;
			
			#ifdef _DEBUG
				assert (arrW[m_arrAux[t]] > 0 && arrW[m_arrAux[t]] < 1.005f);
				arrW[m_arrAux[t]] += m_arrVertices[s].fWeight;
				assert (arrW[m_arrAux[t]] > 0 && arrW[m_arrAux[t]] < 1.005f);
			#endif
		 s++;
		 t++;
		}
	}
	
	/*#ifdef _DEBUG
	for (unsigned i = 0; i < m_numDests; ++i)
		assert (arrW[i] > 0.995f && arrW[i] < 1.005f);
	#endif
	*/

	}
}

//////////////////////////////////////////////////////////////////////////
// does the skinning out of the given array of global matrices
void CrySkinFull::skinAsVec3d16 (const Matrix44* pBones, Vec3dA16* pDest)
{
	//PROFILE_FRAME_SELF(PureSkin);
#if FOR_TEST
	for (int i = 0; i < g_GetCVars()->ca_TestSkinningRepeats(); ++i)
#endif
	{
	const Matrix44* pBone = pBones + m_numSkipBones, *pBonesEnd = pBones + m_numBones;
	CrySkinAuxInt* pAux = &m_arrAux[0];
	Vertex* pVertex = &m_arrVertices[0];


#ifdef _DEBUG
	TFixedArray<float> arrW;
	arrW.reinit(m_numDests, 0);
#endif

	for (; pBone!= pBonesEnd; ++pBone)
	{
		// each bone has a group of vertices

		// first process the rigid vertices
		Vertex* pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex)
		{
			//CHANGED_BY_IVO  - INVALID CHANGE, PLEASE REVISE
			pDest[pVertex->nDest].v = pBone->TransformVectorOLD(pVertex->pt);
			// Temporary fixed by Sergiy. A new operation in the Matrix must be made
			//pDest[pVertex->nDest].v = GetTransposed44(*pBone) * (pVertex->pt);
			//transformVectorNoTrans (pDest[pVertex->nDest].v, pVertex->pt, *pBone);
#ifdef _DEBUG
			assert (arrW[pVertex->nDest] == 0);
			arrW[pVertex->nDest] = 1;
#endif
		}

		// process the smooth1 vertices that were the first time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
		{
			transformWVector (pDest[*pAux].v, *pBone, *pVertex);
#ifdef _DEBUG
			assert (arrW[*pAux] == 0);
			arrW[*pAux] = pVertex->fWeight;
#endif
		}

		// process the smooth vertices that were the first time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
		{
			addWVector (pDest[*pAux].v, *pBone, *pVertex);
#ifdef _DEBUG
			assert (arrW[*pAux] > 0 && arrW[*pAux] < 1.005f);
			arrW[*pAux] += pVertex->fWeight;
			assert (arrW[*pAux] > 0 && arrW[*pAux] < 1.005f);
#endif
		}
	}
#ifdef _DEBUG
	for (unsigned i = 0; i < m_numDests; ++i)
		assert (arrW[i] > 0.995f && arrW[i] < 1.005f);
#endif
	}
}

void CrySkinFull::CStatistics::addDest(unsigned nDest)
{
	if (arrNumLinks.size() < nDest+1)
		arrNumLinks.resize (nDest+1,0);
	++arrNumLinks[nDest];
	setDests.insert (nDest);
}

void CrySkinFull::CStatistics::initSetDests (const CrySkinFull* pSkin)
{
	const CrySkinAuxInt* pAux = &pSkin->m_arrAux[0];
	const Vertex* pVertex = &pSkin->m_arrVertices[0];

	arrNumLinks.clear();

	for (unsigned nBone = pSkin->m_numSkipBones; nBone < pSkin->m_numBones; ++nBone)
	{
		// each bone has a group of vertices

		// first process the rigid vertices
		const Vertex* pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex)
		{
			unsigned nDest = pVertex->nDest;
			addDest (nDest);
			assert (arrNumLinks[nDest] == 1);
		}

		// process the smooth1 vertices that were the first time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
		{
			unsigned nDest = *pAux;
			addDest (nDest);
			assert (arrNumLinks[nDest] == 1);
			//pVertex->fWeight is the weight of the vertex
		}

		// process the smooth vertices that were the second/etc time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
		{
			unsigned nDest = *pAux;
			addDest (nDest);
			assert (arrNumLinks[nDest] > 1);
			// pVertex->fWeight contains the weight of the vertex
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
// validates the skin against the given geom info
#if defined (_DEBUG)
void CrySkinFull::validate (const ICrySkinSource* pGeometry)
{
	TElementaryArray<unsigned> arrNumLinks ("CrySkinFull::validate.arrNumLinks");
	arrNumLinks.reinit (pGeometry->numVertices(), 0);

	CrySkinAuxInt* pAux = &m_arrAux[0];
	Vertex* pVertex = &m_arrVertices[0];

	for (unsigned nBone = m_numSkipBones; nBone < m_numBones; ++nBone)
	{
		// each bone has a group of vertices

		// first process the rigid vertices
		Vertex* pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex)
		{
			unsigned nDest = pVertex->nDest;
			const CryVertexBinding& rLink = pGeometry->getLink(nDest);
			assert (arrNumLinks[nDest] == 0);
			arrNumLinks[nDest] = 1;
			assert (rLink.size()==1);
			assert (rLink[0].Blending == 1);
			assert (rLink[0].BoneID == nBone);
		}

		// process the smooth1 vertices that were the first time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
		{
			unsigned nDest = *pAux;
			const CryVertexBinding& rLink = pGeometry->getLink(nDest);
			assert (arrNumLinks[nDest]++ == 0);
			assert (rLink.size()>1);
			float fLegacyWeight = rLink.getBoneWeight(nBone);
			assert (pVertex->fWeight == fLegacyWeight);
		}

		// process the smooth vertices that were the first time met
		pGroupEnd = pVertex + *pAux++;
		for (;pVertex < pGroupEnd; ++pVertex, ++pAux)
		{
			unsigned nDest = *pAux;
			const CryVertexBinding& rLink = pGeometry->getLink(nDest);
			assert (arrNumLinks[nDest]++ > 0);
			assert (arrNumLinks[nDest] <= rLink.size());
			assert (rLink.size()>1);
			float fLegacyWeight = rLink.getBoneWeight(nBone);
			assert (rLink.hasBoneWeight(nBone,pVertex->fWeight));
		}
	}

	for (unsigned nVert = 0; nVert < pGeometry->numVertices(); ++nVert)
		assert (arrNumLinks[nVert] == pGeometry->getLink(nVert).size());
}
#endif

#if ( defined (_CPU_X86) || defined (_CPU_AMD64) ) & !defined(LINUX)

DEFINE_ALIGNED_DATA( CryBBoxA16, CrySkinFull::g_BBox, 32 ); // align by cache line boundaries

#if defined (_CPU_AMD64)
extern "C" void Amd64Skinner(CrySkinAuxInt* pAux, CrySkinVertexAligned* pVertex, Vec3dA16* pDest, const Matrix44* pBone, Vec3dA16* pvMin,const Matrix44* pBoneEnd);
#endif


void CrySkinFull::skinSSE (const Matrix44* pBones, Vec3dA16* pDest)
{
#ifdef DEFINE_PROFILER_FUNCTION
	DEFINE_PROFILER_FUNCTION();
#endif

	Vec3 vBBoxMin (1e6f, 1e6f, 1e6f);   // negative volume: the first vertex re-initializes it
	Vec3 vBBoxMax (-1e6f, -1e6f, -1e6f);

#if FOR_TEST
	for (int i = 0; i < g_GetCVars()->ca_TestSkinningRepeats(); ++i)
#endif
	{
		const Matrix44* pBone = pBones + m_numSkipBones, *pBoneEnd = pBones + m_numBones;
		const CrySkinAuxInt* pAux = &m_arrAux[0];
		const Vertex* pVertex = &m_arrVertices[0];

		for (; pBone != pBoneEnd; ++pBone)
		{
			// rigid vertices: the destination is over-written, the bbox is built from them
			for (unsigned nCount = *pAux++; nCount; --nCount, ++pVertex)
			{
				const Vec3 vResult = pBone->TransformPointOLD(pVertex->pt);
				pDest[pVertex->nDest & 0xFFFFFF].v = vResult;
				AddToBounds(vResult, vBBoxMin, vBBoxMax);
			}

			// smooth1 vertices: met for the first time
			for (unsigned nCount = *pAux++; nCount; --nCount, ++pVertex, ++pAux)
				pDest[*pAux].v = pBone->TransformPointOLD(pVertex->pt) * pVertex->fWeight;

			// smooth2 vertices: met for the second and the following times
			for (unsigned nCount = *pAux++; nCount; --nCount, ++pVertex, ++pAux)
				pDest[*pAux].v += pBone->TransformPointOLD(pVertex->pt) * pVertex->fWeight;
		}
	}

	g_BBox.vMin.v = vBBoxMin;
	g_BBox.vMax.v = vBBoxMax;
	g_BBox.vMin.m_Pad = 0;
	g_BBox.vMax.m_Pad = 0;
}

#endif
