#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL\freeglut.h>
#include <windows.h>
#include "RgbImage.h"
#include "RgbImage.cpp"

#define BLACK    0.0, 0.0, 0.0, 1.0
#define PI		 3.14159

//------------------------------------------------------------------------------------------------ Para as animacoes
GLfloat roda = 0.0;				// angulo de rotaçao da animaçao de rodar
GLfloat abre_vidro = 0.0;		// angulo de abertura do vidro
GLfloat abre_macaneta = 0.0;	// para abrir a macaneta

//------------------------------------------------------------------------------------------------ Objectos (sistema coordenadas)
GLint wScreen = 800, hScreen = 600;		// janela (pixeis)
GLfloat	xC = 30.0, yC = 10.0, zC = 30.0;	// Mundo  (unidades mundo)

//------------------------------------------------------------------------------------------------ Visualizacao/Observador
GLfloat rVisao = 20, aVisao = 0.25 * PI, incVisao = 0.5;
GLfloat obsP[] = { 0, 0, 25 };
GLfloat obsT[] = { 0, 0, 0 };
float anguloZ = 90;		// angulo de observacao em funcao de Z

//------------------------------------------------------------------------------------------------ Texturas
GLuint texture[7];		// 7 texturas -> mundo, porta, vidro, macaneta, chao, teto, parte direita da malha
RgbImage imag;

//------------------------------------------------------------------------------------------------ Luz Ambiente
GLint Ambiente_On_Off = 1;
GLfloat luzGlobalCorAmb[4] = { 0.1, 0.1, 0.1 }; // tem inicial intensidade 0.1 nos canais RGB

//------------------------------------------------------------------------------------------------ Luz Foco
GLint xf = 0;

//------------------------------------------------------------------------------------------------ Luz pontual no teto (no plano yOz)
GLint Luz_Pontual_On_Off = 0;
GLint luzR = 1;
GLint luzG = 1;
GLint luzB = 1;
GLfloat intensidadeT = 0.1; 
GLfloat localPos[4] = { 0.0, yC, zC , 1.0 };		// posicao esta na da frente da sala
GLfloat localCorAmb[4] = { 0, 0, 0, 0.0 };			// componente ambiente
GLfloat localCorDif[4] = { luzR, luzG, luzB, 1.0 };	// componente difusa
GLfloat localCorEsp[4] = { luzR, luzG, luzB, 1.0 };	// componente especular

//------------------------------------------------------------------------------------------------ Luz de foco
GLuint Luz_Focal_On_Off = 0;

//------------------------------------------------------------------------------------------------ Componestes de iluminacao da porta para reflectir prateado
GLfloat silverAmb[] = { 0.19225 ,0.19225 ,0.19225 };
GLfloat silverDif[] = { 0.50754 ,0.50754 ,0.50754, 1};
GLfloat silverSpec[] = {0.508273, 0.508273 ,0.508273}; 
GLfloat silverCoef = 0.4;							   // coeficiente de intensidade

//------------------------------------------------------------------------------------------------ Componestes de iluminacao da Macaneta para reflectir dourado
GLfloat goldAmb[] = {0.24725 ,0.1995 ,0.0745};
GLfloat goldDif[] = {0.75164 ,0.60648 ,0.22648, 1};
GLfloat goldSpec[] = {0.628281 ,0.555802 ,0.366065};
GLint goldCoef = 0.4 * 128;

//------------------------------------------------------------------------------------------------ Componestes de iluminacao do vidro para reflectir perola
GLfloat customAmb[] = { 0.5 , 0.6 , 0.6 };
GLfloat customDif[] = { 0.5, 0.6, 0.6, 0.5 };
GLfloat customSpec[] = { 0.5, 0.6, 0.6 };
GLint customCoef = 0.088 * 128;

//------------------------------------------------------------------------------------------------ Componentes de iluminacao do Mundo para reflectir branco plastico
GLfloat whitePlasticAmb[] = { 0.8 ,0.8 ,0.8 };
GLfloat whitePlasticDif[] = { 0.55 ,0.55 ,0.55, 1};
GLfloat whitePlasticSpec[] = { 0.870 ,0.870 ,0.870 };
GLint whitePlasticCoef = 0.25 * 128;

