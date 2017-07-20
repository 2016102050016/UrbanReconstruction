#include "mesh.h"

#define ITERATE_TIMES 1
#define	ALPHA_STRETCH 1.0

//////////////////////////////////////////////////////
//						CVertex						//
//////////////////////////////////////////////////////
CVertex::~CVertex()
{
	if(m_piEdge!=NULL)
		delete[] m_piEdge;
	m_lEdgeList.clear();
}

CVertex& CVertex::operator = (CVertex& v)
{
	m_vPosition = v.m_vPosition;//�������
	m_nValence = v.m_nValence;//��Ķ���
	m_piEdge = new UINT[m_nValence];//�Ӹõ㷢����halfedge,Ҫ���ݵ�Ķ�����̬����
	for(short i=0;i<m_nValence;i++)
		m_piEdge[i] = v.m_piEdge[i];
//	m_lEdgeList = v.m_lEdgeList;//��������m_piEdge����ʱ����
	
	m_bIsBoundary = v.m_bIsBoundary;//�Ƿ�Ϊ�߽��
	m_vNormal = v.m_vNormal;
	m_vMeanCurvature = v.m_vMeanCurvature;//Mean curvature
	m_bColapsed = v.m_bColapsed;//�Ƿ��Ѿ���ɾ��

	return *this;
}
//////////////////////////////////////////////////////
//						CWedge						//
//////////////////////////////////////////////////////

CWedge::~CWedge()
{
}


//////////////////////////////////////////////////////
//						CEdge						//
//////////////////////////////////////////////////////

CEdge::~CEdge()
{
}

CEdge& CEdge::operator = (const CEdge& e)
{
	m_iVertex[0] = e.m_iVertex[0];//�ߵ����˵㣬Vertex0��>Vertex1
	m_iVertex[1] = e.m_iVertex[1];
	m_iTwinEdge = e.m_iTwinEdge;//��ñ߷����෴����һ���ߣ����Ϊ-1��ñ�Ϊ�߽�
	m_iNextEdge = e.m_iNextEdge;//����ʱ�뷽�����һ����
	m_iFace = e.m_iFace;//�ñ��������棬Ӧ�����������

	//for progressive mesh
	m_dPriority = e.m_dPriority;//�ñ߻���ʱ������ֵ
//	double	m_dQEM;//�ñ߻���ʱ������ֵ
	m_bColapsed = e.m_bColapsed;//�Ƿ��Ѿ���ɾ��
	m_bLegal = e.m_bLegal;//ɾ���ñ��Ƿ�Ϸ���3��׼��
	m_bCut = e.m_bCut;
	m_iIndex = e.m_iIndex;//��EdgeArray�е�Indexֵ

	return *this;
}

//////////////////////////////////////////////////////
//						CFace						//
//////////////////////////////////////////////////////
CFace::CFace(short s)
{
	m_nType=s;
	m_vNormal=Vector3D(0.0,0.0,1.0);
	m_dArea=0.0;
	m_piEdge=new UINT[s];
	m_piVertex=new UINT[s];
	m_bColapsed=false;
	m_color=255;
}

CFace::~CFace()
{
	if(m_piEdge!=NULL)
		delete[] m_piEdge;
	if(m_piVertex!=NULL)
		delete[] m_piVertex;
}

void CFace::Create(short s)
{
	m_nType=s;
	m_vNormal=Vector3D(0.0,0.0,1.0);
	m_dArea=0.0;
	m_piEdge=new UINT[s];
	m_piVertex=new UINT[s];
}

CFace& CFace::operator =(const CFace& f)
{
	m_nType = f.m_nType;//������
	m_piVertex = new UINT[m_nType];//���е�
	m_piEdge = new UINT[m_nType];//���б�
	for(short i=0;i<m_nType;i++)
	{
		m_piVertex[i] = f.m_piVertex[i];
		m_piEdge[i] = f.m_piEdge[i];
	}
	
	m_vNormal = f.m_vNormal;//����
	m_vMassPoint = f.m_vMassPoint;//����
	m_dArea = f.m_dArea;//���
	//for progressive mesh
	m_bColapsed = f.m_bColapsed;
	m_color = f.m_color;//�����ɫ
	
	return (*this);
}
//////////////////////////////////////////////////////
//						CEdgeArray					//
//////////////////////////////////////////////////////
CEdgeArray::~CEdgeArray()
{
	if(m_aEdge!=NULL)
		delete[] m_aEdge;
}
void CEdgeArray::Create(CEdge* pEdge,UINT nValidEdge)
{
	m_pEdge=pEdge;
	m_aEdge = new UINT[nValidEdge];
	for(UINT i=0;i<nValidEdge;i++)
	{
		m_aEdge[i]=i;
		m_pEdge[i].m_iIndex=i;
	}
	m_nTotalEdge = m_nValidEdge = nValidEdge;

}

