#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "glm.h"

#define PADRAO 100
//experimentar valores
#define AumentoAngulo_RodaGigante 1
#define AumentoAngulo_Carrossel 0,05
#define AnguloEntreCarrinhos M_PI/4

#define LARGURA_DO_MUNDO 1920
#define ALTURA_DO_MUNDO 1080


typedef struct coordenadas {
    double x, y, z;
}coordenadas;

coordenadas camera;
coordenadas base_gigante;
coordenadas chao;
coordenadas pedra1Pos;
coordenadas base_barco;

float Angulo_RodaGigante = 0;
float Angulo_Carrossel = 0;

int camera_posicao=1;
int brinquedo=1;

int light_on=1;

int w=0,a=0,s=0,d=0;

// configura alguns parâmetros do modelo de iluminação: FONTE DE LUZ
const GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
// configura alguns parâmetros do modelo de iluminação: MATERIAL
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

GLuint carregaTextura(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(
                           arquivo,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );

    if (idTextura == 0) {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}

typedef struct vetor_r3{
    GLfloat x;
    GLfloat y;
    GLfloat z;

} vetor_r3;

GLMmodel *RodaGigante_base,*RodaGigante_roda,*RodaGigante_carrinho,
*Carrossel_estrutura,*Carrossel_giragira,
*BarcoViking_base,*BarcoViking_barco,*chaoObj,
*pedra1;

void desenhaChao(GLMmodel* objeto, coordenadas coordenada){
    if(!objeto)
        exit(0);
    glmScale(objeto, 200.0);
    glmUnitize(objeto);
    glmFacetNormals(objeto);
    glmVertexNormals(objeto, 90.0, 1);

    glPushMatrix();
        glTranslatef(coordenada.x, coordenada.y, coordenada.z);
        glScalef(30,1,30);
        glmDraw(objeto, GLM_SMOOTH | GLM_TEXTURE | GLM_COLOR);
    glPopMatrix();
}

void desenhaObjeto(GLMmodel* objeto, coordenadas coordenada){
    if(!objeto)
        exit(0);
    glmScale(objeto, 200.0);
    glmUnitize(objeto);
    glmFacetNormals(objeto);
    glmVertexNormals(objeto, 90.0, 1);

    glPushMatrix();
    glTranslatef(coordenada.x, coordenada.y, coordenada.z);
    glScalef(1,1,1);
    glmDraw(objeto, GLM_SMOOTH | GLM_TEXTURE | GLM_COLOR);
    glPopMatrix();

}

void desenhaRodaGigante(GLMmodel* objeto1,GLMmodel* objeto2,GLMmodel* objeto3, char* string1,char* string2,char* string3, coordenadas coordenada){
    if(objeto1 == NULL){
            objeto1 = glmReadOBJ(string1);
            if(!objeto1)
                exit(0);
            glmScale(objeto1, 200.0);
            glmUnitize(objeto1);
            glmFacetNormals(objeto1);
            glmVertexNormals(objeto1, 90.0, 1);
        }
    if(objeto2 == NULL){
            objeto2 = glmReadOBJ(string2);
            if(!objeto2)
                exit(0);
            glmScale(objeto2, 200.0);
            glmUnitize(objeto2);
            glmFacetNormals(objeto2);
            glmVertexNormals(objeto2, 90.0, 1);
        }
    if(objeto3 == NULL){
            objeto3 = glmReadOBJ(string3);
            if(!objeto3)
                exit(0);
            glmScale(objeto3, 200.0);
            glmUnitize(objeto3);
            glmFacetNormals(objeto3);
            glmVertexNormals(objeto3, 90.0, 1);
        }
    glPushMatrix();
    glTranslatef(coordenada.x, coordenada.y, coordenada.z);
    glScalef(1,1,1);
    glmDraw(objeto1, GLM_SMOOTH | GLM_TEXTURE | GLM_COLOR);
        glPushMatrix();
        glTranslatef(0, +coordenada.y-0.1,0);
        glRotatef(Angulo_RodaGigante,1,0,0);
        glmDraw(objeto2, GLM_SMOOTH | GLM_TEXTURE | GLM_COLOR);
        glRotatef(-Angulo_RodaGigante,1,0,0);
            int t;
            for(t=0;t<8;t++)
            {
                glPushMatrix();
                glScalef(0.25,0.25,0.25);
                glTranslatef(0,-1+0.5-3.5*sin(Angulo_RodaGigante*M_PI/180+t*M_PI/4),+3.5*cos(Angulo_RodaGigante*M_PI/180+t*M_PI/4));
                glmDraw(objeto3, GLM_SMOOTH | GLM_COLOR);
                glPopMatrix();
            }
        glPopMatrix();
    glPopMatrix();
}


void redimensiona(int width, int height) {

    glEnable(GL_DEPTH_TEST);
    glViewport (0, 0, width, height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, ((float)width)/height, 0.5, 200);
    glMatrixMode(GL_MODELVIEW);
}

void PosicionarOBJ(){

}

void desenhaCarrossel(){

}

void desenhaBarcoViking(){

}

void desenha(){

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5, 0.75, 1, 1);
    glLoadIdentity();
    if(camera_posicao==1)
        gluLookAt( camera.x, camera.y, camera.z, camera.x-4, 0, camera.z, 0, 1, 0); //geral
    else
        if(brinquedo==1)
            gluLookAt(2,2.5,0,0,1,0,0,1,0);

    glColor3f(0.0,0.0,0.0);
    desenhaChao(chaoObj,chao);
    desenhaRodaGigante(RodaGigante_base,RodaGigante_roda,RodaGigante_carrinho,"Roda_Gigante/base2.obj","Roda_Gigante/roda.obj","Roda_Gigante/carrinho2.obj",base_gigante);
    glPushMatrix();
    glScalef(0.5,0.5,0.5);
    desenhaObjeto(pedra1,pedra1Pos);
    glPopMatrix();
    glPushMatrix();
    desenhaObjeto(BarcoViking_base,base_barco);
    glPopMatrix();
//    desenhaCarrossel();
//    desenhaBarcoViking();

    glutSwapBuffers();
}

