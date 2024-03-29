#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float coordenadaTexturaPaisagemY = 0.0f;   // Coordenada Y da textura da paisagem
float velocidadeTexturaPaisagem = 0.007f;  // Velocidade de movimento da paisagem

int recordeMaximo = 0;

GLuint idTexturaFundo;
GLuint idTexturaPaisagem;
GLuint idTexturaCarro; 

GLuint texturasCarros[4];
float z;
float x;
float y;

int i, q;
int a = 60;
int pontuacao = 0;
int tempo = 0;
int tela = 0;
bool colisao = false;
char buffer[10];
bool jogoPausado = false;
int posicaoCarroX = 200, posicaoCarroY = 70;
int posicaoCarroInimigoX[4], posicaoCarroInimigoY[4];
int divx = 250, divy = 5, movd;

void atualizarCoordenadasTexturaPaisagem() {
    coordenadaTexturaPaisagemY -= velocidadeTexturaPaisagem;
    if (coordenadaTexturaPaisagemY < 0.0f) {
        // Reset das coordenadas quando a textura atinge a parte inferior
        coordenadaTexturaPaisagemY = 1.0f;
    }
}

void atualizarRecordeMaximo() {
    if (pontuacao > recordeMaximo) {
        recordeMaximo = pontuacao;
    }
}

void desenharTexto(const char ch[], int xpos, int ypos) {
    int numDeCaracteres = strlen(ch);
    glLoadIdentity();
    glRasterPos2f(xpos, ypos);
    for (i = 0; i <= numDeCaracteres - 1; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[i]);
    }
}

void desenharTextoNum(const char ch[], int numTexto, int xpos, int ypos) {
    int len;
    int k;
    k = 0;
    len = numTexto - strlen(ch);
    glLoadIdentity();
    glRasterPos2f(xpos, ypos);
    for (i = 0; i <= numTexto - 1; i++) {
        if (i < len)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0');
        else {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[k]);
            k++;
        }
    }
}

void posicoesCarroInimigo() {
    glClearColor(0.4, 0.2, 0.8, 0);
    for (i = 0; i < 4; i++) {
        if (rand() % 2 == 0) {
            posicaoCarroInimigoX[i] = 200;
        } else {
            posicaoCarroInimigoX[i] = 300;
        }
        posicaoCarroInimigoY[i] = 1000 - i * 180;
    }
}

void desenharEstrada() {
    glBegin(GL_QUADS);
    glColor3f(0, 0, 0);
    glVertex2f(270 - 100, 500);
    glVertex2f(270 - 100, 0);
    glVertex2f(228 + 100, 0);
    glVertex2f(228 + 100, 500);
    glEnd();
}

void desenharSeparador() {
    glLoadIdentity();
    glTranslatef(0, movd, 0);
    for (i = 1; i <= 10; i++) {
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(divx - 5, divy * 15 * i + 18);
        glVertex2f(divx - 5, divy * 15 * i - 18);
        glVertex2f(divx + 5, divy * 15 * i - 18);
        glVertex2f(divx + 5, divy * 15 * i + 18);
        glEnd();
    }
    glLoadIdentity();
}

GLuint carregarTextura(const char* nomeArquivo) {
    int largura, altura, canais;
    unsigned char* dados = stbi_load(nomeArquivo, &largura, &altura, &canais, 0);

    if (!dados) {
        printf("Erro ao carregar a textura.\n");
        exit(1);
    }

    GLuint idTextura;
    glGenTextures(1, &idTextura);
    glBindTexture(GL_TEXTURE_2D, idTextura);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, largura, altura, GL_RGB, GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);

    return idTextura;
}

void carregarTexturas() {
    idTexturaFundo = carregarTextura("C:\\Projetos\\ImagensTEXT\\POSTER.PNG");
    idTexturaPaisagem = carregarTextura("C:\\Projetos\\ImagensTEXT\\FUNDO.jpg");
    idTexturaCarro = carregarTextura("C:\\Projetos\\ImagensTEXT\\modelcar.jpg");
    
    texturasCarros[0] = carregarTextura("C:\\Projetos\\ImagensTEXT\\modelcar.jpg");
        texturasCarros[1] = carregarTextura("C:\\Projetos\\ImagensTEXT\\modelcar.jpg");
            texturasCarros[2] = carregarTextura("C:\\Projetos\\ImagensTEXT\\modelcar.jpg");
                texturasCarros[3] = carregarTextura("C:\\Projetos\\ImagensTEXT\\modelcar.jpg");
    
}

void desenharQUADS() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idTexturaFundo);

    glBegin(GL_QUADS);
    
    glTexCoord2f(0.0, 1.0); glVertex2f(0,0);
    glTexCoord2f(1.0, 1.0); glVertex2f(500,0);
    glTexCoord2f(1.0, 0.0); glVertex2f(500, 500);
    glTexCoord2f(0.0, 0.0); glVertex2f(0, 500);
    glEnd();
   
    glDisable(GL_TEXTURE_2D);
}