UINT CEdgeArray::GetNextEdge()
{
	UINT iMin=-1;
	double dMin=9999999.0;
	UINT i,j;
	for(i=0;i<m_nValidEdge;i++)
	{
		j=m_aEdge[i];
		if(!m_pEdge[j].m_bLegal)
			continue;
		if(m_pEdge[j].m_dPriority<dMin)
		{
			iMin=j;
			dMin=m_pEdge[j].m_dPriority;
		}
//		else if(m_pEdge[j].m_dPriority==dMin)
//			if(m_pEdge[m_pEdge[j].m_iNextEdge].m_dPriority<m_pEdge[m_pEdge[iMin].m_iNextEdge].m_dPriority)
//				iMin=j;
		
	}
	return iMin;
}
void CEdgeArray::RemoveEdge(UINT iEdge)
{
	m_pEdge[iEdge].m_bColapsed = true;

	UINT i = m_pEdge[iEdge].m_iIndex;
	UINT iNewEdge = m_aEdge[--m_nValidEdge];
	m_aEdge[i] = iNewEdge;
	m_pEdge[iNewEdge].m_iIndex=i;
}

void CEdgeArray::RestoreEdge(UINT iEdge)
{
	m_pEdge[iEdge].m_bColapsed = false;
	m_pEdge[iEdge].m_iIndex = m_nValidEdge;
	m_aEdge[m_nValidEdge++] = iEdge;
	
}

CEdgeArray& CEdgeArray::operator = (CEdgeArray& ea)
{
	m_nTotalEdge = ea.m_nTotalEdge;
	m_aEdge = new UINT[m_nTotalEdge];//array
	for(UINT i=0;i<m_nTotalEdge;i++)
		m_aEdge[i] = ea.m_aEdge[i];
	m_pEdge = ea.m_pEdge;
	m_nValidEdge = ea.m_nValidEdge;//left edge number

	return *this;
}
//////////////////////////////////////////////////////
//						CMesh						//
//////////////////////////////////////////////////////
void CMesh::clear()
{
	if(m_pVertex!=NULL)	{delete[] m_pVertex;m_pVertex=NULL;}
	if(m_pEdge!=NULL)	{delete[] m_pEdge;m_pEdge=NULL;}
	if(m_pWedge!=NULL)	{delete[] m_pWedge;m_pWedge=NULL;}
	if(m_pFace!=NULL)	{delete[] m_pFace;m_pFace=NULL;}
	m_nVertex=m_nEdge=m_nWedge=m_nFace=0;
	m_lFocusEdge.clear();
	m_lFocusVertex.clear();
}

CMesh::CMesh(CMesh* pMesh)
{
	int i;
	m_nVertex = pMesh->m_nVertex;				//����
	m_pVertex = new CVertex[m_nVertex];			//���
	for(UINT i=0;i<m_nVertex;i++)
		m_pVertex[i] = pMesh->m_pVertex[i];

	m_nEdge = pMesh->m_nEdge;					//����
	m_pEdge = new CEdge[m_nEdge]; 				//�߱�
	for(i=0;i<m_nEdge;i++)
		m_pEdge[i] = pMesh->m_pEdge[i];
	m_nFace = pMesh->m_nFace;	 				//����
	m_pFace = new CFace[m_nFace];				//���
	for(i=0;i<m_nFace;i++)
		m_pFace[i] = pMesh->m_pFace[i];

	m_lFocusEdge = pMesh->m_lFocusEdge;
//	_UINTLIST m_lPickedVertex;
	m_iPickedFace=m_iPickedEdge=m_iPickedVertex=-1;


}
CMesh::~CMesh()
{
	clear();	
}

