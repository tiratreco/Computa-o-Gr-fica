
//Gustavo André de Sousa Paulino
//g++ trabalho.c -o trabalho -lglut -lGLU -lGL
#include <math.h>


#define MENU -1
#define NENHUM 0
#define RETA 1
#define QUADRILATERO 2
#define TRIANGULO 3
#define POLIGONO 4
#define CIRCULO 5
#define TRANSLADO_H 6
#define TRANSLADO_V 7
#define ROTACAO 8
#define CISALHAMENTO 9
#define ESCALA 10
#define ESPELHAMENTO 11
#define SELECIONAR 12
#define DELETAR 13

///////////////// ESTRUTURAS

typedef struct ponto{
    int x;
    int y;
    ponto* prox;
}ponto;

typedef struct figura{
    float r = 0.0, g = 0.0, b = 0.0;
    int id;
    int tipo;
    ponto* pontos;
    int numCantos;
    ponto* cantos;
    int numPontos;
    figura* prox;
}figura;

///////////////// VARIÁVEIS GLOBAIS

figura* figuras = NULL;
int numFiguras = 0;

int estado = NENHUM;
int figura_selecionada = -1;
int a1, a2;
int b1, b2;
int figura_atual = -1;
int conta_pontos = 0;

///////////////// FUNÇÕES DAS LISTAS ENCADEADAS

