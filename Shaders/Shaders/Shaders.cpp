
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <fstream>
#include <errno.h>
#include <GL\glew.h>
#include <GL\freeGlut.h>
#include "RgbImage.h"
#include "RgbImage.cpp"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")

GLint wScreen = 1920, hScreen = 1080;

//------------------------------------------------------------------------------------------------ Visualizacao/Observador
#define PI 3.14159
GLfloat  rVisao = 33, aVisao = 0.5 * PI, incVisao = 0.05;
GLfloat  obsP[] = { rVisao * cos(aVisao), 20.0, rVisao * sin(aVisao) };
float anguloZ = 90;		// angulo de observacao em funcao de Z

// --------------------------------------------------------- Para as luzes
GLfloat pontual[4] = { 0, 200, 0, 1.0 };
int PLoc;
GLfloat direcional[4] = { 0, 10, 10, 0.0 };
int DLoc;
GLfloat luz = 1;
int luzLoc;

//---------------------------------------------------------- Para a animacao
GLint anim = 1;
GLfloat teta = 0.0;
GLint tetaLoc;
GLint aux = 0;
GLfloat matrix[16];
GLint location;
GLfloat fi = 0.0;
GLint fiLoc;
GLfloat rotacao[16];
int rotLoc;
GLfloat rotacao2[16];
int rot2Loc;

//---------------------------------------------------------- Para a textura
GLuint text[4];
int textLoc;
RgbImage image;

//---------------------------------------------------------- SHADERS variaveis
char* VertexShaderSource;
char* FragmentShaderSource;
GLuint VertexShader, FragmentShader;
GLuint ShaderProgram;
char filenameV[] = "Vshader.txt";
char filenameF[] = "Fshader.txt";

char* readShaderFile(char* FileName) {

	char* DATA = NULL;
	int   flength = 0;
	FILE* filepoint;
	errno_t err;

	err = fopen_s(&filepoint, FileName, "r");
	if (!err) {
		fseek(filepoint, 0, SEEK_END);
		flength = ftell(filepoint);
		rewind(filepoint);

		DATA = (char*)malloc(sizeof(char) * (flength + 1));
		flength = fread(DATA, sizeof(char), flength, filepoint);

		DATA[flength] = '\0';
		fclose(filepoint);
		return DATA;
	}
	else {
		printf(" --------  Error while reading  %s ", FileName);
	}
}

void BuiltShader(void) {

	GLEW_ARB_vertex_shader;
	GLEW_ARB_fragment_shader;

	//......................................................... Criar
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	VertexShaderSource = readShaderFile(filenameV);
	FragmentShaderSource = readShaderFile(filenameF);

	const char* VS = VertexShaderSource;
	const char* FS = FragmentShaderSource;
	glShaderSource(VertexShader, 1, &VS, NULL);
	glShaderSource(FragmentShader, 1, &FS, NULL);
	free(VertexShaderSource);
	free(FragmentShaderSource);

	//......................................................... Compilar
	glCompileShaderARB(VertexShader);
	glCompileShaderARB(FragmentShader);

	//......................................................... Criar e Linkar
	ShaderProgram = glCreateProgramObjectARB();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);

	//......................................................... Usar
	glUseProgramObjectARB(ShaderProgram);
}

void InitShader(void) {

	BuiltShader();

	// passar o parametro da textura ao FShader
	textLoc = glGetUniformLocation(ShaderProgram, "text");

	PLoc = glGetUniformLocation(ShaderProgram, "pontual");
	glUniform4fv(PLoc, 1, pontual);

	DLoc = glGetUniformLocation(ShaderProgram, "direcional");
	glUniform4fv(DLoc, 1, direcional);

	// passar a variavel que controla a tecla da luz ao Fshader
	luzLoc = glGetUniformLocation(ShaderProgram, "luz");
	glUniform1f(luzLoc, luz);

	// parametro da animacao
	tetaLoc = glGetUniformLocation(ShaderProgram, "teta");
	glUniform1f(tetaLoc, teta);
	fiLoc = glGetUniformLocation(ShaderProgram, "fi");
	glUniform1f(fiLoc, fi);

	rotLoc = glGetUniformLocation(ShaderProgram, "rotacao");
	glUniformMatrix4fv(rotLoc, 1, 0, rotacao);

	rot2Loc = glGetUniformLocation(ShaderProgram, "rotacao2");
	glUniformMatrix4fv(rot2Loc, 1, 0, rotacao2);
}

