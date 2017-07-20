///////////////////////class CScene///////////////////////////////////
//						��������֯�����ƣ�����						//
//////////////////////////////////////////////////////////////////////
#ifndef _SCENE_H_
#define _SCENE_H_

#include "..\model\mesh.h"
#include "display.h"

#define MAX_MODEL_NUM 0xff

//��������Ⱦ��ʽ
#define RENDER_INVISIBLE		0	//���ɼ�
#define RENDER_WIREFRAME		1	//�߿�
#define RENDER_HIDENLINE		2	//������
#define RENDER_FLATSHADE		3	//flat shade
#define RENDER_FLATSHADE_LINE	4	//flat shade with line
#define RENDER_SMOOTHSHADE		5	//smooth shade
#define RENDER_FOCUS			6	//focus edge
#define RENDER_TEXTURE			7	//with texture

//ʰȡ״̬
#define PICK_NONE	0
#define PICK_VERTEX	1
#define PICK_EDGE	2
#define PICK_FACE	3


class CScene
{
public:
	CScene();
	virtual ~CScene();
	BOOL	Create(HDC hDC);
	BOOL	Create(HWND hWnd);
	void	DrawScene();
	BOOL	AddMesh(CMesh* pNewMesh);
	
	//�ӽǱ任
	void	Move(int x,int y);
	void	Rotate(double a,double b);
	void	Scale(int d);
	void	MouseMove(UINT nFlags, int cx, int cy);//����ƶ�ʱ�任�ӽ�
	void	Reshape(int cx,int cy);//�ı�viewport�Ĵ�С

	//�ı䳡������Ⱦ��ʽ,����������һ������Ⱦ��ʽ
	void	SetRenderMode(BYTE mode);

	//�ı����
	void	SetMeterial(BYTE meterial);

	//ʰȡ
	void	SetPickMode(BYTE mode);//����ʰȡ����
	void	Pick(int x, int y);//���ʰȡ����

private:
	void	DrawMesh(CMesh *pMesh, bool bFlat);
	void	DrawPMesh(CMesh *pMesh, bool bFlat, bool bColor, bool bTexture);//Progressive Mesh
	void	DrawFace(CMesh *pMesh, bool bPick);
	void	DrawEdge(CMesh *pMesh, bool bPick);
	void	DrawVertex(CMesh *pMesh, bool bPick);
	void	DrawFocus(CMesh *pMesh);

	void	PickFace(CMesh *pMesh, int n,UINT *pLong);
	void	PickEdge(CMesh *pMesh, int n,UINT *pLong);
	void	PickVertex(CMesh *pMesh, int n,UINT *pLong);

	void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID);

public:
	CDisplay m_Display;//��ʾ�ӿ�,����ѡ����ʲô��ʽ��ʾ,OPENGL or DIRECT3D
	CMesh*	m_MeshPointers[MAX_MODEL_NUM];//����ģ�͵�ָ��
	

private:
	BYTE	m_MeshNum;//������ģ�͵ĸ���
	BYTE	m_RenderMode;//��������Ⱦ��ʽ,����������һ������Ⱦ��ʽ
	BYTE	m_Meterial;
	CMesh*	m_pCurrentMesh;//��ǰѡ�е�ģ��

	//��׶
	Vector3D m_vEyePoint;
	Vector3D m_vGazePoint;
	Vector3D m_vUp;
	Vector3D m_vFront;
	double	m_dDistance;

	//viewport
	int		m_nWidth;
	int		m_nHeight;

	//��Դ��Ŀǰ�涨ֻ��һ����Դ
	Vector3D m_vLight;

	//ʰȡ״̬
	BYTE	m_PickMode;//ʰȡ����

};

#endif