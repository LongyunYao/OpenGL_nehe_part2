// ����.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "����.h"
#include <stdio.h>			// Header File For Standard Input/Output
// Win32��Ŀ.cpp : ����Ӧ�ó������ڵ㡣
//
#pragma comment(lib,"gltools.lib")

#include <GL/glew.h>
#include <GL/glut.h>  
#include <GL/glaux.h>		// Header File For The Glaux Library
#pragma comment (lib, "winmm.lib")       /* link with Windows MultiMedia lib */
#pragma comment (lib, "opengl32.lib")   /* link with Microsoft OpenGL lib */
#pragma comment (lib, "glu32.lib")       /* link with OpenGL Utility lib */
#pragma comment (lib, "glut32.lib")      /* link with Win32 GLUT lib */ 


#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

												// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


HGLRC           hRC = NULL;		// ������ɫ��������   
HDC             hDC = NULL;		// OpenGL��Ⱦ��������   
HWND            hWnd = NULL;	// �������ǵĴ��ھ��   
HINSTANCE       hInstance;		// ��������ʵ�� 
bool    keys[256];				// ������̰���������   
bool    active = TRUE;			// ���ڵĻ��־��ȱʡΪTRUE   
bool    fullscreen = TRUE;		// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ 

GLfloat	rtri;				// Angle For The Triangle ( NEW )
GLfloat	rquad;				// Angle For The Quad ( NEW )

GLfloat	xrot;				// X Rotation ( NEW )
GLfloat	yrot;				// Y Rotation ( NEW )
GLfloat	zrot;				// Z Rotation ( NEW )

GLuint	texture[1];			// Storage For One Texture ( NEW )

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc


AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File = NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File = fopen(Filename, "r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}


int LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	int Status = FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage, 0, sizeof(void *) * 1);           	// Set The Pointer To NULL

															// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0] = LoadBMP("Data/NeHe.bmp"))
	{
		Status = TRUE;									// Set The Status To TRUE

		glGenTextures(1, &texture[0]);					// Create The Texture

														// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (TextureImage[0])									// If Texture Exists
	{
		if (TextureImage[0]->data)							// If Texture Image Exists
		{
			free(TextureImage[0]->data);					// Free The Texture Image Memory
		}

		free(TextureImage[0]);								// Free The Image Structure
	}

	return Status;										// Return The Status
}


GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)// ��ֹ�����   
	{
		height = 1;										// Making Height Equal One
	}
	//���漸��Ϊ͸��ͼ������Ļ����ζ��ԽԶ�Ķ���������ԽС��
	//��ô��������һ����ʵ��۵ĳ������˴�͸�Ӱ��ջ��ڴ��ڿ�Ⱥ͸߶ȵ�45���ӽ������㡣
	//0.1f��100.0f�������ڳ��������ܻ�����ȵ������յ㡣
	glViewport(0, 0, width, height);					// ���õ�ǰ���ӿ� 
														//glMatrixMode(GL_PROJECTION)ָ�������������д��뽫Ӱ��projection matrix(ͶӰ����)��
														//ͶӰ������Ϊ���ǵĳ�������͸�ӡ� 
	glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����   
														//glLoadIdentity()���������á�
														//������ѡ�ľ���״̬�ָ�����ԭʼ״̬��
														//���� glLoadIdentity()֮������Ϊ��������͸��ͼ��
	glLoadIdentity();									// ����ͶӰ����   

														// �����ӿڵĴ�С   
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	//glMatrixMode(GL_MODELVIEW)ָ���κ��µı任����Ӱ�� modelview matrix(ģ�͹۲����)��
	glMatrixMode(GL_MODELVIEW);							// ѡ��ģ�͹۲����   
	glLoadIdentity();									// ����ģ�͹۲����   
}