void desenharPaisagem() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idTexturaPaisagem);

    glBegin(GL_QUADS);
    
    glTexCoord2f(0.0, coordenadaTexturaPaisagemY + 1.0); glVertex2f(0,0);
    glTexCoord2f(1.0, coordenadaTexturaPaisagemY + 1.0); glVertex2f(500,0);
    glTexCoord2f(1.0, coordenadaTexturaPaisagemY); glVertex2f(500, 500);
    glTexCoord2f(0.0, coordenadaTexturaPaisagemY); glVertex2f(0, 500);
    glEnd();
   
    glDisable(GL_TEXTURE_2D);
}

void desenharCarro() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idTexturaCarro);
    glBegin(GL_QUADS);  
       
    glTexCoord2f(0.0, 0.0); glVertex2f(posicaoCarroX - 18, posicaoCarroY - 60);
    glTexCoord2f(1.0, 0.0); glVertex2f(posicaoCarroX - 18, posicaoCarroY + 60);
    glTexCoord2f(1.0, 1.0); glVertex2f(posicaoCarroX + 18, posicaoCarroY + 60);
    glTexCoord2f(0.0, 1.0); glVertex2f(posicaoCarroX + 18, posicaoCarroY - 60);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenharCarroInimigo() {   
    
    for(i = 0; i < 4; i++) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturasCarros[i]);
        glColor3f(0.1,1,1);
        glBegin(GL_QUADS); 
        glTexCoord2f(0.0, 0.0); glVertex2f(posicaoCarroInimigoX[i] - 18, posicaoCarroInimigoY[i] - 60);
        glTexCoord2f(1.0, 0.0); glVertex2f(posicaoCarroInimigoX[i] - 18, posicaoCarroInimigoY[i] + 60);
        glTexCoord2f(1.0, 1.0); glVertex2f(posicaoCarroInimigoX[i] + 18, posicaoCarroInimigoY[i] + 60);
        glTexCoord2f(0.0, 1.0); glVertex2f(posicaoCarroInimigoX[i] + 18, posicaoCarroInimigoY[i] - 60);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    
        posicaoCarroInimigoY[i] = posicaoCarroInimigoY[i] - 8; 
     
        if(posicaoCarroInimigoY[i] > posicaoCarroY - 25 - 25 && posicaoCarroInimigoY[i] < posicaoCarroY + 40 + 40 && posicaoCarroInimigoX[i] == posicaoCarroX) {
            colisao = true;
        }
     
        if(posicaoCarroInimigoY[i] < -25) { 
            if(rand() % 2 == 0) {
                posicaoCarroInimigoX[i] = 200;
            } else {
                posicaoCarroInimigoX[i] = 300;
            }
            posicaoCarroInimigoY[i] = 800; 
        } 
    }
}

void desenharModoPontuacao() {
    glBegin(GL_QUADS);  
    glColor3f(0.0,0.0 ,0.0);
    glVertex2f(413,400);
    glVertex2f(492,400);
    glVertex2f(492,492);
    glVertex2f(413,492);
    glEnd();
    
    glBegin(GL_QUADS);  
    glColor3f(0.3,0.5 ,0.3);
    glVertex2f(415,403);
    glVertex2f(490,403);
    glVertex2f(490,490);
    glVertex2f(415,490);
    glEnd();
}

void desenharModoFinal() {
    glBegin(GL_QUADS);  
    glColor3f(0.0,0.0 ,0.0);
    glVertex2f(365,100);
    glVertex2f(480,100);
    glVertex2f(480,292);
    glVertex2f(365,292);
    glEnd();
    
    glBegin(GL_QUADS);  
    glColor3f(0.5,0.5 ,0.3);
    glVertex2f(368,103);
    glVertex2f(478,103);
    glVertex2f(478,290);
    glVertex2f(368,290);
    glEnd();
}

void resetarJogo() {
    pontuacao = 0;
    tempo = 0;
    colisao = false;
    posicaoCarroX = 200;
    posicaoCarroY = 70;
    posicoesCarroInimigo();
    atualizarRecordeMaximo();
}

void teclaEspecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        for (i = 0; i < 4; i++) {
            posicaoCarroInimigoY[i] = posicaoCarroInimigoY[i] - 10;
        }
        movd = movd - 20;
        break;
    case GLUT_KEY_DOWN:
        for (i = 0; i < 4; i++) {
            posicaoCarroInimigoY[i] = posicaoCarroInimigoY[i] + 10;
        }
        movd = movd + 20;
        break;
    case GLUT_KEY_LEFT:
        posicaoCarroX = 200;
        break;
    case GLUT_KEY_RIGHT:
        posicaoCarroX = 300;
        break;
    }

    glutPostRedisplay();
}

