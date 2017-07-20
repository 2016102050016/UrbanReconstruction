///////////////////////class CMesh////////////////////////////////////
//		���Դ����������ˣ��������εĶ���������2-manifode mesh)	//
//		���Զ�ȡһ���obj,smf,wrl���ļ�����							//
//		���ݵ�������������������ӹ�ϵ							//
//																	//
//////////////////////////////////////////////////////////////////////
#ifndef _MESH_H_
#define _MESH_H_

#include "model.h"
#include <afx.h>
#include <list>

using namespace std;
typedef list<Vector3D>	_VECTORLIST;
typedef list<UINT>		_UINTLIST;

#define MAX_VERTEX_PER_FACE 20

class CVertex
{
public:
	Vector3D	m_vPosition;//�������
	UINT*		m_piEdge;//�Ӹõ㷢����halfedge,Ҫ���ݵ�Ķ�����̬����
	_UINTLIST	m_lEdgeList;//��������m_piEdge����ʱ����
	short		m_nValence;//��Ķ���
	bool		m_bIsBoundary;//�Ƿ�Ϊ�߽��
	Vector3D	m_vNormal;
	Vector3D	m_vMeanCurvature;//Mean curvature

	bool		m_bColapsed;//�Ƿ��Ѿ���ɾ��
	short		m_nCut;//�õ��뼸��cut path����
//	Vector3D	m_vParameter;//�õ�Ĳ���

public:
	//constructions
	CVertex() { m_piEdge=NULL; m_nValence=0; m_bIsBoundary=false;m_bColapsed=false;m_nCut=0;}
	CVertex(double x,double y,double z) {m_vPosition=Vector3D(x,y,z);m_piEdge=NULL; m_nValence=0;m_bIsBoundary=false;m_bColapsed=false;m_nCut=0;}
	CVertex(Vector3D v) {m_vPosition=v;m_piEdge=NULL; m_nValence=0;m_bIsBoundary=false;m_bColapsed=false;m_nCut=0;}
	virtual ~CVertex();

	//operations
	CVertex& operator = (CVertex& v);

};

class CWedge
{
public:
	Vector2D	m_vParameter;
	UINT	m_iVertex;
public:
	CWedge()	{m_iVertex=-1;}
	CWedge(UINT iVertex)	{m_iVertex=iVertex;}
	virtual ~CWedge();
};

class CEdge
{
public:
	UINT	m_iVertex[2];//�ߵ����˵㣬Vertex0��>Vertex1
	UINT	m_iWedge[2];//�ñ�ָ�������Wedge,��ʵ������Ϳ��Բ���m_iVertex[2]��

	UINT	m_iTwinEdge;//��ñ߷����෴����һ���ߣ����Ϊ-1��ñ�Ϊ�߽�
	UINT	m_iNextEdge;//����ʱ�뷽�����һ����
//	UINT	m_iPreEdge;
	UINT	m_iFace;//�ñ��������棬Ӧ�����������

	//for progressive mesh
	double	m_dPriority;//�ñ߻���ʱ������ֵ
//	double	m_dQEM;//�ñ߻���ʱ������ֵ
	bool	m_bColapsed;//�Ƿ��Ѿ���ɾ��
	bool	m_bLegal;//ɾ���ñ��Ƿ�Ϸ���3��׼��
	bool	m_bCut;//�Ƿ�Ϊ���ñ�
	UINT	m_iIndex;//��EdgeArray�е�Indexֵ



public:
	//constructions
	CEdge() {m_iVertex[0]=m_iVertex[1]=m_iWedge[0]=m_iWedge[1]=m_iTwinEdge=m_iNextEdge=m_iFace=-1;m_bColapsed=false;m_bLegal=true;m_bCut=false;}
	CEdge(UINT iV0, UINT iV1) { m_iVertex[0]=iV0; m_iVertex[1]=iV1; m_iWedge[0]=m_iWedge[1]=-1;m_iTwinEdge=m_iNextEdge=m_iFace=-1;m_bColapsed=false;m_bLegal=true;m_bCut=false;}
	virtual ~CEdge();

	//operations
	CEdge& operator = (const CEdge& e);
	
	
};

class CFace
{
public:
	short	m_nType;//������
	UINT*	m_piVertex;//���е�
	UINT*	m_piEdge;//���б�
	Vector3D m_vNormal;//����
	Vector3D m_vMassPoint;//����
	double	m_dArea;//���

	//for progressive mesh
	bool	m_bColapsed;
	WORD	m_color;//�����ɫ
	double	m_dSumArea;//ѹ�������а�����������������
	double	m_dStretch;//Geometry Stretch

public:
	//constructions
	CFace() {m_nType=0;m_piVertex=m_piEdge=NULL;m_vNormal=Vector3D(0.0,0.0,1.0);m_dArea=0.0;m_bColapsed=false;m_color=0;}
	CFace(short s);
	virtual ~CFace();

	//operations
	void Create(short s);
	CFace& operator = (const CFace& f);


};

class CEdgeArray//for progressmesh
{
public:
	CEdge*	m_pEdge;
	UINT*	m_aEdge;//array
	UINT	m_nValidEdge;//left edge number
	UINT	m_nTotalEdge;
public:
	CEdgeArray() {m_pEdge=NULL;m_aEdge=NULL;m_nValidEdge=0;}
	virtual ~CEdgeArray();
	void Create(CEdge* pEdge,UINT nValidEdge);
	UINT GetNextEdge();
	void RemoveEdge(UINT iEdge);
	void RestoreEdge(UINT iEdge);

	CEdgeArray& operator = (CEdgeArray& ea);

};


class CMesh :public CModel
{
public:
	UINT		m_nVertex;				//����
	CVertex*	m_pVertex;				//���
	UINT		m_nEdge;				//����
	CEdge*		m_pEdge; 				//�߱�
	UINT		m_nFace;	 			//����
	CFace*		m_pFace;				//���
	UINT		m_nWedge;				//Wedge��
	CWedge*		m_pWedge;				//Wedge��

	//temp
//	_UINTLIST m_lPickedFace;
	_UINTLIST m_lFocusEdge;
	_UINTLIST m_lFocusVertex;
	_UINTLIST m_lFocusFace;
//	_UINTLIST m_lPickedVertex;
	UINT	m_iPickedFace;
	UINT	m_iPickedEdge;
	UINT	m_iPickedVertex;

	bool	m_bClosed;

public:
	//constructions
	CMesh() {m_nVertex=m_nEdge=m_nWedge=m_nFace=0;m_pVertex=NULL;m_pEdge=NULL;m_pWedge=NULL;m_pFace=NULL;m_iPickedFace=m_iPickedEdge=m_iPickedVertex=-1;}
	CMesh(CMesh* pMesh);
	virtual ~CMesh();

	//operations
	//�������
public:
	BOOL	Load(CString sFileName);	// load from file
	BOOL	Save(CString sFileName);	// save to file
	double	GetEdgeLen(UINT iEdge);
	double  GetEdgeWeight(UINT iEdge);

	BOOL	construct();// construct connectivity

private:
	void	clear();
//	BOOL	construct();// construct connectivity
	BOOL	reConstruct();// construct connectivity from current mesh
	BOOL	loadFromSMF(CString sFileName);
	BOOL	loadFromWRL(CString sFileName);
	BOOL	loadFromTMP(CString sFileName);
	BOOL	loadFromGTS(CString sFileName);
	BOOL	saveToSMF(CString sFileName);
	void	calFaceNormal(UINT i);
	void	calVertexNormal(UINT i);
	void	calVertexMeanCurvature(UINT i);

};

#endif