/*
���µĴ�����У����ǽ���OpenGL�������е����á�
���ǽ����������Ļ���õ���ɫ������Ȼ��棬����smooth shading(��Ӱƽ��)���ȵȡ�
�������ֱ��OpenGL���ڴ���֮��Żᱻ���á�
�˹��̽��з���ֵ��
�����Ǵ˴��ĳ�ʼ��û��ô���ӣ����ڻ��ò��ŵ����������ֵ��
*/
int InitGL(GLvoid)										// �˴���ʼ��OpenGL������������
{
	if (!LoadGLTextures())								// Jump To Texture Loading Routine ( NEW )
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glShadeModel(GL_SMOOTH);							// ������Ӱƽ�� 
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// ��ɫ���� 
														//�����������б��������ǹ���depth buffer(��Ȼ���)�ġ�
	glClearDepth(1.0f);									// ������Ȼ���
	glEnable(GL_DEPTH_TEST);							// ������Ȳ���   
	glDepthFunc(GL_LEQUAL);								// ������Ȳ��Ե�����
														//���Ÿ���OpenGL����ϣ��������õ�͸��������
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// ����ϵͳ��͸�ӽ�������
	return TRUE;										// Initialization Went OK
}

/*
��һ�ΰ��������еĻ�ͼ���롣
�κ�����������Ļ����ʾ�Ķ��������ڴ˶δ����г��֡�
*/
int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The View
	glTranslatef(0.0f, 0.0f, -5.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
	// Front Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	xrot += 0.3f;
	yrot += 0.2f;
	zrot += 0.4f;
	return TRUE;										// Keep Going
}