BOOL CMesh::Load(CString sFileName)
{
	clear();
	CString sExt = sFileName.Right(4);
	sExt.MakeLower();
	if (sExt == ".smf")
		return (loadFromSMF(sFileName));
	if (sExt == ".obj")
		return (loadFromSMF(sFileName));
	if (sExt == ".dat")
		return (loadFromSMF(sFileName));
	if (sExt == ".wrl")
		return (loadFromWRL(sFileName));
	if (sExt == ".tmp")
		return (loadFromTMP(sFileName));
	if (sExt == ".gts")
		return (loadFromGTS(sFileName));
	return FALSE;
}
BOOL CMesh::loadFromSMF(CString sFileName)
// -----  format: smf,obj,dat -----
//vertex:
//      v x y z,
//face(triangle):
//      f v1 v2 v3  (the vertex index is 1-based)
{
	
	//open the file
	FILE *f = NULL;
	if ((f = fopen(LPCTSTR(sFileName),"rb")) == NULL) return FALSE;

	char ch = 0;
	
	_VECTORLIST VertexList;//��ʱ�洢���е������
	_UINTLIST FaceList;//��ʱ�洢����������ĵ����Ϣ

	Vector3D vector;
	Vector3D vMin;
	Vector3D vMax;
	bool bFirst=true;

	UINT l[3];
	short j;

	ch=fgetc(f);
	while(ch>0)//����������Ϣ��ʱ����VertexList��FaceList��
	{
		switch(ch)
		{
		case 'v':	//vertex
			ch=fgetc(f);
			if((ch!=' ')&&(ch!='\t'))
				break;
			fscanf(f, "%lf%lf%lf",&vector.x,&vector.y,&vector.z);
			VertexList.push_back(Vector3D(vector));
			//�������е�İ�Χ��
			if(bFirst)
			{
				bFirst=false;
				vMin=vMax=vector;
				continue;
			}
			if(vector.x<vMin.x)
				vMin.x=vector.x;
			else if(vector.x>vMax.x)
				vMax.x=vector.x;
			if(vector.y<vMin.y)
				vMin.y=vector.y;
			else if(vector.y>vMax.y)
				vMax.y=vector.y;
			if(vector.z<vMin.z)
				vMin.z=vector.z;
			else if(vector.z>vMax.z)
				vMax.z=vector.z;
			break;
		case 'f':	//face
			ch=fgetc(f);
			if((ch!=' ')&&(ch!='\t'))
				break;
			fscanf(f, "%ld%ld%ld\n",&l[0],&l[1],&l[2]);
			for(j=0;j<3;j++)
				FaceList.push_back(l[j]-1);
			break;
		}
		ch=fgetc(f);
	}
	fclose(f);

	//���ݰ�Χ�н�������������ԭ�㣬ͬʱ���й�һ��
	vector=vMax-vMin;
	double d=__max(vector.x,vector.y);
	d=0.5*__max(d,vector.z);
	vector=0.5*(vMax+vMin);//ƫ����
	
	m_nVertex=VertexList.size();
	m_nFace=FaceList.size()/3;
	m_nEdge=3*m_nFace;

	//read vertices and faces
	m_pVertex = new CVertex[m_nVertex];
	if (m_pVertex==NULL) {clear(); return FALSE;}//out of memory
	m_pFace = new CFace[m_nFace];
	if (m_pFace == NULL) {clear(); return FALSE;}//out of memory

	UINT i;
	_VECTORLIST::iterator iVertex = VertexList.begin();
	_UINTLIST::iterator iFace = FaceList.begin();

	for(i=0;i<m_nVertex;i++)
		m_pVertex[i].m_vPosition=(*(iVertex++)-vector)/d;
	for(i=0;i<m_nFace;i++)
	{
		m_pFace[i].Create(3);
		for(j=0;j<3;j++)
			m_pFace[i].m_piVertex[j]=*iFace++;
	}

	VertexList.clear();
	FaceList.clear();
	return construct();


}