void teclaNormal(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        if (tela == 0)
            tela = 1;
        break;
    case '2':
        if (tela == 1)
            tela = 2;
        break;
    case 'm':
        if (tela == 2)
            tela = 1;
            resetarJogo();
        break;
    case 'r':
    case 'R':
        resetarJogo();
        a = 60;
        break;
    case 'p':
    case 'P':
        if (tela == 2 && !colisao)
            jogoPausado = !jogoPausado;  // Inverte o estado de pausa
        break;
    case 27:
        exit(0);
    }

    glutPostRedisplay();
}

void inicializar() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500);

    glMatrixMode(GL_MODELVIEW);
}

void display() {
    atualizarCoordenadasTexturaPaisagem();

    if (tela == 0) {
        glClear(GL_COLOR_BUFFER_BIT);
        desenharQUADS();
        
        glColor3f(1, 1, 1);
        desenharTexto("Bem Vindo", 80, 340);
        desenharTexto("Competidor", 80, 320);
        desenharTexto("Pressione 1 para Continuar", 80, 280);
        
        glutSwapBuffers();
    } else if (tela == 1) {
        glClear(GL_COLOR_BUFFER_BIT);
        desenharQUADS();
        glColor3f(1, 1, 1);
        desenharTexto("Use CIMA para Acelerar", 80, 290);
        desenharTexto("Use ESQUERDA para mover a Esquerda", 80, 270);
        desenharTexto("Use DIREITA para mover a Direita", 80, 250);
        desenharTexto("Use BAIXO para Desacelerar", 80, 230);
        desenharTexto("Pressione 2 para iniciar", 80, 150);
        glutSwapBuffers();
    } else { 
        if (jogoPausado) {
            glColor3f(1, 1, 1);
            desenharTexto("Jogo Pausado", 200, 250);
            desenharTexto("Pressione P para continuar", 180, 200);
            glutSwapBuffers();
        } else {
            glClear(GL_COLOR_BUFFER_BIT);
            
            desenharPaisagem();
            desenharEstrada();
            desenharSeparador();
            desenharCarro();
            desenharCarroInimigo();
            movd = movd - 16;
            if (movd < -60) {
                movd = 0;
            }
            desenharModoPontuacao();
            
            if (!colisao){
                pontuacao = pontuacao + 1;
                
                if(pontuacao ==  a){
                    tempo = tempo  + 1;
                    a = a + 60;
                }
            }
            
            glColor3f(1, 1, 1);
            desenharTexto("Use tecla P para Pausar", 40, 480);
            desenharTexto("Score:", 420, 470);
            sprintf(buffer, "%d", pontuacao);
            desenharTextoNum(buffer, 6, 460, 470);
            
            glColor3f(1, 1, 1);
            desenharTexto("Tempo:", 420, 410);
            sprintf(buffer, "%d", tempo);
            desenharTextoNum(buffer, 6, 460, 410);
            
            if (pontuacao > 1000){
                for (i = 0; i < 4; i++) {
                    posicaoCarroInimigoY[i] = posicaoCarroInimigoY[i] - 2;
                }
                movd = movd - 50;
            }
            
            desenharTexto("Recorde:", 420, 440);
            sprintf(buffer, "%d", recordeMaximo);
            desenharTextoNum(buffer, 6, 460 , 440);

            if (colisao) {
                glutSwapBuffers();
                desenharModoFinal();
                
                glColor3f(1, 1, 1);
                desenharTexto("Game Over", 405, 272);
                desenharTexto("Pressione R para Reiniciar", 370, 242);
                desenharTexto("Pressione M para Voltar ao Menu", 370, 225);
                
                desenharTexto("Score Final:", 370, 120);
                sprintf(buffer, "%d", pontuacao);
                desenharTextoNum(buffer, 6, 420, 120);
                
                if(pontuacao >= recordeMaximo){
                    glColor3f(1, 1, 1);
                    desenharTexto("NOVO RECORDE", 390, 180);                    
                }
                
                atualizarRecordeMaximo();
            }

            glutSwapBuffers();

            for (q = 0; q <= 10000000; q++) {
            }
        }
    }
}

int main(int argc, char** argv) {
    if(z==0){
        z+=1;
        system("\"C:\\Projetos\\musica\\Ryder.bat\"");  //jamais mudar o formato
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 800);
    glutCreateWindow("INFINIT RACE");
    posicoesCarroInimigo();
    inicializar();
    carregarTexturas();
    glutDisplayFunc(display);
    glutSpecialFunc(teclaEspecial);
    glutKeyboardFunc(teclaNormal);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}
