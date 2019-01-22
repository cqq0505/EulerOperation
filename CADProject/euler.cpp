//author: connie cai
#include "HalfEdge.h"
#include "euler.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

bool cmp(Loop *a, Loop *b)
{
	return a->num_loop < b->num_loop;
}

//---------------ŷ���������庯��---------------

//mvfs����������һ��ʵ�壬�õ�һ����һ���㣬һ����
Solid* mvfs(Point *p)
{
	Solid *solid = new Solid;
	Face *face = new Face;
	Loop *loop = new Loop;
	Vertex *vertex = new Vertex;

	vertex->point = *p; 

	solid->addFace(face);
	solid->addVertex(vertex);
	face->addLoop(loop);
	//loop->lhedg = NULL; //�ջ����Ѿ�Ĭ�ϣ�����ɾ��

	return solid;
}

//mev����������һ���µ�,������㹹��һ���±�
HalfEdge* mev(Point * oldPoint, Point * newPoint, Loop * lp)
{
	Solid *solid = lp->lface->fsolid;
	Vertex *v2 = new Vertex;
	Edge *eg = new Edge;
	HalfEdge *halfEdge1= new HalfEdge;
	HalfEdge *halfEdge2 = new HalfEdge;

	Vertex *v1 = solid->findVertex(oldPoint);//old vertex

	//�ж�v1 ���Ƿ���solid��
	if (! v1)
	{
		cout << "the old point is not in the solid" << endl;
		return NULL;
	}

    v2->point = *newPoint; //new vertex

	//������߽ṹ����ӵ�lp��
	halfEdge1->edge =eg ;
	eg->he1 = halfEdge1;
	halfEdge1->hloop = lp;
	halfEdge1->startv = v1;
	halfEdge1->endv = v2;
	halfEdge1->adjacent = halfEdge2;

	halfEdge2->edge = eg;
	eg->he2 = halfEdge2;
	halfEdge2->hloop = lp;
	halfEdge2->startv = v2;
	halfEdge2->endv = v1;
	halfEdge2->adjacent = halfEdge1;

	halfEdge1->next = halfEdge2;
	halfEdge2->prev = halfEdge1;

	//add new verex and edge to solid
	solid->addVertex(v2);
	solid->addEdge(eg);

	HalfEdge *he = lp->lhedg;

	//lp���ױ�
	if (!he)
	{
		lp->lhedg = halfEdge1;
		halfEdge2->next = halfEdge1;
		halfEdge1->prev = halfEdge2;
	}
	else
	{	
		//����lp���ױ�

		//for (he = lp->lhedg;he->next->startv != v1;he = he->next);
		while (he->next->startv != v1)
		{
			he = he->next;
		}

		//�Ƚ���he2������ӣ�˳���ܴ�he->next���ں�
		halfEdge2->next = he->next;
		he->next->prev = halfEdge2;
		he->next = halfEdge1;
		halfEdge1->prev = he;

	}

	//cout << "v" << v1->num_vertex << endl;
	//cout << "v" << halfEdge1->startv->num_vertex <<","<< "v" <<halfEdge2->startv->num_vertex << endl;
	//cout << "v" << v1->num_vertex << ":" << v1->point.x << ","<< v1->point.y << ","<< v1->point.z<<endl;
	return halfEdge1;
	
}