BOOL CMesh::loadFromWRL(CString sFileName)
// ----- format: wrl -----
//vertex:
//      x y z,
//		...
//		x y z;
//face:
//      v1,v2,v3,...,-1,  (the vertex index is 0-based)
//		...
//		v1,v2,v3,...,-1;
{
	
	//open the file
	FILE *f = NULL;
	if ((f = fopen(LPCTSTR(sFileName),"rb")) == NULL) return FALSE;

	_VECTORLIST VertexList;//��ʱ�洢���е������
	_UINTLIST FaceList;//��ʱ�洢����������ĵ����Ϣ

	Vector3D vector;
	Vector3D vMin;
	Vector3D vMax;
	bool bFirst=true;


	char ch = ',';
	while(ch==',')//read vertex information
	{
		fscanf(f, "%lf%lf%lf%ch",&vector.x,&vector.y,&vector.z,&ch);
		VertexList.push_back(Vector3D(vector));
		//�������е�İ�Χ��
		if(bFirst)
		{
			bFirst=false;
			vMin=vMax=vector;
			continue;
		}
		if(vector.x<vMin.x)
			vMin.x=vector.x;
		else if(vector.x>vMax.x)
			vMax.x=vector.x;
		if(vector.y<vMin.y)
			vMin.y=vector.y;
		else if(vector.y>vMax.y)
			vMax.y=vector.y;
		if(vector.z<vMin.z)
			vMin.z=vector.z;
		else if(vector.z>vMax.z)
			vMax.z=vector.z;
	}

	ch=',';
	UINT i;
	UINT l[MAX_VERTEX_PER_FACE];
	short j;
	m_nFace=0;
	m_nEdge=0;
	while(ch==',')//read face information
	{
		i=0;
		l[0]=0;
		while(l[i]!=-1)
			fscanf(f,"%ld%c",&(l[++i]),&ch);
		FaceList.push_back(i-1);
		for (j = 1; j < i; j++)
			FaceList.push_back(l[j]);
		m_nEdge+=i-1;
		m_nFace++;

	}

	fclose(f);

	//���ݰ�Χ�н�������������ԭ�㣬ͬʱ���й�һ��
	vector=vMax-vMin;
	double d=__max(vector.x,vector.y);
	d=0.5*__max(d,vector.z);
	vector=0.5*(vMax+vMin);//ƫ����
	
	m_nVertex=VertexList.size();

	//read vertices and faces
	m_pVertex = new CVertex[m_nVertex];
	if (m_pVertex==NULL) {clear(); return FALSE;}//out of memory
	m_pFace = new CFace[m_nFace];
	if (m_pFace == NULL) {clear(); return FALSE;}//out of memory


	_VECTORLIST::iterator iVertex = VertexList.begin();
	_UINTLIST::iterator iFace = FaceList.begin();

	for(i=0;i<m_nVertex;i++)
		m_pVertex[i].m_vPosition=(*(iVertex++)-vector)/d;
	short nType;
	for(i=0;i<m_nFace;i++)
	{
		nType=*iFace++;
		m_pFace[i].Create(nType);
		for(j=0;j<nType;j++)
			m_pFace[i].m_piVertex[j]=*iFace++;
	}

	VertexList.clear();
	FaceList.clear();
	return construct();

}

BOOL CMesh::loadFromTMP(CString sFileName)
// ----- format: tmp -----
//face type:
//      n
//vertex:
//      x y z
//face:
//      v1 v2 v3  (the vertex index is 1-based)
{
	
	//open the file
	FILE *f = NULL;
	if ((f = fopen(LPCTSTR(sFileName),"rb")) == NULL) return FALSE;

	//k-polygon
	UINT N;
	fscanf(f,"%ld",&N);
	//points
	fscanf(f,"%ld",&m_nVertex);

	m_pVertex = new CVertex[m_nVertex];
	if (m_pVertex == NULL) { clear();fclose(f);	return FALSE;}//out of memory

	UINT i;
	short j;
	for (i = 0; i < m_nVertex; i++)
	{
		fscanf(f,"%lf",&(m_pVertex[i].m_vPosition.x));
		fscanf(f,"%lf",&(m_pVertex[i].m_vPosition.y));
		fscanf(f,"%lf",&(m_pVertex[i].m_vPosition.z));
	}

	UINT iVertex;
	//polygons
	fscanf(f,"%ld",&m_nFace);
	m_pFace = new CFace[m_nFace];
	if (m_pFace == NULL) { clear(); fclose(f); return FALSE;}//out of memory
	for (i = 0; i < m_nFace; i++)
	{
		m_pFace[i].Create(N);
		for (j = 0; j < N; j++)
		{
			fscanf(f,"%ld",&iVertex);
			m_pFace[i].m_piVertex[j]=iVertex-1;
		}
	}

	fclose(f);

	m_nEdge=3*m_nFace;

	return construct();

}