//----------------------------------------------------- Vértices
static GLfloat vertices[] = {
	//______________________Coluna da esquerda
	//______________________Esquerda
	 -5, -6,  1, // 0 
	 -5, -6, -1, // 1 
	 -5,  6, -1, // 2 
	 -5,  6,  1, // 3 
	//______________________Direita
	 -4, -6,  1, // 4 
	 -4,  6,  1, // 5 
	 -4,  6, -1, // 6 
	 -4, -6, -1, // 7 
	//______________________Cima
	 -5,  6,  1, // 8 
	 -5,  6, -1, // 9 
	 -4,  6, -1, // 10 
	 -4,  6,  1, // 11 
	 //_____________________Baixo
	 -5, -6,  1, // 12
	 -5, -6, -1, // 13
	 -4, -6, -1, // 14
	 -4, -6,  1, // 15
	 //_____________________Frente
	 -5, -6,  1, // 16
	 -4, -6,  1, // 17
	 -4,  6,  1, // 18
	 -5,  6,  1, // 19
	//_____________________Trás
	 -4, -6, -1, // 20
	 -5, -6, -1, // 21
	 -5,  6, -1, // 22
	 -4,  6, -1, // 23

	//______________________Coluna da direita
	//______________________Esquerda
	 4, -6,  1,	// 24 
	 4,  6,  1,	// 25
	 4,  6, -1,	// 26
	 4, -6, -1,	// 27
	//______________________Direita
	 5, -6,  1,	// 28
	 5, -6, -1,	// 29
	 5,  6, -1,	// 30
	 5,  6,  1,	// 31
	//______________________Cima
	 4,  6,  1,	// 32
	 4,  6, -1,	// 33
	 5,  6, -1,	// 34 
	 5,  6,  1,	// 35 
	 //_____________________Baixo
	 4, -6,  1,	// 36
	 4, -6, -1,	// 37
	 5, -6, -1,	// 38
	 5, -6,  1,	// 39
	 //_____________________Frente
	 4, -6,  1,	// 40
	 5, -6,  1,	// 41
	 5,  6,  1,	// 42
	 4,  6,  1,	// 43
	//_____________________Trás
	 5, -6, -1,	// 44
	 4, -6, -1,	// 45
	 4,  6, -1,	// 46
	 5,  6, -1,	// 47

	//______________________Topo
	//______________________Esquerda
	-4,  5,  1,	// 48 
	-4,  6,  1,	// 49
	-4,  6, -1,	// 50
	-4,  5, -1,	// 51
	//______________________Direita
	 4,  5,  1,	// 52
	 4,  6,  1,	// 53
	 4,  6, -1,	// 54
	 4,  5, -1,	// 55
	//______________________Cima
	-4,  6,  1,	// 56
	-4,  6, -1,	// 57
	 4,  6, -1,	// 58 
	 4,  6,  1,	// 59 
	 //_____________________Baixo
	-4,  5,  1,	// 60
	-4,  5, -1,	// 61
	 4,  5, -1,	// 62
	 4,  5,  1,	// 63
	 //_____________________Frente
	-4,  5,  1,	// 64
	 4,  5,  1,	// 65
	 4,  6,  1,	// 66
	-4,  6,  1,	// 67
	//_____________________Trás
	-4,  5, -1,	// 68
	-4,  6, -1,	// 69
	 4,  6, -1,	// 70
	 4,  5, -1,	// 71

	//______________________parte de baixo
	//______________________Esquerda
	-4,  0,  1,	// 72 
	-4,  0,  1,	// 73
	-4, -6, -1,	// 74
	-4, -6, -1,	// 75
	//______________________Direita
	 4,  0,  1,	// 76
	 4, -6,  1,	// 77
	 4, -6, -1,	// 78
	 4,  0, -1,	// 79
	//______________________Cima
	-4,  0,  1,	// 80
	-4,  0, -1,	// 81
	 4,  0, -1,	// 82 
	 4,  0,  1,	// 83 
	 //_____________________Baixo
	-4, -6,  1,	// 84
	-4, -6, -1,	// 85
	 4, -6, -1,	// 86
	 4, -6,  1,	// 87
	 //_____________________Frente
	 4, -6,  1,	// 88
	-4, -6,  1,	// 89
	-4,  0,  1,	// 90
	 4,  0,  1,	// 91
	//_____________________Trás
	 4, -6, -1,	// 92
	-4, -6, -1,	// 93
	-4,  0, -1,	// 94
	 4,  0, -1,	// 95

	//______________________parte transparente (vidro)
	//______________________Esquerda
	-4,  0,  0.25, // 96 
	-4,  5,  0.25, // 97
	-4,  5, -0.25, // 98
	-4,  0, -0.25, // 99
	//______________________Direita
	 4,  5,  0.25, // 100
	 4,  0,  0.25, // 101
	 4,  0, -0.25, // 102
	 4,  5, -0.25, // 103
	//______________________Cima
	-4,  5,  0.25, // 104
	-4,  5, -0.25, // 105
	 4,  5, -0.25, // 106
	 4,  5,  0.25, // 107
	 //_____________________Baixo
	-4,  0,  0.25, // 108
	-4,  0, -0.25, // 109
	 4,  0, -0.25, // 110
	 4,  0,  0.25, // 111
	 //_____________________Frente
	-4,  5,  0.25, // 112
	 4,  5,  0.25, // 113
	 4,  0,  0.25, // 114
	-4,  0,  0.25, // 115
	//_____________________Trás
	-4,  5, -0.25, // 116
	-4,  0, -0.25, // 117
	 4,  0, -0.25, // 118
	 4,  5, -0.25, // 119

	//______________________macaneta
	//______________________Esquerda
	-4, -2,  1.5, // 120
	-4, -1,  1.5, // 121
	-4, -1,  1,	  // 122
	-4, -2,  1,   // 123
	//______________________Direita
	-2,  -2, 1.5, // 124
	-2,  -2,  1,  // 125
	-2,  -1,  1,  // 126
	-2,  -1, 1.5, // 127
	//______________________Cima
	-4,  -1,  1,  // 128
	-4,  -1, 1.5, // 129
	-2,  -1, 1.5, // 130
	-2,  -1,  1,  // 131
	//_____________________Baixo
	-4,  -2,  1,  // 132
	-2,  -2,  1,  // 133
	-2,  -2, 1.5, // 134
	-4,  -2, 1.5, // 135
	//_____________________Frente
	-2,  -1, 1.5, // 136
	-4,  -1, 1.5, // 137
	-4,  -2, 1.5, // 138
	-2,  -2, 1.5, // 139

	//________________________________________ Mundo
	//_____________________Chao
	-xC, -6, -zC, // 140
	-xC, -6,  zC, // 141
	 xC, -6,  zC, // 142
	 xC, -6, -zC, // 143
	//_____________________Parede da esquerda
	-xC,  -6,  zC,  // 144
	-xC,  -6, -zC,  // 145
	-xC,  yC, -zC,  // 146
	-xC,  yC,  zC,  // 147
	//_____________________Parede da direita
	 xC,  -6, -zC,  // 148
	 xC,  -6,  zC,  // 149
	 xC,  yC,  zC,  // 150
	 xC,  yC, -zC,  // 151
	//_____________________Teto
	-xC, yC,  zC, // 152
	-xC, yC, -zC, // 153
	 xC, yC, -zC, // 154
	 xC, yC,  zC, // 155
	//_____________________Fundo
	-xC,  yC, -zC, // 156
	-xC,  -6, -zC, // 157
	 xC,  -6, -zC, // 158
	 xC,  yC, -zC, // 159
	//_____________________Frente
	-xC,  -6, zC, // 160
	-xC,  yC, zC, // 161
	 xC,  yC, zC, // 162
	 xC,  -6, zC, // 163  

	//_____________________ Parte de cima da parede que envolve a porta
	 -5,  6, -0.1, // 164
	 -5,  yC,-0.1, // 165
	  5,  yC,-0.1, // 166
	  5,  6, -0.1  // 167
};

