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

//实体
Solid *solid;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;

bool wireframe = false;//渲染出线框模型

int color_faces[1000][3];//存储随机产生的各面颜色
int idx = 0;
GLUtesselator * obj = gluNewTess();

//内调函数
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

//按键输入处理回调函数
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

	//刷新窗口
	glutPostRedisplay();
}

//适应窗口大小
void changeSize(GLint w, GLint h)
{
	//横宽比率
	GLfloat ratio;
	//窗口宽高为零直接返回
	if ((w == 0) || (h == 0))
		return;

	//设置视口和窗口大小一致
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



//openGL初始化函数
bool InitGL()
{
	//色
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

	//获取solid 中的vertex对应的坐标
	double verCoord[1000][3];
	saveVertexToCoord(solid, verCoord);


	while (face)
	{
		//依次绘制各面
		gluTessBeginPolygon(obj, NULL);
		Loop *templp = face->floop;
		//采用随机产生的color来绘制该面
		unsigned char red = color_faces[index][0];
		unsigned char green = color_faces[index][1];
		unsigned char blue = color_faces[index][2];
		
		while (templp)
		{//遍历当前面的各点
			glColor3b(red, green, blue);

			HalfEdge *halfedge = templp->lhedg;
			Vertex *start = halfedge->startv;
			gluTessBeginContour(obj);
			gluTessVertex(obj, verCoord[start->num_vertex], verCoord[start->num_vertex]);

			Vertex *tmp = halfedge->next->startv;
			halfedge = halfedge->next;
			while (tmp != start) //逐点
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


//画线框模型
void drawWireFrame(Solid *solid)
{

	glLineWidth(3);
	Face *face = solid->sface;
	//获取solid 中的vertex对应的坐标
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

//渲染的主要函数
void renderScene()

{
	//把整个窗口清理为当前清理颜色：白色。清除深度缓冲区
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//深度剔除
	
	glEnable(GL_DEPTH_TEST);
	
	//旋转功能
	//将当前Matrix状态入栈
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	
	if (wireframe == false)
	{
		draw(solid);
	}
	//线框模型渲染绘制
	else if (wireframe == true)
	{
		drawWireFrame(solid);
	}


	glPopMatrix();
	glFlush();
	//交换两个缓冲区的指针
	glutSwapBuffers();
}


//对弹出菜单的处理函数
void Menu(int value)
{
	switch (value)
	{
	//显示为线框模型
	case 1:
		wireframe = true;
		break;
	//普通渲染
	case 2:
		wireframe = false;
		break;
	default:
		break;
	}

	//重新绘制
	glutPostRedisplay();

}

int main(int argc,char* argv[])
{
	//solid= getSolid1();//带两个通孔的实体

	solid = getSolid();//从“Input.txt”中读入文件，基于欧拉和扫成操作生成实体

	output(solid);
	int face_count = output2(solid);

	srand(time(NULL));//随机

	//对每个面产生一个随机颜色
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
	
	//初始化
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);//深度缓存，双缓存，RGB缓存
	glutInitWindowSize(600,600); //窗口大小
	glutInitWindowPosition(100,100);//窗口左上角坐标
	glutCreateWindow("render:by Connie Cai");
	InitGL();//环境初始化

	glutReshapeFunc(changeSize); //适应窗口大小改变
	glutSpecialFunc(specialKey);//设置按键输入处理回调函数 ??????

	glutDisplayFunc(renderScene); //渲染函数

    //创建菜单
	glutCreateMenu(Menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("线框模型", 1);//可以查看实体的线框模型
	glutAddMenuEntry("实体模型", 2);//普通渲染方式


	glutMainLoop();

	system("pause");
	return 0;
}