figura* procurarId(int id){
    figura* atual = figuras;
    while (atual!=NULL) {
        if(atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

void setTipoFigura(int id, int tipo){
    procurarId(id)->tipo = tipo;
}

figura* procurarAnterior(figura* f){
    figura* atual = figuras;
    while (atual!=NULL) {
        if (atual->prox==f){
            return atual;
        }
        atual=atual->prox;
    }
    return NULL;
}

void pushPonto(figura *f, int x, int y){
    ponto* pnt = (ponto*)malloc(sizeof(ponto));
	pnt->x = x;
	pnt->y = y;
	pnt->prox = f->pontos;
    f->pontos = pnt;
    f->numPontos++;
}

void popPonto(figura *f){
	ponto* pnt;
	if(f->pontos != NULL){
		pnt = f->pontos;
		f->pontos = pnt->prox;
		free(pnt);
        f->numPontos--;
	}
}

void popPontos(figura *f){
	while(f->pontos != NULL){
		popPonto(f);
	}
}

void pushCanto(figura *f, int x, int y){
    ponto* pnt = (ponto*)malloc(sizeof(ponto));
	pnt->x = x;
	pnt->y = y;
	pnt->prox = f->cantos;
    f->cantos = pnt;
    f->numCantos++;
}

void popCanto(figura *f){
	ponto* pnt;
	if(f->cantos == NULL) return;
    pnt = f->cantos;
    f->cantos = pnt->prox;
    free(pnt);
    f->numCantos--;
}

void popCantos(figura *f){
	while(f->cantos != NULL){
		popCanto(f);
	}
}

void limpaCantos(figura* f){
    int tam = f->numCantos/2;
    for(int i = 0; i<tam; i++){
        popCanto(f);
    }
}

int pushFigura(){
    figura* fig = (figura*)malloc(sizeof(figura));
    fig->r = 0.0;fig->g = 0.0;fig->b = 0.0;
    fig->tipo = NENHUM;
    fig->id = numFiguras;
	fig->numPontos = 0;
	fig->numCantos = 0;
    fig->pontos = NULL;
    fig->cantos = NULL;
	fig->prox = figuras;
    numFiguras++;
    figuras = fig;
    return fig->id;
}

void popFigura(int id){
    figura* f = procurarId(id);
	figura* anterior = procurarAnterior(f);
    if (f==NULL) return;
    (anterior==NULL)? figuras = f->prox:anterior->prox = f->prox;
    popPontos(f);
    popCantos(f);
    free(f);
	return;
}

///////////////// FUNÇÕES DE DESENHOS

int figuraPerto(int x, int y){
    figura* figuraPerto = NULL;
    figura* figuraAtual = figuras;
    ponto* pontoAtual;
    float menorDistancia = 9999999.0, distanciaAtual;
    while (figuraAtual!=NULL){
        pontoAtual = figuraAtual->pontos;
        while (pontoAtual!=NULL && figuraAtual->tipo!=MENU){
            if(x == pontoAtual->x && y == pontoAtual->y) return figuraAtual->id;
            distanciaAtual = sqrt(pow(pontoAtual->x-x,2)+pow(pontoAtual->y-y,2));
            if (menorDistancia > distanciaAtual){
                menorDistancia=distanciaAtual;
                figuraPerto=figuraAtual;
            }
            pontoAtual=pontoAtual->prox;
        }
        figuraAtual = figuraAtual->prox;
    }
    if (figuraPerto==NULL) return -1;
    return figuraPerto->id;

}

ponto* pontoMedio (int id){
    figura* f = procurarId(id);
    ponto *atual = f->pontos;
    int x=0, y=0;
    while (atual!=NULL){
        x += atual->x;
        y += atual->y;
        atual=atual->prox;
    };
    ponto *p = new ponto;
    p->x=x/f->numPontos;
    p->y=y/f->numPontos;
    return p;
}

void bresenham (figura* f, int x0,int y0,int x1,int y1){
    pushCanto(f, x0, y0);
    pushCanto(f, x1, y1);
    int dx = abs(x1-x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0);
    int sy = y0<y1 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;
 
    while(1){
        pushPonto(f, x0, y0);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void redesenhar (int id){
    figura* f = procurarId(id);
    if(f->tipo==CIRCULO) return;
    popPontos(f);
    ponto* atual = f->cantos;
    while(atual!=NULL){
        bresenham(f, atual->x, atual->y, atual->prox->x, atual->prox->y);
        atual = atual->prox->prox; 
    }
    limpaCantos(f);
}

void reta(int id, int x0,int y0,int x1,int y1){
    figura* f = procurarId(id);
    setTipoFigura(id, RETA);
    bresenham(f, x0, y0, x1, y1);
}

void quadrado(int id, int x0,int y0,int x1,int y1){
    figura* f = procurarId(id);
    setTipoFigura(id, QUADRILATERO);
    bresenham(f, x0, y0, x1, y0);
    bresenham(f, x1, y0, x1, y1);
    bresenham(f, x1, y1, x0, y1);
    bresenham(f, x0, y1, x0, y0);
}

void circulo(int id, int x,int y, int raio){
    figura* f = procurarId(id);
    setTipoFigura(id, CIRCULO);
    int d = 1 - raio;
    int variaE = 4;
    int variaSe = -2*raio + 5;
	int xi = 0, yi = raio;
	int x1, x2, y1, y2;
	
	while(yi>=xi){
        pushPonto(f, (x + xi),(y + yi));
        pushPonto(f, (x + yi),(y + xi));
        pushPonto(f, (x - xi),(y + yi));
        pushPonto(f, (x - yi),(y + xi));
        pushPonto(f, (x + xi),(y - yi));
        pushPonto(f, (x + yi),(y - xi));
        pushPonto(f, (x - xi),(y - yi));
        pushPonto(f, (x - yi),(y - xi));
		if(d < 0){
            d += variaE;
            variaE += 2;
            variaSe += 2;
            
        }else{
            d += variaSe;
            variaE += 2;
            variaSe += 4;
            yi--;
        }
        xi++;
    }
}

void transladar(int id, int direcao, int clique, int numPixel){
    ponto* atual = procurarId(id)->pontos;
    switch (direcao){
    case TRANSLADO_H:
        if(clique==0){
            while (atual!=NULL){
                atual->x-=numPixel;
                atual=atual->prox;
            };
        }
        else{
            while (atual!=NULL){
                atual->x+=numPixel;
                atual=atual->prox;
            };
        }
        break;
    case TRANSLADO_V:
        if(clique==0){
            while (atual!=NULL){
                atual->y+=numPixel;
                atual=atual->prox;
            };
        }
        else{
            while (atual!=NULL){
                atual->y-=numPixel;
                atual=atual->prox;
            };
        }
        break;
    default:
        break;
    }
}

void rotacao(int id, int grau){
    figura* f = procurarId(id);
    ponto* atual;
    (f->tipo==CIRCULO)?atual = f->pontos:atual = f->cantos;
    int xaux, yaux;
    double seno, cosseno, rad = grau * M_PI / 180;
    ponto* pMedioInicial = pontoMedio(id);
    while (atual!=NULL){
        xaux=atual->x;
        yaux=atual->y;
        atual->x=(int)(xaux*cos(rad)-yaux*sin(rad));
        atual->y=(int)(xaux*sin(rad)+yaux*cos(rad));
        atual=atual->prox;
    };
    if(f->tipo!=CIRCULO)redesenhar(id);
    ponto* pMedioFinal = pontoMedio(id);
    xaux = pMedioFinal->x - pMedioInicial->x;
    yaux = pMedioFinal->y - pMedioInicial->y;
    atual = f->pontos;
    while (atual!=NULL){
        atual->x-=xaux;
        atual->y-=yaux;
        atual=atual->prox;
    };
}

void cisalhamento(int id, float fator){
    figura* f = procurarId(id);
    ponto* atual;
    (f->tipo==CIRCULO)?atual = f->pontos:atual = f->cantos;
    ponto* pMedioInicial = pontoMedio(id);
    while (atual!=NULL){
        atual->x=(int)(atual->x+(atual->y*fator));
        atual=atual->prox;
    };
    if(f->tipo!=CIRCULO)redesenhar(id);
    ponto* pMedioFinal = pontoMedio(id);
    int xaux, yaux;
    xaux = pMedioFinal->x - pMedioInicial->x;
    yaux = pMedioFinal->y - pMedioInicial->y;
    atual = f->pontos;
    while (atual!=NULL){
        atual->x-=xaux;
        atual->y-=yaux;
        atual=atual->prox;
    };
}

void espelhamento(int id, bool horizontal){
    figura* f = procurarId(id);
    ponto* atual;
    (f->tipo==CIRCULO)?atual = f->pontos:atual = f->cantos;
    ponto* pMedioInicial = pontoMedio(id);
    if (horizontal){
        while (atual!=NULL){
            atual->x=-atual->x;
            atual=atual->prox;
        };
    }
    else{
        while (atual!=NULL){
            atual->y=-atual->y;
            atual=atual->prox;
        };
    }
    if(f->tipo!=CIRCULO)redesenhar(id);
    ponto* pMedioFinal = pontoMedio(id);
    int xaux, yaux;
    xaux = pMedioFinal->x - pMedioInicial->x;
    yaux = pMedioFinal->y - pMedioInicial->y;
    atual = f->pontos;
    while (atual!=NULL){
        atual->x-=xaux;
        atual->y-=yaux;
        atual=atual->prox;
    };
}

void escala(int id, float fator){
    figura* f = procurarId(id);
    ponto* atual;
    (f->tipo==CIRCULO)?atual = f->pontos:atual = f->cantos;
    ponto* pMedioInicial = pontoMedio(id);
    
    while (atual!=NULL){
        atual->x=(int)(atual->x*fator);
        atual->y=(int)(atual->y*fator);
        atual=atual->prox;
    };
    if(f->tipo!=CIRCULO)redesenhar(id);
    ponto* pMedioFinal = pontoMedio(id);
    int xaux, yaux;
    xaux = pMedioFinal->x - pMedioInicial->x;
    yaux = pMedioFinal->y - pMedioInicial->y;
    atual = f->pontos;
    while (atual!=NULL){
        atual->x-=xaux;
        atual->y-=yaux;
        atual=atual->prox;
    };
}


///////////////// FUNÇÕES DA OPERAÇÃO DO PROGRAMA

void setEstado(int e){
    estado = e;
    conta_pontos = 0;
    figura* f= NULL;
    if (figura_atual!=-1){
        popFigura(figura_atual);
        figura_atual=-1;
    }
    if(e == DELETAR && figura_selecionada!=-1){
        popFigura(figura_selecionada);
        figura_selecionada=-1;
        f = figuras;
        while (f!=NULL){
            if (f->tipo!=MENU){
                figura_selecionada=f->id;
                glutPostRedisplay();
                return;
            }
            f=f->prox;
        }
    }
    glutPostRedisplay();
}

void desenho(int x,int y, int tipo_clique){
    //x e y = ultimo ponto clicado
    //a1 e b1 = penultimo ponto clicado
    //a2 e b2 = primeiro ponto clicado
    //printf("%d %d\n", x, y);
    if (conta_pontos == 0 && tipo_clique == 0){
        a2 = x;
        b2 = y;
    }
    if (tipo_clique==0) conta_pontos++;

    switch (estado)
    {
    case NENHUM:

        break;
    case RETA:
        if(tipo_clique==1) return;
        if(conta_pontos==2){
            figura_atual=pushFigura();
            reta(figura_atual, a1, b1, x, y); 
            figura_selecionada=figura_atual;
            figura_atual=-1;
            conta_pontos=0;
        }
        break;
    
    case QUADRILATERO:
        if(tipo_clique==1) return;
        if(conta_pontos==2){
            figura_atual=pushFigura();
            quadrado(figura_atual, a1, b1, x, y);
            figura_selecionada=figura_atual;
            figura_atual=-1;
            conta_pontos=0;
        }
        break;
    case TRIANGULO:
        if(tipo_clique==1) return;
        if(conta_pontos==2){
            figura_atual = pushFigura();
            reta(figura_atual, a1, b1, x, y);
        }
        if (conta_pontos == 3){
            setTipoFigura(figura_atual, TRIANGULO);
            reta(figura_atual, a1, b1, x, y);
            reta(figura_atual, a2, b2, x, y);
            figura_selecionada=figura_atual;
            figura_atual = -1;
            conta_pontos=0;
        }
        break;
    
    case POLIGONO:
        if (tipo_clique==1 && conta_pontos<3) return;
        if (tipo_clique==1 && conta_pontos>=3){
            setTipoFigura(figura_atual, POLIGONO);
            reta(figura_atual, a2, b2, a1, b1);
            figura_selecionada=figura_atual;
            figura_atual = -1;
            conta_pontos=0;
        }
        if(conta_pontos==2){
            figura_atual = pushFigura();
            reta(figura_atual, a1, b1, x, y);
        }
        if(conta_pontos>=2) reta(figura_atual, a1, b1, x, y);
        break;

    case CIRCULO:
        if(conta_pontos==2){
            figura_atual=pushFigura();
            circulo(figura_atual,a1,b1,sqrt(pow(x-a1,2)+pow(y-b1,2)));
            figura_selecionada=figura_atual;
            figura_atual=-1;
            conta_pontos=0;
        }
        break;
    
    case TRANSLADO_H:
        if(figura_selecionada==-1) return;
        transladar(figura_selecionada, estado, tipo_clique, 5);
        break;
    case TRANSLADO_V:
        if(figura_selecionada==-1) return;
        transladar(figura_selecionada, estado, tipo_clique, 5);
        break;

    case CISALHAMENTO:
        if(figura_selecionada==-1) return;
        (tipo_clique==0)?cisalhamento(figura_selecionada, -0.1):cisalhamento(figura_selecionada, 0.1);
        break;

    case ESCALA:
        if(figura_selecionada==-1) return;
        (tipo_clique==0)?escala(figura_selecionada, 0.95):escala(figura_selecionada, 1.05);
        break;

    case ROTACAO:
        if(figura_selecionada==-1) return;
        (tipo_clique)?rotacao(figura_selecionada, -5):rotacao(figura_selecionada, 5);
        break;

    case ESPELHAMENTO:
        if(figura_selecionada==-1) return;
        (tipo_clique)?espelhamento(figura_selecionada, true):espelhamento(figura_selecionada, false);
        break;

    case SELECIONAR:
        figura_selecionada = figuraPerto(x, y);
        break;

    case DELETAR:
        //executada no setEstado
        break;

    default:
        break;
    }

    a1 = x;
    b1 = y;
    glutPostRedisplay();
}

void limparMenu(){
    figura* f;
    f = figuras;
    while(f!=NULL){
        if (f->tipo==MENU){
            popFigura(f->id);
        }
        f=f->prox;
    }
}

void menu (){
    limparMenu();

    int idAtual = pushFigura();
    int figuraApagar = pushFigura();

    quadrado(idAtual, 0,511,511,481);

    for (int i=0 ; i<512 ; i = i+32){
        reta(idAtual, i, 512, i, 482);
    } 

    //reta
    reta(idAtual, 5, 507, 25, 487);
    //quadrilatero
    quadrado(idAtual, 37, 507, 57, 485);
    //triangulo
    reta(idAtual, 69, 487, 91, 487);
    reta(idAtual, 91, 487, 80, 507);
    reta(idAtual, 80, 507, 69, 487);
    //poligono
    reta(idAtual, 112, 507, 123, 498);
    reta(idAtual, 123, 498, 120, 487);
    reta(idAtual, 120, 487, 102, 487);
    reta(idAtual, 102, 487, 101, 498);
    reta(idAtual, 101, 498, 112, 507);
    //circulo
    circulo(idAtual,144,496,11);

    if(figura_selecionada!=-1){
        //translacao horizontal
        reta(idAtual, 165, 497, 172, 507);
        reta(idAtual, 165, 497, 172, 487);
        reta(idAtual, 165, 497, 187, 497);
        reta(idAtual, 187, 497, 180, 507);
        reta(idAtual, 187, 497, 180, 487);
        //translacao vertical
        reta(idAtual, 208, 507, 197, 501);
        reta(idAtual, 208, 507, 219, 501);
        reta(idAtual, 208, 507, 208, 487);
        reta(idAtual, 208, 487, 197, 493);
        reta(idAtual, 208, 487, 219, 493);
        //rotacao
        circulo(idAtual,240,496,11);
        reta(idAtual, 250, 500, 246, 495);
        reta(idAtual, 250, 500, 254, 495);
        reta(idAtual, 229, 495, 226, 500);
        reta(idAtual, 229, 495, 234, 500);
        //cisalhamento
        reta(idAtual, 261, 487, 272, 487);
        reta(idAtual, 272, 487, 283, 507);
        reta(idAtual, 283, 507, 272, 507);
        reta(idAtual, 272, 507, 261, 487);
        //escala
        quadrado(idAtual, 293, 507, 315, 485);
        quadrado(idAtual, 298, 502, 310, 490);
        //espelhamento
        reta(idAtual, 325, 507, 347, 487);
        reta(idAtual, 347, 487, 347, 507);
        reta(idAtual, 347, 507, 325, 487);
        reta(idAtual, 325, 487, 325, 507);

        //apagar
        reta(figuraApagar, 389, 507, 411, 487);
        reta(figuraApagar, 389, 487, 411, 507);
        procurarId(figuraApagar)->r=1.0;

    }
    //selecionar
    circulo(idAtual,368,497,11);
    circulo(idAtual,368,497,5);

    setTipoFigura(figuraApagar, MENU);
    setTipoFigura(idAtual, MENU);
}