//------------------------------------------------------ Vetores normais
static GLfloat normais[] = {
	//____________ Coluna da esquerda
	//____________ Esquerda
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Direita
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//____________ Cima
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Baixo
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Frente
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	//____________ Trás
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,

	//____________ Coluna da direita
	//____________ Esquerda
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Direita
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//____________ Cima
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Baixo
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Frente
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	//____________ Trás
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,

	//____________ topo
	//____________ Esquerda
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Direita
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//____________ Cima
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Baixo
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Frente
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	//____________ Trás
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,

	//____________ parte de baixo
	//____________ Esquerda
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Direita
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//____________ Cima
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Baixo
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Frente
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	//____________ Trás
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,

	//____________ parte transparente
	//____________ Esquerda
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Direita
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//____________ Cima
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Baixo
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Frente
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	0.0,  0.0, 1.0,
	//____________ Trás
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,
	0.0,  0.0, -1.0,

	//____________ maçaneta
	//____________ Esquerda
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Direita
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//____________ Cima
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Baixo
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Frente
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,

	//_______________________ Mundo
	//____________ Chao
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//____________ Parede da esquerda
	1.0,  0.0, 0.0,
	1.0,  0.0, 0.0,
	1.0,  0.0, 0.0,
	1.0,  0.0, 0.0,
	//____________ Parede da direita
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	//____________ Teto
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	//____________ Fundo
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	//____________ Frente
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,

	//____________ Parte de cima que envolve a porta
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0
};