BOOL CMesh::loadFromGTS(CString sFileName)
// ----- format: tmp -----
//      VertexNum SomethingNum FaceNum
//vertex:
//      x y z
//edge:
//		a b			(expressed with vertex index,which is 1-based)
//face:
//      e1 e2 e3	(expressed with edge index,which is 1-based)
{
	
	//open the file
	FILE *f = NULL;
	if ((f = fopen(LPCTSTR(sFileName),"rb")) == NULL) return FALSE;


	UINT i;
	//read vertex number,edge number,face number
	fscanf(f,"%ld%ld%ld",&m_nVertex,&m_nEdge,&m_nFace);

	m_pVertex = new CVertex[m_nVertex];
	if (m_pVertex == NULL) { clear();fclose(f);	return FALSE;}//out of memory
	UINT* pEdge[2];
	pEdge[0] = new UINT[m_nEdge];
	pEdge[1] = new UINT[m_nEdge];
	m_pFace = new CFace[m_nFace];
	if (m_pFace == NULL) { clear(); fclose(f); return FALSE;}//out of memory

	
	for (i = 0; i < m_nVertex; i++)
	{
		fscanf(f,"%lf",&(m_pVertex[i].m_vPosition.x));
		fscanf(f,"%lf",&(m_pVertex[i].m_vPosition.y));
		fscanf(f,"%lf",&(m_pVertex[i].m_vPosition.z));
	}
	UINT iVertex;
	for (i = 0; i < m_nEdge; i++)
	{
		fscanf(f,"%ld",&pEdge[0][i]);
		fscanf(f,"%ld",&pEdge[1][i]);
	}

	
	//faces
	UINT iEdge;
	for (i = 0; i < m_nFace; i++)
	{
		m_pFace[i].Create(3);
		fscanf(f,"%ld",&iEdge);
		m_pFace[i].m_piVertex[0]=pEdge[0][iEdge-1]-1;
		m_pFace[i].m_piVertex[1]=pEdge[1][iEdge-1]-1;

		fscanf(f,"%ld",&iEdge);
		iVertex=pEdge[0][iEdge-1]-1;
		if((iVertex!=m_pFace[i].m_piVertex[0])&&(iVertex!=m_pFace[i].m_piVertex[1]))
		{
			m_pFace[i].m_piVertex[2]=iVertex;
			fscanf(f,"%ld",&iEdge);
			continue;
		}
		iVertex=pEdge[1][iEdge-1]-1;
		if((iVertex!=m_pFace[i].m_piVertex[0])&&(iVertex!=m_pFace[i].m_piVertex[1]))
		{
			m_pFace[i].m_piVertex[2]=iVertex;
			fscanf(f,"%ld",&iEdge);
			continue;
		}

		fscanf(f,"%ld",&iEdge);
		iVertex=pEdge[0][iEdge-1]-1;
		if((iVertex!=m_pFace[i].m_piVertex[0])&&(iVertex!=m_pFace[i].m_piVertex[1]))
		{
			m_pFace[i].m_piVertex[2]=iVertex;
			continue;
		}
		iVertex=pEdge[1][iEdge-1]-1;
		if((iVertex!=m_pFace[i].m_piVertex[0])&&(iVertex!=m_pFace[i].m_piVertex[1]))
		{
			m_pFace[i].m_piVertex[2]=iVertex;
			continue;
		}
		
	}
	fclose(f);

	delete[] pEdge[0];
	delete[] pEdge[1];

	m_nEdge=3*m_nFace;
	return construct();

}

BOOL CMesh::Save(CString sFileName)
{
	CString sExt = sFileName.Right(4);
	sExt.MakeLower();
	if (sExt == ".smf")
		return (saveToSMF(sFileName));
	return FALSE;
}

BOOL CMesh::saveToSMF(CString sFileName)
// -----  format: smf,obj,dat -----
//vertex:
//      v x y z,
//face(triangle):
//      f v1 v2 v3  (the vertex index is 1-based)
{
	if ((m_pVertex==NULL)||(m_pFace==NULL)) return FALSE;//empty

	if(!reConstruct())
		return FALSE;

	// open the file
	FILE *f = NULL;
	if ((f = fopen(LPCTSTR(sFileName),"wb")) == NULL) return FALSE;

	// file header
	fprintf(f, "# vertices : %ld\r\n", m_nVertex);
	fprintf(f, "# faces    : %ld\r\n", m_nFace);
	fprintf(f, "\r\n");

	
	// vertices
	UINT i;
	for (i=0;i<m_nVertex;i++)
		fprintf(f, "v %.3lf %.3lf %.3lf\r\n", m_pVertex[i].m_vPosition.x, m_pVertex[i].m_vPosition.y, m_pVertex[i].m_vPosition.z);

	// faces
	for (i=0;i<m_nFace;i++)
		fprintf(f, "f %ld %ld %ld\r\n", m_pFace[i].m_piVertex[0]+1,m_pFace[i].m_piVertex[1]+1,m_pFace[i].m_piVertex[2]+1);

	fclose(f);

	return TRUE;
}

