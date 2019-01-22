//author :connie cai
#include "euler.h"
#include "HalfEdge.h"
#include <iostream>
#include <GL\glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>

using namespace std;

//ʵ��
Solid *solid;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;

bool wireframe = false;//��Ⱦ���߿�ģ��

int color_faces[1000][3];//�洢��������ĸ�����ɫ
int idx = 0;
GLUtesselator * obj = gluNewTess();

//�ڵ�����
void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}
void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *estring;
	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}
void CALLBACK endCallback(void)
{
	glEnd();
}
void CALLBACK vertexCallback(GLvoid *vertex)
{
	const GLdouble *pointer;
	pointer = (GLdouble *)vertex;
	glVertex3dv(pointer);
}

//�������봦��ص�����
void specialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN) xRot += 5.0f;
	if (key == GLUT_KEY_LEFT) yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT) yRot += 5.0f;
	if (key == GLUT_KEY_PAGE_UP) zRot -= 5.0f;
	if (key == GLUT_KEY_PAGE_DOWN) zRot += 5.0f;

	if (xRot > 356.0f) xRot = 0.0f;
	if (xRot < -1.0f) xRot = 355.0f;
	if (yRot > 356.0f) yRot = 0.0f;
	if (yRot < -1.0f) yRot = 355.0f;
	if (zRot > 356.0f) zRot = 0.0f;
	if (zRot < -1.0f) zRot = 355.0f;

	//ˢ�´���
	glutPostRedisplay();
}