//----------------------------------------------------- Texturas
static GLfloat texturas[] = {

	//__________________________coluna esquerda
	// lado esquerdo
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	// lado direito
	0, 1,
	0, 0,
	0.1, 0,
	0.1, 1,
	// lado de cima
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado de baixo
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado da frente
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	// lado de trás
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,

	//_____________________coluna direita
	// lado esquerdo
	0, 1,
	0, 0,
	0.1, 0,
	0.1, 1,
	// lado direito
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	// lado de cima
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado de baixo
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado da frente
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	// lado de trás
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,

	//__________________________topo
	//______ esquerda
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	//______ direita
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	//______ cima
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,
	//______ baixo
	0, 1,
	0.5, 0.9,
	0.5, 1,
	0, 0.9,
	//____ frente
	0, 1,
	0, 0,
	0.1, 0,
	0.1, 1,
	//_____ trás
	0, 0,
	0.1, 0,
	0.1, 1,
	0, 1,

	//__________________parte de baixo
	// lado esquerdo
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado direito
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado de cima
	0, 1,
	0, 0.9,
	0.5, 0.9,
	0.5, 1,
	// lado de baixo
	0, 0.9,
	0.1, 0.9,
	0.1, 1,
	0, 1,
	// lado da frente
	0, 0,
	1, 0,
	1, 0.6,
	0, 0.6,
	// lado de trás
	0, 0,
	1, 0,
	1, 0.6,
	0, 0.6,

	// vidro
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0,	1, 1, 0, 1,

	// macaneta
	0, 0, 1, 0,	1, 1, 0, 1,
	0, 0, 1, 0,	1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,

	//_____________________________ Mundo
	// chao
	0, 0,
	1, 0,
	1, 1,
	0, 1,
	// parede esquerda
	0, 0,
	1, 0,
	1, 1,
	0, 1,
	// parede direita
	0, 0,
	1, 0,
	1, 1,
	0, 1,
	// teto
	0, 1,
	0, 0,
	1, 0,
	1, 1,
	// fundo
	0, 1,
	0, 0,
	1, 0,
	1, 1,
	// frente
	1, 0,
	1, 1,
	0, 1,
	0, 0,

	// parte de cima da parede que envolve a porta
	0.4, 0.6,
	0.4, 0.9,
	0.8, 0.9,
	0.8, 0.6
};

//_____________________________________________________ Coluna esquerda
static GLuint coluna_e_cima[] = { 8, 11, 10,  9 };
static GLuint coluna_e_esquerda[] = { 0, 1, 2, 3 };
static GLuint coluna_e_direita[] = { 4, 7, 6, 5 };
static GLuint coluna_e_baixo[] = { 12, 13, 14, 15 };
static GLuint coluna_e_frente[] = { 16, 17, 18, 19 };
static GLuint coluna_e_tras[] = { 20, 21, 22, 23 };

//_____________________________________________________ Coluna direita
static GLuint coluna_d_cima[] = { 24, 25, 26, 27 };
static GLuint coluna_d_esquerda[] = { 28, 29, 30, 31 };
static GLuint coluna_d_direita[] = { 32, 33, 34, 35 };
static GLuint coluna_d_baixo[] = { 36, 37, 38, 39 };
static GLuint coluna_d_frente[] = { 40, 41, 42, 43 };
static GLuint coluna_d_tras[] = { 44, 45, 46, 47 };

//_____________________________________________________ Topo
static GLuint topo_cima[] = { 48, 49, 50, 51 };
static GLuint topo_esquerda[] = { 52, 53, 54, 55 };
static GLuint topo_direita[] = { 56, 57, 58, 59 };
static GLuint topo_baixo[] = { 60, 61, 62, 63 };
static GLuint topo_frente[] = { 64, 65, 66, 67 };
static GLuint topo_tras[] = { 68, 69, 70, 71 };