//mef����������������乹��һ���±ߣ�ͬʱ����һ�����棬�Լ�һ����
//�⻷�������ַ���ָ������
//��V2Ϊ���Ļ���Ϊ�⻷(ԭlp)��he1���⻷�У�V1Ϊ���Ļ���Ϊ�ڻ���newLp����he2���ڻ���
//���ص�Ϊ�ڻ�
Loop* mef(Point *p1, Point *p2, Loop *lp)
{
	Solid *solid = lp->lface->fsolid;
	Face *face = new Face;
	Loop *newLp = new Loop;
	Edge *eg = new Edge;
	HalfEdge *he1 = new HalfEdge;
	HalfEdge *he2 = new HalfEdge;

	Vertex *v1 = solid->findVertex(p1);
	Vertex *v2 = solid->findVertex(p2);

	solid->addEdge(eg);
	solid->addFace(face);
	face->addLoop(newLp);

	//create new edge and HalfEdges
	he1->edge = eg;
	eg->he1 = he1;
	he1->startv = v1;
	he1->endv = v2;
	he1->adjacent = he2;

	he2->edge = eg;
	eg->he2 = he2;
	he2->startv = v2;
	he2->endv = v1;
	he2->adjacent = he1;

	he1->next = he2;
	he2->prev = he1;
	
	//�ҵ��ؼ��ļ�����ߣ���v1��v2Ϊ���İ��
	HalfEdge* tmp1 = lp->findHe(v1);
	HalfEdge* tmp2 = lp->findHe(v2);
	HalfEdge* tmp3 = tmp1->prev;
	
	//���� lp �� newLp
	//����������newLp,�ڻ�
	newLp->addHalfEdge(he2);
	HalfEdge* tmp = tmp1;
	//���İ�ߵ�lp����
	while (tmp)
	{
		if (tmp == tmp2)
			break;
		tmp->hloop = newLp;
		tmp = tmp->next;
	}
	he2->next = tmp1;
	tmp1->prev = he2;

	he2->prev = tmp2->prev;
	tmp2->prev->next = he2;
	

	//��������lp ���⻷
	//lp->addHalfEdge(he1);
	he1->prev = tmp3;
	tmp3->next = he1;
	he1->next = tmp2;
	tmp2->prev = he1;
	he1->hloop = lp;
	lp->lhedg = he1;

	return newLp;
}

//���� innerloop 
//kemr��������ȥһ���ߣ�����һ���ڻ�(innerLp),ԭ��Ϊ�⻷(lp)
Loop* kemr(Point *p1, Point *p2, Loop *lp)
{
	Solid *solid = lp->lface->fsolid;
	Face *face = lp->lface;
	Loop *innerLp = new Loop;
	HalfEdge *he1, *he2;

	Vertex* v1 = solid->findVertex(p1);
	Vertex* v2 = solid->findVertex(p2);

	face->addLoop(innerLp);

	//��ȡ��Ҫɾ���İ��
	he1 = lp->lhedg;
	while (he1)
	{
		if (he1->startv == v1 && he1->endv == v2)
		{
			break;
		}
		he1 = he1->next;
	}
	he2 = he1->adjacent;

	if (he1->next != he2)
	{

		//����������newLp
		innerLp->addHalfEdge(he1->next);
		HalfEdge* tmp = he1->next->next;

		//���İ�ߵ�loop����
		while (tmp != he2)
		{
			tmp->hloop = innerLp;
			tmp = tmp->next;
		}
		he1->next->prev = he2->prev;
		he2->prev->next = he1->next;

	}

	//����������lp,�⻷
	lp->lhedg = he1->prev;
	he1->prev->next = he2->next;
	he2->next->prev = he1->prev;

	//ɾ���ߣ����������
	Edge *deleteEdge = he1->edge;
	while (deleteEdge)
	{
		if (deleteEdge->he1 == he1 || deleteEdge->he2 == he1)
			break;
		deleteEdge = deleteEdge->nexte;
	}

	//�����м�
	if (deleteEdge->preve && deleteEdge->nexte)
	{
		deleteEdge->nexte->preve = deleteEdge->preve;
		deleteEdge->preve->nexte = deleteEdge->nexte;
		
	}
	//����ĩβ
	else if (deleteEdge->preve !=NULL && deleteEdge->nexte == NULL)
	{
		deleteEdge->preve->nexte = NULL;

	}

	//���ڿ�ʼ
	else if (deleteEdge->preve ==NULL && deleteEdge->nexte != NULL)
	{
		deleteEdge->nexte->preve = NULL;
		solid->sedge = deleteEdge->nexte;

	}
	else
	{
		solid->sedge = NULL;
	}

	delete deleteEdge;
	delete he1, he2;

	//cout << "v" << v1->num_vertex << ",v" << v2->num_vertex << endl;
	

	return innerLp;
}