/*
��һ�δ���ֻ�ڳ����˳�֮ǰ���á�
KillGLWindow() �������������ͷ���ɫ�������豸������ʹ��ھ����
���Ѿ��������������顣
��������޷����ٴ��ڵ����ⲿ�֣����ᵯ������Ӧ������Ϣ��ѶϢ���ڣ�������ʲô�����ˡ�
ʹ�������Ĵ����в���ø�����Щ��
*/
GLvoid KillGLWindow(GLvoid)								// �������ٴ���
{
	if (fullscreen)										// ���Ǵ���ȫ��ģʽ��?
	{
		//��NULL��Ϊ��һ��������0��Ϊ�ڶ�����������ǿ��Windowsʹ�õ�ǰ�����ע����е�ֵ(ȱʡ�ķֱ��ʡ�ɫ����ȡ�ˢ��Ƶ�ʣ��ȵ�)����Ч�Ļָ����ǵ�ԭʼ���档
		ChangeDisplaySettings(NULL, 0);					// �ǵĻ����л�������
		ShowCursor(TRUE);								// ��ʾ���ָ��
	}
	//�������Ĵ���鿴�����Ƿ�ӵ����ɫ������(hRC)
	if (hRC)											// ����ӵ��OpenGL��Ⱦ��������? 
	{
		//���������ɫ������Ļ�������Ĵ��뽫�鿴�����ܷ��ͷ���(�� hRC��hDC�ֿ�)��
		//������ע����ʹ�õĵĲ������
		//��������ֻ���ó������ͷ���ɫ������(ͨ������wglMakeCurrent(NULL,NULL)��Ȼ�����ٲ鿴�ͷ��Ƿ�ɹ���
		//����Ľ����д����ϵ���һ�С�
		if (!wglMakeCurrent(NULL, NULL))				// �����ܷ��ͷ�DC��RC������? 
		{
			MessageBox(NULL, "�ͷ�DC��RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// �����ܷ�ɾ��RC?
		{
			MessageBox(NULL, "�ͷ�RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// ��RC��Ϊ NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// �����ܷ��ͷ� DC? 
	{
		MessageBox(NULL, "�ͷ�DCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// �ܷ����ٴ���?
	{
		MessageBox(NULL, "�ͷŴ��ھ��ʧ�ܡ�", "�ر�ʧ��", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;									// �� hWnd ��Ϊ NULL
	}
	//���Ҫ��������ע�����ǵĴ����ࡣ
	//�����������������ٴ��ڣ������ڴ���������ʱ�������յ�����"Windows Class already registered"(��������ע��)�Ĵ�����Ϣ��
	if (!UnregisterClass("OpenGL", hInstance))			// �ܷ�ע����?
	{
		MessageBox(NULL, "����ע�������ࡣ", "�رմ���", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;								// �� hInstance ��Ϊ NULL
	}
}

/*
�˹��̷��ز�������(TRUE �� FALSE)��
��������5�����������ڵı����������ڵĿ�ȣ����ڵĸ߶ȣ�ɫ��λ��(16/24/32)����ȫ����־(TRUE --ȫ��ģʽ�� FALSE--����ģʽ )��
���صĲ���ֵ�������Ǵ����Ƿ�ɹ�������

This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*
*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	//������Ҫ��WindowsΪ����Ѱ����ƥ������ظ�ʽʱ��WindowsѰ�ҽ�����ģʽֵ�����ڱ���PixelFormat�С�
	GLuint		PixelFormat;			// �������ƥ��Ľ��
										//wc�����������ǵĴ�����Ľṹ��������ṹ�б��������ǵĴ�����Ϣ��ͨ���ı���Ĳ�ͬ�ֶ����ǿ��Ըı䴰�ڵ���ۺ���Ϊ��
										//ÿ�����ڶ�����һ�������ࡣ������������ʱ��������Ϊ����ע���ࡣ
	WNDCLASS	wc;						// ������ṹ
										//dwExStyle��dwStyle�����չ��ͨ���Ĵ��ڷ����Ϣ��
										//��ʹ�ñ�������ŷ���Ŀ����Ϊ���ܹ���������Ҫ�����Ĵ�������(��ȫ��Ļ�µĵ������ڻ��Ǵ���ģʽ�µĴ��߿����ͨ����)��
										//���ı䴰�ڵķ��
	DWORD		dwExStyle;				// ��չ���ڷ�� 
	DWORD		dwStyle;				// ���ڷ��
										//�����5�д���ȡ�þ��ε����ϽǺ����½ǵ�����ֵ��
										//���ǽ�ʹ����Щֵ���������ǵĴ���ʹ�����ϵĻ�ͼ���Ĵ�Сǡ������������ķֱ��ʵ�ֵ��
										//ͨ��������Ǵ���һ��640x480�Ĵ��ڣ����ڵı߿��ռ��һЩ�ֱ��ʵ�ֵ��
	RECT		WindowRect;				// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left = (long)0;			// ��Left��Ϊ 0
	WindowRect.right = (long)width;		// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top = (long)0;			// ��Top��Ϊ 0
	WindowRect.bottom = (long)height;	// ��Bottom ��ΪҪ��ĸ߶�

	fullscreen = fullscreenflag;		// ����ȫ��ȫ����־

										//��һ���ֵĴ����У�����ȡ�ô��ڵ�ʵ����Ȼ���崰���ࡣ 
										//CS_HREDRAW �� CS_VREDRAW ����˼�����ۺ�ʱ��ֻҪ���ڷ����仯ʱ��ǿ���ػ���
	hInstance = GetModuleHandle(NULL);				// ȡ�����Ǵ��ڵ�ʵ��
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc = (WNDPROC)WndProc;				// WndProc������Ϣ
	wc.cbClsExtra = 0;								// �޶��ⴰ������
	wc.cbWndExtra = 0;								// �޶��ⴰ������
	wc.hInstance = hInstance;						// ����ʵ��
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// װ��ȱʡͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);		// װ�����ָ��
	wc.hbrBackground = NULL;						// GL����Ҫ����
	wc.lpszMenuName = NULL;							// ����Ҫ�˵�
	wc.lpszClassName = "OpenGL";					// �趨������

													//����ע�������֡�����д�����������������Ϣ���ڡ�
													//���������OK��ť�󣬳����˳�
	if (!RegisterClass(&wc))									// ����ע�ᴰ����
	{
		MessageBox(NULL, "ע�ᴰ��ʧ��", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// �˳�������FALSE 
	}
	/*
	���л���ȫ��ģʽʱ���м���ʮ����Ҫ�����������μǡ�
	����ȷ������ȫ��ģʽ�����õĿ�Ⱥ͸߶ȵ�ͬ�ڴ���ģʽ�µĿ�Ⱥ͸߶ȡ�
	������Ҫ����Ҫ�ڴ�������֮ǰ����ȫ��ģʽ��

	����Ĵ����У��������ٵ��Ŀ�Ⱥ͸߶ȣ������ѱ����ó�����ʾģʽ����Ӧ�Ĵ�С��
	*/
	if (fullscreen)												// Ҫ����ȫ��ģʽ��?
	{
		DEVMODE dmScreenSettings;								// �豸ģʽ
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth = width;					// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight = height;					// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel = bits;					// ÿ������ѡ��ɫ�����
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//����Ĵ����У����Ƿ��������ڴ洢��Ƶ���õĿռ䡣�趨����Ļ�Ŀ��ߣ�ɫ����ȡ�
		//����Ĵ������ǳ�������ȫ��ģʽ��
		//������dmScreenSettings�б��������еĿ��ߣ�ɫ�����ѶϢ��
		//��һ��ʹ��ChangeDisplaySettings�������л�����dmScreenSettings��ƥ��ģʽ��
		// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬����   
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// ���ģʽδ�����óɹ������ǽ��������µĴ��롣
			//�������ƥ��ȫ��ģʽ��������Ϣ���ڣ��ṩ����ѡ��ڴ���ģʽ�����л��˳���
			if (MessageBox(NULL, "ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// ѡ�񴰿�ģʽ(Fullscreen=FALSE) 
			}
			else
			{
				// ����һ���Ի��򣬸����û��������
				MessageBox(NULL, "���򽫱��ر�", "����", MB_OK | MB_ICONSTOP);
				return FALSE;									//  �˳������� FALSE
			}
		}
	}
	//����ȫ��ģʽ����ʧ�ܣ��û����ܾ����ڴ��������У�������Ҫ��������Ļ/����֮ǰ���ٴμ��fullscreen��ֵ��TRUE��FALSE��
	//�������ʹ�ô��ڶ�����ȫ��ģʽ����������չ�������������� WS_EX_WINDOWEDGE����ǿ�����3D�й�
	if (fullscreen)												// �Դ���ȫ��ģʽ��?
	{
		dwExStyle = WS_EX_APPWINDOW;							// ��չ������
		dwStyle = WS_POPUP;										// ������
		ShowCursor(FALSE);										// �������ָ��
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// ��չ������
		dwStyle = WS_OVERLAPPEDWINDOW;							//  ������
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// �������ڴﵽ����Ҫ��Ĵ�С

																//���ڲ���鴰���Ƿ�ɹ�������								// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,						// ��չ������
		("OpenGL"),							// ������
		(title),							// ���ڱ���
		dwStyle |							// ѡ��Ĵ�������
		WS_CLIPSIBLINGS |					// ����Ĵ���������
		WS_CLIPCHILDREN,					// ����Ĵ���������
		0, 0,								// ����λ��(���Ͻ�)
		WindowRect.right - WindowRect.left,	// ��������õĴ��ڿ��
		WindowRect.bottom - WindowRect.top,	// ��������õĴ��ڸ߶�
		NULL,								// �޸�����
		NULL,								// �޲˵�
		hInstance,							// ʵ��
		NULL)))								//����WM_CREATE�����κζ���
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, ("���ܴ���һ�������豸������"), ("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// ������ʽ�������Ĵ�С
		1,											// �汾��
		PFD_DRAW_TO_WINDOW |						// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,							// ����֧��˫����
		PFD_TYPE_RGBA,								// ���� RGBA ��ʽ
		bits,										// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,							// ���Ե�ɫ��λ
		0,											// ��Alpha����
		0,											// ����Shift Bit
		0,											// ���ۼӻ���
		0, 0, 0, 0,									// ���Ծۼ�λ
		16,											// 16λ Z-���� (��Ȼ���)
		0,											// ���ɰ建��
		0,											// �޸�������
		PFD_MAIN_PLANE,								// ����ͼ��
		0,											// Reserved
		0, 0, 0										// ���Բ�����
	};
	//���ǰ�洴������ʱû�д����������ǽ��ų���ȡ��OpenGL�豸���������޷�ȡ��DC������������Ϣ�����˳�(����FALSE)��
	if (!(hDC = GetDC(hWnd)))						// ȡ���豸��������ô?
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, ("���ܴ���һ����ƥ������ظ�ʽ"), "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}
	//�跨ΪOpenGL����ȡ���豸����������ǳ����ҵ���Ӧ���ǰ����ѡ�������ظ�ʽ�����ظ�ʽ�����Windows�����ҵ��Ļ�������������Ϣ�����˳�����(����FALSE)��
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	//Windows �ҵ���Ӧ�����ظ�ʽ�󣬳����������ظ�ʽ������޷����ã�����������Ϣ�����˳�����(����FALSE)��
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))	// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	//�����������ظ�ʽ�󣬳���ȡ����ɫ�������������ȡ����ɫ������Ļ�������������Ϣ�����˳�����(����FALSE)��
	if (!(hRC = wglCreateContext(hDC)))				// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	//�����������δ���ִ���Ļ��������Ѿ��跨ȡ�����豸���������ɫ����������Ҫ�����Ǽ�����ɫ������
	if (!wglMakeCurrent(hDC, hRC))					// ���Լ�����ɫ������
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "���ܼ��ǰ��OpenGL��Ȼ������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	// һ��˳���Ļ���OpenGL�����Ѿ�������ɣ����ſ�����ʾ������������Ϊǰ�˴���(�������ߵ����ȼ�)���������������˴��ڡ�
	// Ȼ�����ReSizeGLScene ����Ļ�Ŀ�Ⱥ͸߶����ø�͸��OpenGL��Ļ��
	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority ����������ȼ�
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window ���ü��̵Ľ������˴��� 
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen ����͸�� GL ��Ļ
													//��ת�� InitGL()������������ù��ա������ȵ��κ���Ҫ���õĶ�����
													//�������� InitGL()�ڲ����ж�������飬������ TRUE (һ������)��FALSE (��ʲô����)��
	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	//��������԰�ȫ���ƶ����������Ѿ��ɹ��ˡ�������WinMain()����TRUE����֪WinMain()û�д����Է�ֹ�����˳���

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(HWND	hWnd,				// Handle For This Window ���ڵľ��
	UINT	uMsg,			// Message For This Window ���ڵ���Ϣ
	WPARAM	wParam,			// Additional Message Information ���ӵ���Ϣ����
	LPARAM	lParam)			// Additional Message Information ���ӵ���Ϣ����
{
	switch (uMsg)									// Check For Windows Messages ���Windows��Ϣ
	{//���uMsg����WM_ACTIVE���鿴�����Ƿ���Ȼ���ڼ���״̬����������ѱ���С����������active��ΪFALSE����������ѱ��������active��ֵΪTRUE�� 
	case WM_ACTIVATE:							// Watch For Window Activate Message ���Ӵ��ڼ�����Ϣ
	{
		if (!HIWORD(wParam))					// Check Minimization State �����С��״̬
		{
			active = TRUE;						// Program Is Active �����ڼ���״̬
		}
		else
		{
			active = FALSE;						// Program Is No Longer Active �����ټ���
		}

		return 0;								// Return To The Message Loop ������Ϣѭ��
	}
	//�����Ϣ��WM_SYSCOMMAND(ϵͳ����)���ٴαȶ�wParam��
	//���wParam �� SC_SCREENSAVE �� SC_MONITORPOWER�Ļ�����������Ļ����Ҫ���У�������ʾ�������ڵ�ģʽ������0������ֹ�������·����� 
	case WM_SYSCOMMAND:							// Intercept System Commands ϵͳ�ж�����
	{
		switch (wParam)							// Check System Calls ���ϵͳ����
		{
		case SC_SCREENSAVE:						// Screensaver Trying To Start? ����Ҫ����?
		case SC_MONITORPOWER:					// Monitor Trying To Enter Powersave? ��ʾ��Ҫ����ڵ�ģʽ?
			return 0;							// Prevent From Happening ��ֹ����
		}
		break;									// Exit
	}
	//��� uMsg��WM_CLOSE�����ڽ����رա�
	//���Ƿ����˳���Ϣ����ѭ�������жϡ�����done����ΪTRUE��WinMain()����ѭ����ֹ������رա�
	case WM_CLOSE:								// Did We Receive A Close Message? �յ�Close��Ϣ?
	{
		PostQuitMessage(0);						// Send A Quit Message �����˳���Ϣ
		return 0;								// Jump Back
	}
	//��������м����£�ͨ����ȡwParam����Ϣ�����ҳ���ֵ��
	//�ҽ���������keys[ ]��Ӧ���������Ա��ֵ��ΪTRUE��
	//�����Ժ�Ϳ��Բ���key[ ]����֪ʲô�������¡�����ͬʱ���¶������ 
	case WM_KEYDOWN:							// Is A Key Being Held Down? �м�����ô?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}
	//ͬ������������м��ͷţ�ͨ����ȡwParam����Ϣ�����ҳ���ֵ��
	//Ȼ�󽫼�������keys[ ]��Ӧ���������Ա��ֵ��ΪFALSE��
	case WM_KEYUP:								// Has A Key Been Released? �м��ſ�ô?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}
	//����������ʱ��uMsg ��������ϢWM_SIZE��
	//��ȡlParam��LOWORD ��HIWORD���Եõ������µĿ�Ⱥ͸߶ȡ�
	//�����Ǵ��ݸ�ReSizeGLScene()��OpenGL����������Ϊ�µĿ�Ⱥ͸߶ȡ� 
	case WM_SIZE:								// Resize The OpenGL Window ����OpenGL���ڴ�С
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	}
	//�����޹ص���Ϣ�����ݸ�DefWindowProc����Windows���д���
	// Pass All Unhandled Messages To DefWindowProc �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE	hInstance,			// ��ǰ����ʵ��
	HINSTANCE	hPrevInstance,		// ǰһ������ʵ�� 
	LPSTR		lpCmdLine,			// �����в���
	int			nCmdShow)			// ������ʾ״̬ 
{
	//������������������
	//msg ��������Ƿ�����Ϣ�ȴ�����
	//done�ĳ�ʼֵ��ΪFALSE������ζ�����ǵĳ�����δ������С�ֻҪ����done����FALSE������������С�һ��done��ֵ�ı�ΪTRUE�������˳��� 
	MSG		msg;									// Windows Message Structure Windowsx��Ϣ�ṹ
	BOOL	done = FALSE;							// Bool Variable To Exit Loop �����˳�ѭ����Bool ����

													// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL, "������ȫ��ģʽ������ô��", "����ȫ��ģʽ", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = FALSE;							// Windowed Mode
	}
	//���Ŵ���OpenGL���ڡ�CreateGLWindow�����Ĳ�������Ϊ���⡢��ȡ��߶ȡ�ɫ����ȣ��Լ�ȫ����־��
	//����ô�򵥣��Һ�������δ���ļ�ࡣ
	//���δ�ܴ����ɹ�����������FALSE�����������˳��� 
	// Create Our OpenGL Window
	if (!CreateGLWindow("NeHe's��ɫʵ��", 640, 480, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting? ����Ϣ�ڵȴ���?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message? �յ��˳���Ϣ?
			{
				done = TRUE;						// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages 
			{										//��������˳���Ϣ�����Ƿ�����Ϣ��Ȼ������Ϣ��ʹ��WndProc() �� Windows�ܹ��������ǡ� 
				TranslateMessage(&msg);				// Translate The Message ������Ϣ
				DispatchMessage(&msg);				// Dispatch The Message ������Ϣ
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			//���û����Ϣ���������ǵ�OpenGL����������ĵ�һ�в鿴�����Ƿ񼤻�������ESC����done��������ΪTRUE�����򽫻��˳��� 
			if (active)								// Program Active? ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done = TRUE;					// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen �����˳���ʱ��ˢ����Ļ
				{
					DrawGLScene();					// Draw The Scene ���Ƴ��� 
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering) �������� (˫����)
				}
			}
			//�����һ�����������¼ӵ�(05-01-00)�������û�����F1����ȫ��ģʽ�ʹ���ģʽ���л���
			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
														// Recreate Our OpenGL Window
				if (!CreateGLWindow("NeHe's ��һ������γ���", 640, 480, 16, fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}
	//���done����������FALSE�������˳�����������OpenGL���ڣ������е��ڴ��ͷţ��˳�����
	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}


/*
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
_In_opt_ HINSTANCE hPrevInstance,
_In_ LPWSTR    lpCmdLine,
_In_ int       nCmdShow)
{
UNREFERENCED_PARAMETER(hPrevInstance);
UNREFERENCED_PARAMETER(lpCmdLine);

// TODO: �ڴ˷��ô��롣

// ��ʼ��ȫ���ַ���
LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
LoadStringW(hInstance, IDC_WIN32, szWindowClass, MAX_LOADSTRING);
MyRegisterClass(hInstance);

// ִ��Ӧ�ó����ʼ��:
if (!InitInstance (hInstance, nCmdShow))
{
return FALSE;
}

HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));

MSG msg;

// ����Ϣѭ��:
while (GetMessage(&msg, nullptr, 0, 0))
{
if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
{
TranslateMessage(&msg);
DispatchMessage(&msg);
}
}

return (int) msg.wParam;
}
*/


//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
/*
ATOM MyRegisterClass(HINSTANCE hInstance)
{
WNDCLASSEXW wcex;

wcex.cbSize = sizeof(WNDCLASSEX);

wcex.style          = CS_HREDRAW | CS_VREDRAW;
wcex.lpfnWndProc    = WndProc;
wcex.cbClsExtra     = 0;
wcex.cbWndExtra     = 0;
wcex.hInstance      = hInstance;
wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));
wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);
wcex.lpszClassName  = szWindowClass;
wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

return RegisterClassExW(&wcex);
}
*/
//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
/*
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

if (!hWnd)
{
return FALSE;
}

ShowWindow(hWnd, nCmdShow);
UpdateWindow(hWnd);

return TRUE;
}
*/
/*
�ⲿ�ֺ���ʵ���˴��ڵĻ�ͼ���ִ���
���봰�ڵľ������þ��ָ��Ĵ��ڽ��в���
*/
/*
void Event(HWND hWnd) {
PAINTSTRUCT ps;
HDC hdc = BeginPaint(hWnd, &ps);
// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
//hdc = BeginPaint(hWnd, &ps);
// TODO: �ڴ���������ͼ����...
SetTextColor(hdc, RGB(0, 0, 0));//��ɫ�Ļ���
TextOut(hdc, 100, 100, "��ɫ�Ļ���", strlen("��ɫ�Ļ���"));//��(100,100)����ļ������Helloworld��
EndPaint(hWnd, &ps);
}
*/
//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
switch (message)
{
case WM_COMMAND:
{
int wmId = LOWORD(wParam);
// �����˵�ѡ��:
switch (wmId)
{
case IDM_ABOUT:
DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
break;
case IDM_EXIT:
DestroyWindow(hWnd);
break;
default:
return DefWindowProc(hWnd, message, wParam, lParam);
}
}
break;
case WM_PAINT:
{
Event(hWnd);

//PAINTSTRUCT ps;
//HDC hdc = BeginPaint(hWnd, &ps);
// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
//hdc = BeginPaint(hWnd, &ps);
// TODO: �ڴ���������ͼ����...
//SetTextColor(hdc, RGB(0, 0, 0));//��ɫ�Ļ���
//TextOut(hdc, 100, 100, L"Helloworld", strlen("Helloworld"));//��(100,100)����ļ������Helloworld��
//EndPaint(hWnd, &ps);


//HGLRC           hRC = NULL;   // ������ɫ��������
//HDC             hDC = NULL;   // OpenGL��Ⱦ��������
//HWND            hWnd = NULL;  // �������ǵĴ��ھ��
//HINSTANCE       hInstance;  // ��������ʵ��
//bool    keys[256];  // ������̰���������
//bool    active = TRUE;    // ���ڵĻ��־��ȱʡΪTRUE
//bool    fullscreen = TRUE;    // ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

}
break;
case WM_DESTROY:
PostQuitMessage(0);
break;
default:
return DefWindowProc(hWnd, message, wParam, lParam);
}
return 0;
}
*/

// �����ڡ������Ϣ�������
/*
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
UNREFERENCED_PARAMETER(lParam);
switch (message)
{
case WM_INITDIALOG:
return (INT_PTR)TRUE;

case WM_COMMAND:
if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
{
EndDialog(hDlg, LOWORD(wParam));
return (INT_PTR)TRUE;
}
break;
}
return (INT_PTR)FALSE;
}
*/