void inicializa_posicoes(){

    base_gigante.x=0;
    base_gigante.y=0.9;
    base_gigante.z=0;

    chao.x=-10;
    chao.y=0;
    chao.z=0;

    camera.x=4;
    camera.y=4;
    camera.z=0;

    pedra1Pos.x=3;
    pedra1Pos.y=0.5;
    pedra1Pos.z=3;

    base_barco.x=5;
    base_barco.y=0.9;
    base_barco.z=0;

}

void MovimentarCamera(){

    if(camera_posicao==1)
    {
        if(w==1 && camera.x>-1)
            camera.x-=0.2;
        if(s==1 && camera.x<19.8)
            camera.x+=0.2;
        if(a==1)
            camera.z+=0.2;
        if(d==1)
            camera.z-=0.2;
    }

}

void AcrescentarAngulos(){
    //arruma o angulo da roda gigante
    Angulo_RodaGigante += AumentoAngulo_RodaGigante;
    if(Angulo_RodaGigante>360)
        Angulo_RodaGigante-=360;

    //arruma o angulo do carrossel
    Angulo_Carrossel += AumentoAngulo_Carrossel;
    if(Angulo_Carrossel>360)
        Angulo_Carrossel-=360;
}
void atualizaCena(int periodo) {

    MovimentarCamera();
    AcrescentarAngulos();
    glutPostRedisplay();
    glutTimerFunc(periodo, atualizaCena, periodo);

}

void setup() {
    glClearColor(1, 1, 1, 1);
    // habilita o depth buffer para que a coordenada Z seja usada
    glEnable(GL_DEPTH_TEST);


    Mix_PlayMusic(Mix_LoadMUS("musica.mp3"), 0);

    float corFog[3] = {0.5,0.75,1};
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, corFog);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 16.0);
    glFogf(GL_FOG_END, 30.0);

    // habilita anti-aliasing para desenhar as linhas de forma suave
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable( GL_LINE_SMOOTH );

    // habilita blending para que transparências (e.g., linhas suaves) funcionem
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    inicializa_posicoes();


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    chaoObj = glmReadOBJ("objetos/grama.obj");
    pedra1 = glmReadOBJ("objetos/Rock_10/Rock_10.obj");
    BarcoViking_base = glmReadOBJ("objetos/barcocorrigido/base.obj");

}

void Apertada(unsigned char key, int x, int y){
    switch(key){
        case 27: //ESC
            exit(0);
            break;
        case 'w':
        case 'W':
            w=1;
            break;
        case 'a':
        case 'A':
            a=1;
            break;
        case 's':
        case 'S':
            s=1;
            break;
        case 'd':
        case 'D':
            d=1;
            break;
        case 'l':
        case 'L':
            if(light_on==1)
            {
                glDisable(GL_LIGHTING);
                light_on=0;
            }
            else
            {
                glEnable(GL_LIGHTING);
                light_on=1;
            }
            break;
        case '1':
            camera_posicao=1;
            break;
        case '2':
            camera_posicao=2;
            break;
    }
}

void Solta(unsigned char key, int x, int y){

    switch(key){
        case 'w':
        case 'W':
            w=0;
            break;
        case 'a':
        case 'A':
            a=0;
            break;
        case 's':
        case 'S':
            s=0;
            break;
        case 'd':
        case 'D':
            d=0;
            break;
    }
}

void movimentoMouse(int x, int y) {

}

void click(int botao, int estado, int x, int y){

}

void main(int argc, char** argv){

    glutInit(&argc, argv);
    SDL_Init(SDL_INIT_AUDIO);
    glutInitContextVersion(1,1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1600,900);
    glutInitWindowPosition (0, 0);
    glutCreateWindow("Zoopark");

    glutEnterGameMode();

    setup();

    glutDisplayFunc(desenha);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(Apertada);
    glutKeyboardUpFunc(Solta);
    glutPassiveMotionFunc(movimentoMouse);
    glutMouseFunc(click);

    glutTimerFunc(0,atualizaCena,33);

    glewInit();

    glutMainLoop();
    }