BOOL CMesh::reConstruct()
{
	_VECTORLIST VertexList;//��ʱ�洢���е������
	_UINTLIST FaceList;//��ʱ�洢����������ĵ����Ϣ

	Vector3D vector;
	Vector3D vMin;
	Vector3D vMax;
	bool bFirst=true;

	UINT i;
	UINT* old2new = new UINT[m_nVertex];
	UINT iNewIndex=0;
	for(i=0;i<m_nVertex;i++)
		if(!m_pVertex[i].m_bColapsed)
		{
			VertexList.push_back(m_pVertex[i].m_vPosition);
			old2new[i]=iNewIndex++;
		}

	for(i=0;i<m_nFace;i++)
		if(!m_pFace[i].m_bColapsed)
		{
			for(short j=0;j<3;j++)
				FaceList.push_back(old2new[m_pFace[i].m_piVertex[j]]);
		}

	clear();

	m_nVertex=VertexList.size();
	m_nFace=FaceList.size()/3;
	m_nEdge=3*m_nFace;
	//read vertices and faces
	m_pVertex = new CVertex[m_nVertex];
	if (m_pVertex==NULL) {clear(); return FALSE;}//out of memory
	m_pFace = new CFace[m_nFace];
	if (m_pFace == NULL) {clear(); return FALSE;}//out of memory

	_VECTORLIST::iterator iVertex = VertexList.begin();
	_UINTLIST::iterator iFace = FaceList.begin();

	for(i=0;i<m_nVertex;i++)
		m_pVertex[i].m_vPosition=*iVertex++;
	for(i=0;i<m_nFace;i++)
	{
		m_pFace[i].Create(3);
		for(short j=0;j<3;j++)
			m_pFace[i].m_piVertex[j]=*iFace++;
	}

	delete[] old2new;
	VertexList.clear();
	FaceList.clear();
	return construct();
}