//ɾ��һ���棬���䶨λΪ��һ���棨�⻷���棩�����ڻ��������й�����
void kfmrh(Loop *outerLp, Loop *innerLp)
{
	//���������ж�
	if (!outerLp || !innerLp)
	{
		cout << "the input loop is not available" << endl;
		return;
	}

	Face *face1 = outerLp->lface; 
	Face *face2 = innerLp->lface; //ɾ������
	face1->addLoop(innerLp);
	
	//ɾ����ʱ����Ҫ�ж������������ɾ�������ƣ�
	if (face2->prevf && face2->nextf )
	{
		face2->nextf->prevf = face2->prevf;
		face2->prevf->nextf = face2->nextf;
		
	}
	else if (face2->prevf !=NULL && face2->nextf ==NULL)
	{
		face2->prevf->nextf = NULL;
	}
	else if (face2->prevf == NULL && face2->nextf != NULL)
	{
		face2->nextf->prevf = NULL;
		outerLp->lface->fsolid->sface = face2->nextf;
	}
	else
	{
		outerLp->lface->fsolid->sface = NULL;
	}

	delete face2;
}

//ƽ��ɨ�ɲ���,�켣��Ϊһ��ֱ�ߣ��켣��Ϊԭ��͸�����ɨ�����ɵ�ֱ��
//�ο���ʦ�����α����
void sweeping(Face *face, Point *sweepPoint)
{
	Solid *solid = face->fsolid;
	Loop *lp = face->floop;

	Point  *firstV = new Point;
	Point *firstUp = new Point;
	Point *baseV = new Point;
	Point *baseUp = new Point;
	Point *prevUp = new Point;

	//����ÿ��lp
	for (;lp;lp = lp->nextl)
	{
		HalfEdge *he = lp->lhedg;
		HalfEdge *finalHe = he->prev;

		*firstV = he->startv->point;
		firstUp->x = firstV->x + sweepPoint->x;
		firstUp->y = firstV->y + sweepPoint->y;
		firstUp->z = firstV->z + sweepPoint->z;

		*prevUp = *firstUp; 
		mev(firstV, firstUp, lp);

		if (he == finalHe)
		{
			he = NULL;
		}
		else
		{
			he = he->next;
		}

		while (he)
		{
			*baseV = he->startv->point;
			baseUp->x = baseV->x + sweepPoint->x;
			baseUp->y = baseV->y + sweepPoint->y;
			baseUp->z = baseV->z + sweepPoint->z;

			mev(baseV, baseUp, lp);
			mef(prevUp, baseUp, lp);
			*prevUp = *baseUp;

			if (he == finalHe)
			{
				he = NULL;
				mef(prevUp, firstUp, lp);
			}
			else
			{
				he = he->next;
			}

		}
		/*
		he = he->next;
		while (he->startv != firstV)
		{
		*baseV = he->startv->point;
		baseUp->x = baseV->x + sweepPoint->x;
		baseUp->y = baseV->y + sweepPoint->y;
		baseUp->z = baseV->z + sweepPoint->z;

		mev(baseV,baseUp,lp);
		mef(prevUp,baseUp,lp);
		*prevUp = *baseUp;

		he = he->next;
		}
		//���һ����
		mef(prevUp,firstUp,lp);

		*/
		
	}
	delete firstV, firstUp, baseV, baseUp, prevUp;
}