//_____________________________________________________ Parte de baixo
static GLuint baixo_cima[] = { 72, 73, 74, 75 };
static GLuint baixo_esquerda[] = { 76, 77, 78, 79 };
static GLuint baixo_direita[] = { 80, 81, 82, 83 };
static GLuint baixo_baixo[] = { 84, 85, 86, 87 };
static GLuint baixo_frente[] = { 88, 89, 90, 91 };
static GLuint baixo_tras[] = { 92, 93, 94, 95 };

//_____________________________________________________ Parte transparente vidro
static GLuint vidro_cima[] = { 96, 97, 98, 99 };
static GLuint vidro_esquerda[] = { 100, 101, 102, 103 };
static GLuint vidro_direita[] = { 104, 105, 106, 107 };
static GLuint vidro_baixo[] = { 108, 109, 110, 111 };
static GLuint vidro_frente[] = { 112, 113, 114, 115 };
static GLuint vidro_tras[] = { 116, 117, 118, 119 };

//_____________________________________________________ Macaneta
static GLuint macaneta_cima[] = { 120, 121, 122, 123 };
static GLuint macaneta_esquerda[] = { 124, 125, 126, 127 };
static GLuint macaneta_direita[] = { 128, 129, 130, 131 };
static GLuint macaneta_baixo[] = { 132, 133, 134, 135 };
static GLuint macaneta_frente[] = { 136, 137, 138, 139 };

//_____________________________________________________ Mundo
static GLuint chao[] = { 140, 141, 142, 143 };
static GLuint parede_esquerda[] = { 144, 145, 146, 147 };
static GLuint parede_direita[] = { 148, 149, 150, 151 };
static GLuint teto[] = { 152, 153, 154, 155 };
static GLuint fundo[] = { 156, 157, 158, 159 };
static GLuint frente[] = { 160, 161, 162, 163 };

//_____________________________________________________ Parte de cima da parede que envolve a porta
static GLuint parte_de_cima[] = { 164, 165, 166, 167 };


void initTexturas()
{
	//----------------------------------------- Mundo
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	imag.LoadBmpFile("mundo.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// GL_MODULATE para multiplicar as texturas com a fonte de iluminacao
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	//-----------------------------------------  Porta
	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	imag.LoadBmpFile("porta.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	//-----------------------------------------  Vidro
	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	imag.LoadBmpFile("vidro.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	//-----------------------------------------  macaneta
	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	imag.LoadBmpFile("macaneta.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	//----------------------------------------- Chao
	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	imag.LoadBmpFile("chao.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	//----------------------------------------- Teto
	glGenTextures(1, &texture[5]);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	imag.LoadBmpFile("teto.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());

	//----------------------------------------- parte direita da parede que envolve a porta
	glGenTextures(1, &texture[6]);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	imag.LoadBmpFile("mundo_invertido.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// GL_MODULATE para multiplicar as texturas com a fonte de iluminacao
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		imag.GetNumCols(),
		imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
		imag.ImageData());
}

void prepara_Luz_Pontual() {

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

	glLightfv(GL_LIGHT0, GL_POSITION, localPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, localCorAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, localCorDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, localCorEsp);
}

void prepara_Luz_Focal() {

	GLfloat pos[] = { xf, 1, -10, 1};	// posicao do foco
	GLfloat direcao[] = { 0, 0, 1};		// o foco aponta na diracao do Z
	GLfloat cor[] = { 1, 1, 1, 1 };		// cor branca

	// atenuacao da luz com a distancia (pode ser linearmente ou quadratica)
	GLfloat Foco_ak = 1.0;				// atenuacoes
	GLfloat Foco_al = 0.01;				//   "
	GLfloat Foco_aq = 0.0;				//   "		
	GLfloat Foco_Expon = 10;				// concentracao da luz do foco, quanto maior o valor mais concentrado no centro

	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, cor);
	glLightfv(GL_LIGHT1, GL_AMBIENT, cor);
	glLightfv(GL_LIGHT1, GL_SPECULAR, cor);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, Foco_ak);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, Foco_al);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, Foco_aq);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 35.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direcao);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, Foco_Expon);
}

void initLuzes() 
{
	//____________________________________________________ Ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);

	//____________________________________________________ Luz frontal
	prepara_Luz_Pontual();

	//____________________________________________________ Luz focal
	prepara_Luz_Focal();
}