BOOL CMesh::construct()
{
	if ((m_pVertex==NULL)||(m_pFace==NULL)) return FALSE;//empty

	if(m_pEdge!=NULL){delete[] m_pEdge;m_pEdge=NULL;}//delete old edgelist

	m_bClosed = true;

	UINT i;
	if(m_nEdge==0)//��load������Ӧ���Ѿ�����������������δ����������ɨ�������棬�õ��ܱ���
		for(i=0;i<m_nFace;i++)
			m_nEdge+=m_pFace[i].m_nType;

	m_pEdge = new CEdge[m_nEdge];
	
	short j,nType;

	UINT iEdge=0;
	UINT iVertex;

	for(i=0;i<m_nFace;i++)//ɨ��������
	{
		calFaceNormal(i);//˳��������ķ�������
		m_pFace[i].m_color=i*15;
		m_pFace[i].m_dSumArea=m_pFace[i].m_dArea;

		nType=m_pFace[i].m_nType;
		for(j=0;j<nType;j++)//��ÿ���潨����صĵ㡢�ߡ����������Ϣ
		{
			//���������Ϣ
			m_pFace[i].m_piEdge[j]=iEdge;//��˳���ÿ���߷�����
			//���������Ϣ
			iVertex=m_pFace[i].m_piVertex[j];
			m_pVertex[iVertex].m_nValence++;//������1
			m_pVertex[iVertex].m_lEdgeList.push_back(iEdge);//�Ƚ��ñ߼�������ʱ�߱�
			//�ߵ�������Ϣ
			m_pEdge[iEdge].m_iFace=i;
			m_pEdge[iEdge].m_iVertex[0]=iVertex;//Դ��
			m_pEdge[iEdge].m_iVertex[1]=m_pFace[i].m_piVertex[(j+1)%nType];//Ŀ���
			m_pEdge[iEdge].m_iNextEdge=iEdge+1;
			iEdge++;

		}
		m_pEdge[iEdge-1].m_iNextEdge-=nType;

	}

	_UINTLIST::iterator iv;
	UINT iSrcVertex,iDesVertex;
	bool bFlag;
	for(i=0;i<m_nEdge;i++)//ɨ�����бߣ��������бߵ�m_iTwinEdge�����ӹ�ϵ
	{
		if(m_pEdge[i].m_iTwinEdge!=-1)//�Ѿ������
			continue;
		iSrcVertex=m_pEdge[i].m_iVertex[0];//Դ��
		iDesVertex=m_pEdge[i].m_iVertex[1];//Ŀ���
		//������Ŀ��㷢�������бߣ���������û�лص�Դ��ı�
		bFlag=true;
		for (iv = m_pVertex[iDesVertex].m_lEdgeList.begin(); iv != m_pVertex[iDesVertex].m_lEdgeList.end(); iv++)
		{
			iEdge=*iv;
			if(m_pEdge[iEdge].m_iVertex[1]==iSrcVertex)
			{
				m_pEdge[i].m_iTwinEdge=iEdge;
				m_pEdge[iEdge].m_iTwinEdge=i;
				bFlag=false;
				break;
			}
		}
		//���û�У�˵��Դ���Ǳ߽��
		if(bFlag)
		{
			m_pVertex[iSrcVertex].m_bIsBoundary = true;
			m_bClosed = false;
		}

	}
	short nValence;
	UINT iTwinEdge;
	
	//��ʼ��Wedge�Ĵ�СΪ2����m_nVertex���Ա��Ժ������չ
	m_nWedge = m_nVertex;
	m_pWedge = new CWedge[2*m_nWedge];

	for(i=0;i<m_nVertex;i++)//ɨ�����е㣬����ÿ�����m_lEdgelist,��˳ʱ�뷽������m_piEdge,
	{
		//��i��Wedgeָ���i����
		m_pWedge[i].m_iVertex=i;

		nValence=m_pVertex[i].m_nValence;
		if(nValence<2)//ĳ��Ķ���С��2
			m_pVertex[i].m_bIsBoundary=true;

		if(nValence==0)//�����а��������ĵ㣬Ӧ�ý�ֹ
			continue;

		if(m_pVertex[i].m_piEdge!=NULL)
			delete[] m_pVertex[i].m_piEdge;
		m_pVertex[i].m_piEdge=new UINT[nValence];


//		if(m_pVertex[i].m_piEdge[0]==-1)//temp
//			return FALSE;

		if(m_pVertex[i].m_bIsBoundary)//�߽��
		{
			for (iv = m_pVertex[i].m_lEdgeList.begin(); iv != m_pVertex[i].m_lEdgeList.end(); iv++)
			{
				bFlag=true;
				m_pVertex[i].m_piEdge[0]=*iv;
				for(j=1;j<nValence;j++)
				{
					iTwinEdge=m_pEdge[m_pVertex[i].m_piEdge[j-1]].m_iTwinEdge;
					if(iTwinEdge==-1)//�߽�Edge
					{
						bFlag=false;
						break;
					}
					m_pVertex[i].m_piEdge[j]=m_pEdge[iTwinEdge].m_iNextEdge;
				}
				if(bFlag)//success
					break;
			}
		
		}
		else//�ڲ���
		{
			iv = m_pVertex[i].m_lEdgeList.begin();
			m_pVertex[i].m_piEdge[0]=*iv;
			for(j=1;j<nValence;j++)//���õ㷢����half edge˳ʱ�����m_piEdge
			{
				iTwinEdge=m_pEdge[m_pVertex[i].m_piEdge[j-1]].m_iTwinEdge;
//				if(iTwinEdge==-1)//temp
//					return FALSE;
				m_pVertex[i].m_piEdge[j]=m_pEdge[iTwinEdge].m_iNextEdge;
			}
		}
		m_pVertex[i].m_lEdgeList.clear();//�����ʱ����
		for(j=0;j<nValence;j++)
			m_pVertex[i].m_lEdgeList.push_back(m_pVertex[i].m_piEdge[j]);

		calVertexNormal(i);//���ݵ��������Ϣ����õ��ƽ������
//		calVertexMeanCurvature(i);
	}

	return TRUE;
}