void DeInitShader(void) {
	glDetachShader(ShaderProgram, VertexShader);
	glDetachShader(ShaderProgram, FragmentShader);
	glDeleteShader(ShaderProgram);
}

void initTexturas()
{
	glGenTextures(1, &text[0]);
	glBindTexture(GL_TEXTURE_2D, text[0]);
	image.LoadBmpFile("piramid.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		image.GetNumCols(),
		image.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		image.ImageData());

	glGenTextures(1, &text[1]);
	glBindTexture(GL_TEXTURE_2D, text[1]);
	image.LoadBmpFile("cristal.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		image.GetNumCols(),
		image.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		image.ImageData());

	glGenTextures(1, &text[2]);
	glBindTexture(GL_TEXTURE_2D, text[2]);
	image.LoadBmpFile("base.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		image.GetNumCols(),
		image.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		image.ImageData());

	glGenTextures(1, &text[3]);
	glBindTexture(GL_TEXTURE_2D, text[3]);
	image.LoadBmpFile("space.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		image.GetNumCols(),
		image.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		image.ImageData());
}

void Inicializa(void) {
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	initTexturas();
}

void Desenha(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, wScreen, hScreen);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(anguloZ, (float)wScreen / hScreen, 0.1, 9999);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(obsP[0], obsP[1], obsP[2], 0, 0, 0, 0, 1, 0);

	// --------------------------------------- Textura
	glUniform1i(textLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	// --------------------------------------- tecla da Luz
	glUniform4fv(PLoc, 1, pontual);
	glUniform4fv(DLoc, 1, direcional);
	glUniform1f(luzLoc, luz);
	glEnable(GL_LIGHTING);

	// --------------------------------------- para a animacoesf
	if (anim == 1) {
		if (aux == 0)
			teta -= 0.003;
		else
			teta += 0.003;

		if (teta <= 0)
			aux = 1;
		else if (teta >= 1.5)
			aux = 0;
	}

	// ------------------------------------------------------------------------------- matriz de rotacao em torno de Y para o cristal
	fi += 0.01;
	rotacao[0] = cos(fi); rotacao[1] = 0;  rotacao[2] = -sin(fi); rotacao[3] = 0;
	rotacao[4] = 0;       rotacao[5] = 1;  rotacao[6] = 0;        rotacao[7] = 0;
	rotacao[8] = sin(fi); rotacao[9] = 0;  rotacao[10] = cos(fi); rotacao[11] = 0;
	rotacao[12] = 0;      rotacao[13] = 0; rotacao[14] = 0;       rotacao[15] = 1;

	// ------------------------------------------------------------------------------- matriz de rotacao em torno de Y para o cristal
	rotacao2[0] = cos(fi / 10); rotacao2[1] = 0;  rotacao2[2] = -sin(fi / 10); rotacao2[3] = 0;
	rotacao2[4] = 0;          rotacao2[5] = 1;  rotacao2[6] = 0;           rotacao2[7] = 0;
	rotacao2[8] = sin(fi / 10); rotacao2[9] = 0;  rotacao2[10] = cos(fi / 10); rotacao2[11] = 0;
	rotacao2[12] = 0;         rotacao2[13] = 0; rotacao2[14] = 0;          rotacao2[15] = 1;

	// --------------------------------------- passar variaveis das animacoes ao shader
	glUniform1f(tetaLoc, teta);
	glUniform1f(fiLoc, fi);
	glUniformMatrix4fv(rotLoc, 1, 0, rotacao);
	glUniformMatrix4fv(rot2Loc, 1, 0, rotacao2);

	// ----------------------------------------------------- plano de fundo
	glBindTexture(GL_TEXTURE_2D, text[3]);
	glBegin(GL_POLYGON);
	glNormal3f(-1, 1, 0);
	glTexCoord2f(0, 0); glVertex3f(40, -5, 40);
	glTexCoord2f(1, 0); glVertex3f(40, -5, -40);
	glTexCoord2f(0.5, 1); glVertex3f(0, -40, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 1);
	glTexCoord2f(0, 0); glVertex3f(40, -5, -40);
	glTexCoord2f(1, 0); glVertex3f(-40, -5, -40);
	glTexCoord2f(0.5, 1); glVertex3f(0, -40, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(1, 1, 0);
	glTexCoord2f(0, 0); glVertex3f(-40, -5, -40);
	glTexCoord2f(1, 0); glVertex3f(-40, -5, 40);
	glTexCoord2f(0.5, 1); glVertex3f(0, -40, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0, 1, -1);
	glTexCoord2f(0, 0); glVertex3f(-40, -5, 40);
	glTexCoord2f(1, 0); glVertex3f(40, -5, 40);
	glTexCoord2f(0.5, 1); glVertex3f(0, -40, 0);
	glEnd();

	int a = 0, b = 0;
	for (int i = 0; i <= 24; i++) {

		if (i > 0) {
			a = -0.5;
			b = 0;
		}

		glPushMatrix();
		glRotated(15 * i, 0, 1, 0);
		glTranslated(1 * a, 0, 1 * b);
		glBegin(GL_POLYGON);
		glNormal3f(1, 1, 0);
		glTexCoord2f(0, 0); glVertex3f(35, 20, 5);
		glTexCoord2f(0.2, 0); glVertex3f(35, 20, -5);
		glTexCoord2f(0.2, 1); glVertex3f(35, -5, -5);
		glTexCoord2f(0, 1); glVertex3f(35, -5, 5);
		glEnd();
		glPopMatrix();
	}

	// ----------------------------------------------------- cristal
	glBindTexture(GL_TEXTURE_2D, text[1]);
	for (int i = 0; i < 20; i += 10) {

		// face orientada em funcao de X e Z
		glBegin(GL_POLYGON);
		glNormal3f(4, 12 - (i * 1.4), 4);
		glTexCoord2f(0, 0);   glVertex3f(0, 5, 4);
		glTexCoord2f(1, 0);   glVertex3f(4, 5, 0);
		glTexCoord2f(0.5, 1); glVertex3f(0, 12 - i, 0);
		glEnd();

		// face orientada em funcao de -X e Z
		glBegin(GL_POLYGON);
		glNormal3f(-4, 12 - (i * 1.4), 4);
		glTexCoord2f(0, 0);    glVertex3f(-4, 5, 0);
		glTexCoord2f(1, 0);    glVertex3f(0, 5, 4);
		glTexCoord2f(0.5, 1);  glVertex3f(0, 12 - i, 0);
		glEnd();

		// face orientada em funcao de -X e -Z
		glBegin(GL_POLYGON);
		glNormal3f(-4, 12 - (i * 1.4), -4);
		glTexCoord2f(0, 0);    glVertex3f(0, 5, -4);
		glTexCoord2f(1, 0);    glVertex3f(-4, 5, 0);
		glTexCoord2f(0.5, 1);  glVertex3f(0, 12 - i, 0);
		glEnd();

		// face orientada em funcao de X e -Z
		glBegin(GL_POLYGON);
		glNormal3f(4, 12 - (i * 1.4), -4);
		glTexCoord2f(0, 0);    glVertex3f(4, 5, 0);
		glTexCoord2f(1, 0);    glVertex3f(0, 5, -4);
		glTexCoord2f(0.5, 1);  glVertex3f(0, 12 - i, 0);
		glEnd();
	}

	// ----------------------------------------------------- piramide principal
	glBindTexture(GL_TEXTURE_2D, text[2]);
	glRotated(45, 0, 1, 0);

	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, 0);  glVertex3f(0, 0, 15);
	glTexCoord2f(1, 1);  glVertex3f(15, 0, 0);
	glTexCoord2f(0, 1);  glVertex3f(0, 0, -15);
	glTexCoord2f(0, 0);  glVertex3f(-15, 0, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, text[0]);
	// face orientada em funcao de X
	glBegin(GL_POLYGON);
	glNormal3f(1, 1, 0);
	glTexCoord2f(0, 0);   glVertex3f(0, 0, 15);
	glTexCoord2f(1, 0);   glVertex3f(15, 0, 0);
	glTexCoord2f(0.5, 1); glVertex3f(0.01, 25, 0.01);
	glEnd();

	// face orientada em funcao de Z
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 1);
	glTexCoord2f(0, 0);    glVertex3f(0, 0, 15);
	glTexCoord2f(1, 0);    glVertex3f(-15, 0, 0);
	glTexCoord2f(0.5, 1);  glVertex3f(-0.01, 25, 0.01);
	glEnd();

	// face orientada em funcao de -X
	glBegin(GL_POLYGON);
	glNormal3f(-1, 1, 0);
	glTexCoord2f(0, 0);    glVertex3f(-15, 0, 0);
	glTexCoord2f(1, 0);    glVertex3f(0, 0, -15);
	glTexCoord2f(0.5, 1);  glVertex3f(-0.01, 25, -0.01);
	glEnd();

	// face orientada em funcao de -Z
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, -1);
	glTexCoord2f(0, 0);    glVertex3f(0, 0, -15);
	glTexCoord2f(1, 0);    glVertex3f(15, 0, 0);
	glTexCoord2f(0.5, 1);  glVertex3f(0.01, 25, -0.01);
	glEnd();

	glutSwapBuffers();
}