void inicializa()
{
	glClearColor(BLACK);		// Limpar o mundo
	glShadeModel(GL_SMOOTH);	// Interpolacao de cores metodo de Gourand
	glEnable(GL_DEPTH_TEST);	// Profundidade
	glEnable(GL_BLEND);			// para poder definir transparencia
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // modelo que define como as cores opacas e transparentes se misturam
	glEnable(GL_NORMALIZE);		// para normalizar as normais e nao haver erros de calculos
	glEnable(GL_LIGHTING);		// ativar ilumincao dos objetos
	glEnable(GL_LIGHT0);		// ativar a luz pontual
	glEnable(GL_LIGHT1);		// ativar a luz focal

	initTexturas();				// Inicializar txturas
	initLuzes();				// Inicializar iluminacao

	//VertexArrays: vertices + normais + texturas

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, 0, normais);
	glEnableClientState(GL_NORMAL_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texturas);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void reflecte_Silver() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, silverAmb);  // GL_FRONT_AND_BACK para reflectir nos dois lados
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, silverDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, silverSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, silverCoef);
}

void reflecte_Gold() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, goldAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, goldDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, goldSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, goldCoef);
}

void reflecte_customMaterial() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, customAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, customDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, customSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, customCoef);
}

void reflecte_Plastico_Branco() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, whitePlasticAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whitePlasticDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whitePlasticSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, whitePlasticCoef);
}

void malha_poligonos() {

	//----------------------------------------------- Definir a cor que a malha reflecte
	reflecte_Plastico_Branco();

	//----------------------------------------------- Textura do mundo
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//----------------------------------------------- Desenha malha poligonos para a parte esquerda da parede que envolve a porta
	glPushMatrix();
	glNormal3f(0, 0, -1);		// definir a normal de todos os poligonos da malha

	float i, j;
	float auxX = 0.0;
	float auxY = 0.0;

	//-------------------------------------------------------- Parte esquerda da parede que envolve a porta
	glTranslatef(-30, -6, 0);	// desenhar no sitio certo
	glBegin(GL_QUADS);
	for (i = 0; i < 16; i += 0.2) {			// 16 linhas
		for (j = 0; j < 25; j += 0.2) {		// 25 colunas

			glTexCoord2d(0.0 + auxX, 0.0 + auxY);
			glVertex3d( 0 + j, 0 + i, -0.1);

			glTexCoord2d(0.0 + auxX, 0.0125 + auxY);
			glVertex3d( 0 + j, 0.2 + i, -0.1);

			glTexCoord2d(0.008 + auxX, 0.0125 + auxY);
			glVertex3d(0.2 + j, 0.2 + i, -0.1);

			glTexCoord2d(0.008 + auxX, 0.0 + auxY);
			glVertex3d(0.2 + j, 0 + i, -0.1);
			auxX += 0.008;
		}
		auxY += 0.0125;
	}
	glEnd();
	
	//-------------------------------------------------------- Parte direita da parede que envolve a porta
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTranslatef(35, 0, 0); 
	glBegin(GL_QUADS);
	for (i = 0; i < 16; i += 1) {			// 16 linhas
		for (j = 0; j < 25; j += 1) {		// 25 colunas

			glTexCoord2d(0.0 + auxX, 0.0 + auxY);
			glVertex3d(0 + j, 0 + i, -0.1);

			glTexCoord2d(0.0 + auxX, 0.0625 + auxY);
			glVertex3d(0 + j, 1 + i, -0.1);

			glTexCoord2d(0.04 + auxX, 0.0625 + auxY);
			glVertex3d(1 + j, 1 + i, -0.1);

			glTexCoord2d(0.04 + auxX, 0.0 + auxY);
			glVertex3d(1 + j, 0 + i, -0.1);
			auxX += 0.04;
		}
		auxY += 0.0625;
	}
	glEnd();
	glPopMatrix();
}

