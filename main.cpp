#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL/SDL.h>
#include <time.h>
#define HAUTEUR 16
#define LARGEUR 30
#define NBRE_BOMBES 99
enum{SAFE, UNSAFE};//bombe
enum{FERME, OUVERT, TROUVE};//etat
enum{CASE_0, CASE_1, CASE_2, CASE_3, CASE_4, CASE_5, CASE_6, CASE_7, CASE_8, CASE_9, BOMBE, DRAPEAU, VIDE};//valeur
struct carre
{
    int bombe;
    int etat;
};
typedef struct carre carre;
carre tableau[LARGEUR][HAUTEUR];
SDL_Surface *ecran= NULL, *piece[13];
SDL_Event event;
SDL_Rect pos;
int continuer =1, x, y, nbre_drapeau=0, first_time=1;

void initialiser()
 {
     int i, j;
     for(i=0; i<LARGEUR; i++)
     {
         for(j=0; j<HAUTEUR; j++)
         {
             tableau[i][j].bombe=SAFE;
             tableau[i][j].etat=FERME;
         }
     }
     for(i=0; i<LARGEUR; i++)
     {
         for(j=0; j<HAUTEUR; j++)
         {
             pos.x= 30*i ;
             pos.y= 30*j ;
             SDL_BlitSurface(piece[VIDE], NULL, ecran, &(pos));
         }
     }
     SDL_Flip(ecran);
 }
void charger_images()
 {
     piece[0]= SDL_LoadBMP("images/0.bmp");
     piece[1]= SDL_LoadBMP("images/1.bmp");
     piece[2]= SDL_LoadBMP("images/2.bmp");
     piece[3]= SDL_LoadBMP("images/3.bmp");
     piece[4]= SDL_LoadBMP("images/4.bmp");
     piece[5]= SDL_LoadBMP("images/5.bmp");
     piece[6]= SDL_LoadBMP("images/6.bmp");
     piece[7]= SDL_LoadBMP("images/7.bmp");
     piece[8]= SDL_LoadBMP("images/8.bmp");
     piece[9]= SDL_LoadBMP("images/9.bmp");
     piece[BOMBE]= SDL_LoadBMP("images/bombe.bmp");
     piece[DRAPEAU]= SDL_LoadBMP("images/drapeau.bmp");
     piece[VIDE]= SDL_LoadBMP("images/vide.bmp");
 }
void charger_partie()
{
     int k, j, i=NBRE_BOMBES;
     while(i>0)
     {
         j= rand()%HAUTEUR;
         k= rand()%LARGEUR;
         if(tableau[k][j].bombe==SAFE && (k!=x)&& (j!=y))
         {
            tableau[k][j].bombe=UNSAFE;
            i--;
         }
     }
}
void vider ()
 {
    int i;
    for (i=0; i<13; i++)
        SDL_FreeSurface(piece[i]);
 }
void lost()
 {
     int i, j;
     for(i=0; i<LARGEUR; i++)
     {
         for(j=0; j<HAUTEUR; j++)
         {
             pos.x= 30*i ;
             pos.y= 30*j ;
             if(tableau[i][j].bombe)
                SDL_BlitSurface(piece[10], NULL, ecran, &(pos));
         }
     }
     SDL_Flip(ecran);
 }
int verifier()
{
    int i, j;
    for(i=0; i<LARGEUR; i++)
     {
         for(j=0; j<HAUTEUR; j++)
         {
            if(tableau[i][j].etat==TROUVE && tableau[i][j].bombe== UNSAFE)
                return 0;
         }
     }
     return 1;
}
int alentours(int x, int y)
{
    int i, j, b_a=0 ;
    for(i=-1; i<2; i++)
    {
        for(j=-1; j<2; j++)
        {
            if(tableau[x+i][y+j].bombe==UNSAFE)
                if((x+i>=0)&&(y+j>=0)&&(x+i<LARGEUR)&&(y+j<HAUTEUR))
                    b_a++ ;
        }
    }
    return b_a;
}
void pause()
{
    int conti=1;
    while(conti)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            conti=0;
            break;
        }
    }
}
void afficher_CASE_0(int x, int y)
{
    int i, j, aux;
    for(i=-1; i<2; i++)
    {
        for(j=-1; j<2; j++)
        {
            if((x+i>=0)&&(y+j>=0)&&(x+i<LARGEUR)&&(y+j<HAUTEUR)&&(tableau[x+i][y+j].etat== FERME))
            {
                aux= alentours(x+i, y+j);
                if(aux!=0 || i*j==0)
                {
                    pos.x=(x+i)*30;
                    pos.y=(y+j)*30;
                    SDL_BlitSurface(piece[aux], NULL, ecran, &pos);
                    tableau[x+i][y+j].etat= OUVERT;
                    if(aux==0 && i*j==0)
                    {
                        afficher_CASE_0(x+i, y+j);
                    }
                }
            }
        }
    }
}
void poser_drapeau()
{
                x= event.button.x/30 ;
                y= event.button.y/30 ;
                switch(tableau[x][y].etat)
                {
                case FERME:
                    {
                        tableau[x][y].etat= TROUVE;
                        pos.x=30*x;
                        pos.y=30*y;
                        SDL_BlitSurface(piece[DRAPEAU], NULL, ecran, &pos);
                        SDL_Flip(ecran);
                        nbre_drapeau++;
                        if(nbre_drapeau == NBRE_BOMBES)
                        {
                            if(verifier()==1)
                                continuer =0;
                        }
                        break;
                    }
                case TROUVE:
                    {
                        tableau[x][y].etat= FERME;
                        pos.x=30*x;
                        pos.y=30*y;
                        SDL_BlitSurface(piece[VIDE], NULL, ecran, &pos);
                        SDL_Flip(ecran);
                        nbre_drapeau--;
                        if(nbre_drapeau == NBRE_BOMBES)
                        {
                            if(verifier()==1)
                                continuer =0;
                        }
                        break;
                default:
                    break;
                    }
                }
}

int main ( int argc, char** argv )
{
    int  i, j, bombes_alentours=0;
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_WM_SetCaption("Demineur", NULL);
    ecran= SDL_SetVideoMode(LARGEUR*30, HAUTEUR*30, 32, SDL_HWSURFACE| SDL_DOUBLEBUF);
    charger_images();
    initialiser();
    while(continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            continuer =0;
            break ;
        case SDL_MOUSEBUTTONUP:
            if(event.button.button==SDL_BUTTON_RIGHT)
                poser_drapeau();
            else if(event.button.button==SDL_BUTTON_LEFT)
            {
                x= event.button.x/30 ;
                y= event.button.y/30 ;
                if(tableau[x][y].etat!= TROUVE)
                {
                    if(first_time==1)
                    {
                        charger_partie();
                        first_time=0;
                    }
                    if(tableau[x][y].bombe== UNSAFE)
                    {
                        lost();
                        continuer=0;
                    }
                    else
                    {
                        tableau[x][y].etat=OUVERT;
                        bombes_alentours= alentours(x, y);
                        pos.x= 30*x;
                        pos.y= 30*y;
                        SDL_BlitSurface(piece[bombes_alentours], NULL, ecran, &pos);
                        if(bombes_alentours==0)
                        {
                            afficher_CASE_0(x, y);
                        }
                        SDL_Flip(ecran);
                    }
                }
            }
            break;
        }
    }
    vider();
    SDL_Quit();
    return 0;
}