//�����������ɨ�ɣ���input.txt�ļ�����ߵ���Ϣ��������������ɨ����
Solid* getSolid( )
{
	ifstream infile("Input.txt",ios::in);
	if (!infile.is_open())
	{
		cout << "error opening file" << endl;
	}

	int num;
	int polygon[10];
	string polygon_num,point_num;

	infile >> polygon_num;
	infile >> num;
	infile >> point_num;
	for (int i=0; i<num; i++)
	{
		infile >> polygon[i];
	}

	Point point;
	infile>> point.x >> point.y >> point.z;
	Solid* solid = mvfs(&point);//���ڵ�һ���㣬����mvfs����
	Loop* lp = solid->sface->floop;
	Loop* outerLp = lp;//�⻷

	Point firstp = point;//��¼��һ����,���ں�������
	Point prevp = point;

	for (int i=0; i<polygon[0]-1;i++)
	{
		infile >> point.x >> point.y >> point.z;
	    mev(&prevp, &point, lp);
		prevp = point;
	}

	lp = mef(&prevp,&firstp,lp);
	Loop *newLp = lp;//�µĻ�����Ϊ���ɺ����ڻ���Lp

	Point firstInnerP;

	//�������ɻ�����ڻ�
	for (int i=1; i<num; i++)
	{
		infile >> point.x >> point.y >> point.z;
		lp = newLp;
		mev(&firstp, &point, lp);
		lp = kemr(&firstp, &point, lp);
		firstInnerP = prevp = point;

		for (int j = 0; j<polygon[i] - 1; j++)
		{
			infile >> point.x >> point.y >> point.z;
			mev(&prevp, &point, lp);
			prevp = point;
		}
		Loop * innerLp = mef(&point, &firstInnerP, lp);
		kfmrh(outerLp, innerLp);

	}
	
	Point sweepPoint;
	infile >> sweepPoint.x >> sweepPoint.y >> sweepPoint.z;

	sweeping(solid->sface,&sweepPoint);

	infile.close();
	return solid;

}


//����һ��������ͨ�׵�ʵ��ľ��岽��
Solid * getSolid1()
{

	//���������
	vector<Point> outer;
	outer.push_back(Point(0.0, 0.0, 0.0));
	outer.push_back(Point(120.0, 0.0, 0.0));
	outer.push_back(Point(120.0, 100.0, 0.0));
	outer.push_back(Point(00.0, 100.0, 0.0));


	vector<Point> inner1;
	inner1.push_back(Point(30.0, 30.0, 0.0));
	inner1.push_back(Point(40.0, 30.0, 0.0));
	inner1.push_back(Point(40.0, 70.0, 0.0));
	inner1.push_back(Point(30.0, 70.0, 0.0));


	vector<Point> inner2;
	inner2.push_back(Point(60.0, 30.0, 0.0));
	inner2.push_back(Point(70.0, 30.0, 0.0));
	inner2.push_back(Point(70.0, 70.0, 0.0));
	inner2.push_back(Point(60.0, 70.0, 0.0));


	Solid *solid = mvfs(&outer[0]);
	Loop *lp = solid->sface->floop;
	Loop *outerLp = lp; //�⻷

	
	//���
	mev(&outer[0], &outer[1], lp);
	mev(&outer[1], &outer[2], lp);
	mev(&outer[2], &outer[3], lp);
	mef(&outer[3], &outer[0], lp);

	//��1
	lp = solid->sface->nextf->floop;
	mev(&outer[0], &inner1[0], lp);
	lp = kemr(&outer[0], &inner1[0], lp);

	mev(&inner1[0], &inner1[1], lp);
	mev(&inner1[1], &inner1[2], lp);
	mev(&inner1[2], &inner1[3], lp);
	Loop *innerLp1 = mef(&inner1[3], &inner1[0], lp);//��һ���ڻ�
	kfmrh(outerLp, innerLp1);

	//��2
	lp = solid->sface->nextf->floop;
	mev(&outer[0], &inner2[0], lp);
	lp = kemr(&outer[0], &inner2[0], lp);

	mev(&inner2[0], &inner2[1], lp);
	mev(&inner2[1], &inner2[2], lp);
	mev(&inner2[2], &inner2[3], lp);
	Loop *innerLp2 = mef(&inner2[3], &inner2[0], lp);//�ڶ����ڻ�

	kfmrh(outerLp, innerLp2);

	//ɨ�ɲ�����sweepPoint ������Զ�㹹�� ƽ��ɨ�ɵķ���;���
	Point *sweepPoint = new Point(0, 0, 30);
	sweeping(solid->sface, sweepPoint);

	return solid;
}


void saveVertexToCoord(Solid *solid, double verCoord[1000][3])
{
	Vertex *vertex = solid->svertex;
	for (; vertex; vertex = vertex->nextv)
	{
		verCoord[vertex->num_vertex][0] = vertex->point.x;
		verCoord[vertex->num_vertex][1] = vertex->point.y;
		verCoord[vertex->num_vertex][2] = vertex->point.z;
	}
}