void Teclado(unsigned char key, int x, int y) {

	switch (key) {
	case 'l':
		if (luz == 0)
			luz = 1;
		else
			luz = 0;
		glutPostRedisplay();
		break;
	case 's':
		if (anim == 1)
			anim = 0;
		else
			anim = 1;
		glutPostRedisplay();
		break;

	case 27:
		exit(0);
		break;
	}
}

void teclasNotAscii(int key, int x, int y) {

	if (key == GLUT_KEY_UP)   obsP[1] = (obsP[1] + 1);
	if (key == GLUT_KEY_DOWN) obsP[1] = (obsP[1] - 1);
	if (key == GLUT_KEY_LEFT)  aVisao = (aVisao + 0.1);
	if (key == GLUT_KEY_RIGHT) aVisao = (aVisao - 0.1);

	if (obsP[1] > 30)   obsP[1] = 30;   // limita altura
	if (obsP[1] < 5)  obsP[1] = 5;

	obsP[0] = rVisao * cos(aVisao);      // actualiza posicao (X,Z)
	obsP[2] = rVisao * sin(aVisao);

	glutPostRedisplay();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);										// Inicia janela
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);								// dimensoes (pixeis)
	glutInitWindowPosition(500, 100);							// localizacao
	glutCreateWindow("Teclas:  L - luz; Setas; S - animacao On/Off");

	Inicializa();
	GLenum err = glewInit();
	InitShader();
	DeInitShader();

	glutSpecialFunc(teclasNotAscii);
	glutDisplayFunc(Desenha);
	glutKeyboardFunc(Teclado);
	glutIdleFunc(Desenha);

	glutMainLoop();
	return 1;
}