void CMesh::calFaceNormal(UINT i)
{
	Vector3D v[2];

	//get the vector
	v[0] = m_pVertex[m_pFace[i].m_piVertex[2]].m_vPosition-m_pVertex[m_pFace[i].m_piVertex[0]].m_vPosition;

	if(m_pFace[i].m_nType==3)
		v[1] = m_pVertex[m_pFace[i].m_piVertex[2]].m_vPosition-m_pVertex[m_pFace[i].m_piVertex[1]].m_vPosition;
	else
		v[1] = m_pVertex[m_pFace[i].m_piVertex[3]].m_vPosition-m_pVertex[m_pFace[i].m_piVertex[1]].m_vPosition;
	m_pFace[i].m_vNormal=v[0]^v[1];

	if(m_pFace[i].m_nType==3)
		m_pFace[i].m_dArea=0.5*m_pFace[i].m_vNormal.normalize();//������λ��ͬʱ�����˸�����������
	else if(m_pFace[i].m_nType==4)
		m_pFace[i].m_dArea=m_pFace[i].m_vNormal.normalize();//������λ��ͬʱ�����˸��ı���������
	else
		m_pFace[i].m_vNormal.normalize();

	Vector3D vMassPoint;

	for(short j = 0; j < m_pFace[i].m_nType; j++)
		vMassPoint+=m_pVertex[m_pFace[i].m_piVertex[j]].m_vPosition;
	vMassPoint/=(double)(m_pFace[i].m_nType);
	m_pFace[i].m_vMassPoint=vMassPoint;

	

}

void CMesh::calVertexNormal(UINT i)
{
	Vector3D v;
	UINT iFace;
	short valence=m_pVertex[i].m_nValence;
	if(valence<1)
		return;
	for(short j=0;j<valence;j++)
	{
		iFace=m_pEdge[m_pVertex[i].m_piEdge[j]].m_iFace;
		v+=m_pFace[iFace].m_vNormal;
	}
	v/=(double)valence;
	m_pVertex[i].m_vNormal=v;

}

void CMesh::calVertexMeanCurvature(UINT i)
{
	short valence=m_pVertex[i].m_nValence;
	if(valence<1)
		return;

	UINT iEdge[2];
	UINT iFace;
	Vector3D vNow=m_pVertex[i].m_vPosition;
	Vector3D v[2];

	double Amixed=0.0;
	Vector3D vCurvature;

	for(short j=0;j<valence;j++)
	{
		iEdge[0]=m_pVertex[i].m_piEdge[j];
		if(j==valence-1)
		{
			if(m_pVertex[i].m_bIsBoundary)
				break;
			else
				iEdge[1]=m_pVertex[i].m_piEdge[0];
		}
		else
			iEdge[1]=m_pVertex[i].m_piEdge[j+1];
		iFace=m_pEdge[m_pVertex[i].m_piEdge[j]].m_iFace;

		v[0]=m_pVertex[m_pEdge[iEdge[0]].m_iVertex[1]].m_vPosition-vNow;
		v[1]=m_pVertex[m_pEdge[iEdge[1]].m_iVertex[1]].m_vPosition-vNow;
		double d=v[0]*v[1];
		double dCT[2];

		if(d<=0)
		{
			Amixed+=m_pFace[iFace].m_dArea/3.0;
			dCT[1]=(v[1].length()+d)/sqrt(v[0].length2()-d*d);
			dCT[0]=(v[0].length()+d)/sqrt(v[1].length2()-d*d);
		}
		else
		{
			
			dCT[0]=(v[1].length()-d)/sqrt(v[0].length2()-d*d);
			dCT[1]=(v[0].length()-d)/sqrt(v[1].length2()-d*d);
			Amixed+=(v[0].length2()*dCT[0]+v[1].length2()*dCT[1])/8.0;
		}

		vCurvature+=dCT[0]*v[0]+dCT[1]*v[1];
	}

	Amixed=0.5/Amixed;
	m_pVertex[i].m_vMeanCurvature=Amixed*vCurvature;

	double dTemp=m_pVertex[i].m_vMeanCurvature.length();

}

double CMesh::GetEdgeWeight(UINT iEdge)
{
	CEdge* pEdge=&(m_pEdge[iEdge]);
	CVertex* pVertex0=&(m_pVertex[pEdge->m_iVertex[0]]);
	double dArea=m_pFace[pEdge->m_iFace].m_dSumArea;
	pEdge = &(m_pEdge[pEdge->m_iTwinEdge]);
	dArea+=m_pFace[pEdge->m_iFace].m_dSumArea;
	return dArea;
}

double CMesh::GetEdgeLen(UINT iEdge)
{
	CVertex* pVertex[2];
	pVertex[0]=&(m_pVertex[m_pEdge[iEdge].m_iVertex[0]]);
	pVertex[1]=&(m_pVertex[m_pEdge[iEdge].m_iVertex[1]]);
		
	//get the vector
	Vector3D v = pVertex[0]->m_vPosition-pVertex[1]->m_vPosition;
	return v.length();
}