//��Ӧ���ڴ�С
void changeSize(GLint w, GLint h)
{
	//������
	GLfloat ratio;
	//���ڿ��Ϊ��ֱ�ӷ���
	if ((w == 0) || (h == 0))
		return;

	//�����ӿںʹ��ڴ�Сһ��
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat nRange = 150.0f;
	ratio =(GLfloat) w / (GLfloat)h;

	if (w <= h)
		glOrtho(-nRange, nRange, -nRange/ ratio, nRange / ratio, -nRange, nRange);
	else
		glOrtho(-nRange * ratio, nRange * ratio, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



//openGL��ʼ������
bool InitGL()
{
	//ɫ
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluTessCallback(obj, GLU_TESS_VERTEX, (void (CALLBACK *)())vertexCallback);
	gluTessCallback(obj, GLU_TESS_BEGIN, (void (CALLBACK *)())beginCallback);
	gluTessCallback(obj, GLU_TESS_END, (void (CALLBACK *)())endCallback);
	gluTessCallback(obj, GLU_TESS_ERROR, (void (CALLBACK *)())errorCallback);
	return true;
}

void draw(Solid *solid)
{
	Face *face = solid->sface;
	int index = 0;

	//��ȡsolid �е�vertex��Ӧ������
	double verCoord[1000][3];
	saveVertexToCoord(solid, verCoord);


	while (face)
	{
		//���λ��Ƹ���
		gluTessBeginPolygon(obj, NULL);
		Loop *templp = face->floop;
		//�������������color�����Ƹ���
		unsigned char red = color_faces[index][0];
		unsigned char green = color_faces[index][1];
		unsigned char blue = color_faces[index][2];
		
		while (templp)
		{//������ǰ��ĸ���
			glColor3b(red, green, blue);

			HalfEdge *halfedge = templp->lhedg;
			Vertex *start = halfedge->startv;
			gluTessBeginContour(obj);
			gluTessVertex(obj, verCoord[start->num_vertex], verCoord[start->num_vertex]);

			Vertex *tmp = halfedge->next->startv;
			halfedge = halfedge->next;
			while (tmp != start) //���
			{
				gluTessVertex(obj, verCoord[tmp->num_vertex], verCoord[tmp->num_vertex]);
				halfedge = halfedge->next;
				tmp = halfedge->startv;
			}
			gluTessEndContour(obj);
			templp = templp->nextl;
		}
		gluTessEndPolygon(obj);
		face = face->nextf;
		index++;
	}
}


//���߿�ģ��
void drawWireFrame(Solid *solid)
{

	glLineWidth(3);
	Face *face = solid->sface;
	//��ȡsolid �е�vertex��Ӧ������
	double verCoord[1000][3];
	saveVertexToCoord(solid, verCoord);
	
	while (face)
	{
		
		Loop *templp = face->floop;
		while (templp)
		{
			HalfEdge *halfedge = templp->lhedg;
			Vertex *start = halfedge->startv;
			Vertex * end = halfedge->endv;
			glColor3f(0.0, 1.0, 0.0);

			//cout << verCoord[start->num_vertex][0] << "," << verCoord[start->num_vertex][1] << "," << verCoord[start->num_vertex][2] << endl;

			glBegin(GL_LINES);
			glVertex3f(verCoord[start->num_vertex][0], verCoord[start->num_vertex][1], verCoord[start->num_vertex][2]);
			glVertex3f(verCoord[end->num_vertex][0], verCoord[end->num_vertex][1], verCoord[end->num_vertex][2]);
			glEnd();


			halfedge = halfedge->next;
			Vertex *tmp = halfedge->startv;
			Vertex *tmp1 = halfedge->endv;
			while (tmp != start)
			{
				glColor3f(0.0, 1.0, 0.0);
				
				glBegin(GL_LINES);
				glVertex3f(verCoord[tmp->num_vertex][0], verCoord[tmp->num_vertex][1], verCoord[tmp->num_vertex][2]);
				glVertex3f(verCoord[tmp1->num_vertex][0], verCoord[tmp1->num_vertex][1], verCoord[tmp1->num_vertex][2]);
				glEnd();

				halfedge = halfedge->next;
				tmp = halfedge->startv;
				tmp1 = halfedge->endv;
			}
			templp = templp->nextl;
		}
		face = face->nextf;
	}
}

//��Ⱦ����Ҫ����
void renderScene()

{
	//��������������Ϊ��ǰ������ɫ����ɫ�������Ȼ�����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//����޳�
	
	glEnable(GL_DEPTH_TEST);
	
	//��ת����
	//����ǰMatrix״̬��ջ
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	
	if (wireframe == false)
	{
		draw(solid);
	}
	//�߿�ģ����Ⱦ����
	else if (wireframe == true)
	{
		drawWireFrame(solid);
	}


	glPopMatrix();
	glFlush();
	//����������������ָ��
	glutSwapBuffers();
}


//�Ե����˵��Ĵ�����
void Menu(int value)
{
	switch (value)
	{
	//��ʾΪ�߿�ģ��
	case 1:
		wireframe = true;
		break;
	//��ͨ��Ⱦ
	case 2:
		wireframe = false;
		break;
	default:
		break;
	}

	//���»���
	glutPostRedisplay();

}

int main(int argc,char* argv[])
{
	//solid= getSolid1();//������ͨ�׵�ʵ��

	solid = getSolid();//�ӡ�Input.txt���ж����ļ�������ŷ����ɨ�ɲ�������ʵ��

	output(solid);
	int face_count = output2(solid);

	srand(time(NULL));//���

	//��ÿ�������һ�������ɫ
	for (int i = 0; i < face_count; i++)
	{
		unsigned char red = rand() % 255;
		unsigned char green = rand() % 255;
	    unsigned char blue = rand() % 255;
	    color_faces[idx][0] = red;
	    color_faces[idx][1] = green;
	    color_faces[idx][2] = blue;
	    idx++;
	}
	
	//��ʼ��
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);//��Ȼ��棬˫���棬RGB����
	glutInitWindowSize(600,600); //���ڴ�С
	glutInitWindowPosition(100,100);//�������Ͻ�����
	glutCreateWindow("render:by Connie Cai");
	InitGL();//������ʼ��

	glutReshapeFunc(changeSize); //��Ӧ���ڴ�С�ı�
	glutSpecialFunc(specialKey);//���ð������봦��ص����� ??????

	glutDisplayFunc(renderScene); //��Ⱦ����

    //�����˵�
	glutCreateMenu(Menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("�߿�ģ��", 1);//���Բ鿴ʵ����߿�ģ��
	glutAddMenuEntry("ʵ��ģ��", 2);//��ͨ��Ⱦ��ʽ


	glutMainLoop();

	system("pause");
	return 0;
}