void drawMundo()
{
	//___________________ Definir a cor que o mundo reflecte reflecte
	reflecte_Plastico_Branco();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[4]);

	glPushMatrix();
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, chao);

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, teto);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, parede_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, parede_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, fundo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, frente);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, parte_de_cima);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void drawDoor() {

	//___________________ Definir a cor que a porta reflecte
	reflecte_Silver();

	glEnable(GL_TEXTURE_2D);				  // activar modelo de exturas 2D
	glBindTexture(GL_TEXTURE_2D, texture[1]); // fazer bind das faces à textura
	glPushMatrix();
	glTranslatef(5, 0, 0);
	glRotatef(roda, 0.0, 1.0, 0.0);
	glTranslatef(-5, 0, 0);

	//desenhar todas as componentes da porta
	//_____________________________coluna da esquerda
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_e_cima);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_e_baixo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_e_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_e_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_e_frente);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_e_tras);

	//_____________________________coluna da direita
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_d_cima);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_d_baixo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_d_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_d_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_d_frente);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, coluna_d_tras);

	//_____________________________topo
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, topo_cima);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, topo_baixo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, topo_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, topo_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, topo_frente);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, topo_tras);

	//_____________________________parte de baixo
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, baixo_cima);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, baixo_baixo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, baixo_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, baixo_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, baixo_frente);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, baixo_tras);

	//___________________ Definir a cor que o vidro reflecte reflecte
	glDisable(GL_TEXTURE_2D); // e preciso desativar a textura primeiro

	reflecte_customMaterial();

	glPushMatrix();
	glTranslatef(0, 5, 0);
	glRotatef(abre_vidro, 1.0, 0.0, 0.0);
	glTranslatef(0, -5, 0);
	//_____________________________vidro
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, vidro_cima);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, vidro_baixo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, vidro_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, vidro_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, vidro_frente);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, vidro_tras);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);

	//___________________ Definir a cor que a macaneta reflecte
	reflecte_Gold();

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glPushMatrix();
	glTranslatef(abre_macaneta, 0, 0);
	//_____________________________macaneta
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, macaneta_cima);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, macaneta_baixo);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, macaneta_esquerda);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, macaneta_direita);
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, macaneta_frente);
	glPopMatrix();

	glPopMatrix();
}

void display() {

	glClearColor(BLACK);								// limpa ecra
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// limpa uffer de cor e de profundidade
	
	glEnable(GL_LIGHTING);

	glViewport(0, 0, wScreen, hScreen);

	//__________________________________ activar matriz de projecao e limpa-a e define projecao perspectiva
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	gluPerspective(anguloZ, (float)wScreen / hScreen, 0.1, 9999);
	
	//__________________________________ activar matriz de visualizacao e limpa-a
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//__________________________________ definir posicao do observador
	gluLookAt(obsP[0], obsP[1], obsP[2], obsT[0], obsT[1], obsT[2], 0, 1, 0);

	//------------------------Luzes
	prepara_Luz_Pontual();
	prepara_Luz_Focal();

	//------------------------Objectos
	drawMundo();
	malha_poligonos();
	drawDoor();

	glutSwapBuffers();	// Actualizacao
}