//���ʵ�����Ϣ��������������ÿ�����Ӧ�Ļ���ÿ�����İ������
void output(Solid *solid)
{
	ofstream outfile;
	outfile.open("Output.txt", ios::out);
	outfile << "Solid: " << solid->num_solid << endl;
	Vertex *v = solid->svertex;
	Edge *e = solid->sedge;
	Face *f = solid->sface;
	int edge_count = 0;
	while (v)
	{
		outfile << "v" << v->num_vertex << " ";
		v = v->nextv;
	}
	outfile << endl;
	while (e)
	{
		edge_count++;
		e = e->nexte;
	}
	int face_count = 0;
	while (f)
	{
		face_count++;
		outfile << "face" << f->num_face << " ";
		
		f = f->nextf;
	}
	outfile << endl;
	outfile << "edge count:" << edge_count << endl;
	outfile << "face count:" << face_count << endl;

	Face *face = solid->sface;
	while (face)
	{
		outfile << "\n\nFace: " << face->num_face << endl;
		Loop *loop = face->floop;
		while (loop)
		{
			outfile << "\nLoop: " << loop->num_loop << "  " << endl;
			HalfEdge *he = loop->lhedg;
			while (he)
			{
				outfile <<"("<< "v"<<he->startv->num_vertex <<","<< "v" << he->endv->num_vertex <<")"<< " ";
				he = he->next;
				if (he == loop->lhedg)
					break;
			}
			loop = loop->nextl;
		}
		face = face->nextf;
	}
	outfile.close();
}


//���Ϊ.brp��ʽ
int output2(Solid *solid)
{
	ofstream outfile;
	Loop* loop_all[800];
	int loop_idx = 0;
	outfile.open("Connie.brp", ios::out);
	Face* face = solid->sface;
	int face_num = 0;
	int loop_num = 0;
	while (face)
	{
		face_num++;
		Loop *lp = face->floop;
		int time;
		while (lp)
		{
			time = 0;
			time++;
			loop_num++;
			loop_all[loop_idx++] = lp;
			lp = lp->nextl;
		}
		face = face->nextf;
	}

	Vertex *vertex = solid->svertex;
	int vertex_num = 0;
	while (vertex)
	{
		vertex_num++;
		vertex = vertex->nextv;
	}
	sort(loop_all, loop_all + loop_idx, cmp);
	outfile << "BRP" << endl;
	outfile << vertex_num << " " << loop_num << " " << face_num << " " << 1 << endl;
	vertex = solid->svertex;
	while (vertex)
	{
		outfile << vertex->point.x << " " << vertex->point.y << " " << vertex->point.z << endl;
		vertex = vertex->nextv;
	}
	outfile << endl;
	for (int i = 0; i < loop_idx; i++)
	{
		Loop * lp = loop_all[i];
		HalfEdge *he = lp->lhedg;
		int time = 0;
		while (he)
		{
			time++;
			he = he->next;
			if (he == lp->lhedg)
				break;
		}
		outfile << time << " ";
		while (he)
		{
			Vertex* vertex = he->startv;
			outfile << vertex->num_vertex << " ";
			he = he->next;
			if (he == lp->lhedg)
				break;
		}
		outfile << endl;
	}

	face = solid->sface;
	outfile << endl;
	while (face)
	{
		Loop *lp = face->floop;
		int time = 0;
		outfile << face->num_face<< " ";
		while (lp)
		{
			time++;
			loop_all[loop_idx++] = lp;
			lp = lp->nextl;
		}
		if (time == 1)
		{
			outfile << 0 << " ";
		}
		else if (time > 1)
		{
			outfile << time - 1 << " ";
			int temp = time - 1;
			int k = 1;
			while (temp--)
			{
				outfile << loop_all[loop_idx - k]->num_loop << " ";
				k++;
			}
		}
		outfile << 0 << " " << endl;
		face = face->nextf;
	}

	outfile.close();
	return face_num;


}