void updateLuz() {
	localCorAmb[0] = luzR * intensidadeT;
	localCorAmb[1] = luzG * intensidadeT;
	localCorAmb[2] = luzB * intensidadeT;
	localCorDif[0] = luzR * intensidadeT;
	localCorDif[1] = luzG * intensidadeT;
	localCorDif[2] = luzB * intensidadeT;
	localCorEsp[0] = luzR * intensidadeT;
	localCorEsp[1] = luzG * intensidadeT;
	localCorEsp[2] = luzB * intensidadeT;;
	glLightfv(GL_LIGHT0, GL_AMBIENT, localCorAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, localCorDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, localCorEsp);
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {

	//_________________________________________ Obs sobe / desce
	case '+':
		obsP[1] += 1;
		if (obsP[1] > yC)
			obsP[1] = yC;
		glutPostRedisplay();
		break;
		
	case '-':
		obsP[1] -= 1;
		if (obsP[1] < -6)
			obsP[1] = -6;
		glutPostRedisplay();
		break;

	//_________________________________________ Abre fecha porta
	case 'w':
		if (roda < 90) roda += 2;
		glutPostRedisplay();
		break;

	case 's':
		if (roda > 0) roda -= 2;
		glutPostRedisplay();
		break;

	//_________________________________________ Abre fecha vidro
	case 'q':
		abre_vidro += 1;
		if (abre_vidro > 90) abre_vidro = 90;
		glutPostRedisplay();
		break;

	case 'a':
		abre_vidro -= 1;
		if (abre_vidro < 0.0) abre_vidro = 0.0;
		glutPostRedisplay();
		break;

	//_________________________________________ Desloca macaneta
	case 'e':
		abre_macaneta += 0.2;
		if (abre_macaneta + 2.0 > 4.8 ) abre_macaneta = 2.8;
		glutPostRedisplay();
		break;

	case 'd':
		abre_macaneta -= 0.2;
		if (abre_macaneta < 0) abre_macaneta = 0;
		glutPostRedisplay();
		break;

	//_________________________________________ On/Off luz ambiente
	case 'o':
		Ambiente_On_Off = !Ambiente_On_Off;
		if (Ambiente_On_Off) {
			luzGlobalCorAmb[0] = 0.1;   // Red
			luzGlobalCorAmb[1] = 0.1;   // Green
			luzGlobalCorAmb[2] = 0.1;	// Blue
		}
		else { 
			luzGlobalCorAmb[0] = 0;		// Red
			luzGlobalCorAmb[1] = 0;		// Green
			luzGlobalCorAmb[2] = 0;		// Blue
		}
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalCorAmb);
		glutPostRedisplay();
		break;

	//_________________________________________ Luz frontal
	case 't':
		Luz_Pontual_On_Off = !Luz_Pontual_On_Off;
		if (Luz_Pontual_On_Off)
			glDisable(GL_LIGHT0);
		else
			glEnable(GL_LIGHT0);
		updateLuz();
		glutPostRedisplay();
		break;

	//__________________________________________ intensidade da luz frontal
	case 'i':
		intensidadeT = intensidadeT + 0.1;
		if (intensidadeT > 1.1) intensidadeT = 0;
		updateLuz();
		glutPostRedisplay();
		break;

	//__________________________________________ alterar as cores que a luz emite
	case 'r':
		luzR = (luzR + 1) % 2;
		updateLuz();
		glutPostRedisplay();
		break;
	case 'g':
		luzG = (luzG + 1) % 2;
		updateLuz();
		glutPostRedisplay();
		break;
	case 'b':
		luzB = (luzB + 1) % 2;
		updateLuz();
		glutPostRedisplay();
		break;

	//_________________________________________ Luz focal
	case 'f':
		Luz_Focal_On_Off = !Luz_Focal_On_Off;
		if (Luz_Focal_On_Off)
			glDisable(GL_LIGHT1);
		else
			glEnable(GL_LIGHT1);
		glutPostRedisplay();
		break;

	case 'n':
		xf += 1;
		if (xf > xC) xf = xC;
		glutPostRedisplay();
		break;

	case 'm':
		xf -= 1;
		if (xf < -xC) xf = -xC;
		glutPostRedisplay();
		break;

	//__________________________________________ alterar o coeficiente especular da porta
	case 'x':
		silverCoef += 0.1;
		if (silverCoef > 1.0) silverCoef = 1.0;
		glutPostRedisplay();
		break;

	case 'z':
		silverCoef -= 0.1;
		if (silverCoef < 0.0) silverCoef = 0.0;
		glutPostRedisplay();
		break;

	//__________________________________________ sair
	case 27:
		exit(0);
		break;
	}
}

void teclasNotAscii(int key, int x, int y) {

	if (key == GLUT_KEY_UP) {
		obsP[0] = obsP[0] + incVisao * cos(aVisao);
		obsP[2] = obsP[2] - incVisao * sin(aVisao);
	}
	else if (key == GLUT_KEY_DOWN) {
		obsP[0] = obsP[0] - incVisao * cos(aVisao);
		obsP[2] = obsP[2] + incVisao * sin(aVisao);
	}

	if (key == GLUT_KEY_LEFT)
		aVisao = aVisao + incVisao / 10;

	if (key == GLUT_KEY_RIGHT)
		aVisao = aVisao - incVisao / 10;

	// limites eixo x
	if (obsP[0] > xC)
		obsP[0] = xC;

	else if (obsP[0] < -xC)
		obsP[0] = -xC;

	// limites eixo z
	if (obsP[2] > zC)
		obsP[2] = zC;

	else if (obsP[2] < -zC)
		obsP[2] = -zC;

	// para onde olha
	obsT[0] = obsP[0] + rVisao * cos(aVisao);
	obsT[2] = obsP[2] - rVisao * sin(aVisao);

	glutPostRedisplay();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(wScreen, hScreen);
	glutInitWindowPosition(600, 100);
	glutCreateWindow("|Projecao -'p'| |Abre/Fecha vidro: 'q/a'| |Abre macaneta: 'e/d'| |Abre/fecha porta -'w/s'| |Observador:'SETAS'|");

	inicializa();
	glutSpecialFunc(teclasNotAscii);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}