#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

/**
 * @author Pierre ALIBERT 22001689
 * @name TP "Rumba des chiffres"
*/


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))



#define NBPIQUE 4
#define NBCUBEPARPIQUE 3
#define TAILLEJEU NBPIQUE*NBCUBEPARPIQUE


/**
 * Structure d'un etat du jeu
*/
typedef struct struct_etat{
    int jeu[TAILLEJEU]; // 0 == vide
    int poids[TAILLEJEU]; // 0 == vide/pas de poids
    int tetes[NBPIQUE]; // -1 == vide
    int g;
} Etat;


Etat but; // <------------------------ penser a initialiser dans le main
int choixHeuristique = 0;
int isCoutUniforme = 1; 


/**
 * Initialise un etat de jeu en utilisant une liste de l'ordre de ses cubes et une de ses poids correspondant
*/
Etat initEtat(int jeu[], int poids[]){
    Etat e;
    e.g = 0;
    for(int i = 0; i < NBPIQUE; i++){
        e.tetes[i] = -1;
    }
    for(int i = 0; i < NBPIQUE; i++){
        for(int j = 0; j < NBCUBEPARPIQUE; j++){
            e.jeu[i*NBCUBEPARPIQUE + j] = jeu[i*NBCUBEPARPIQUE + j];
            e.poids[i*NBCUBEPARPIQUE + j] = poids[i*NBCUBEPARPIQUE + j];

            if(jeu[i*NBCUBEPARPIQUE + j] != 0)
                e.tetes[i]++;

        }
    }
    return e;
}


/**
 * Initialise un etat en utilisant une liste de ses cubes et leur donne un poids de 1
*/
Etat initEtat1(int jeu[]){
    Etat e;
    e.g = 0;
    for(int i = 0; i < NBPIQUE; i++){
        e.tetes[i] = -1;
    }
    for(int i = 0; i < NBPIQUE; i++){
        for(int j = 0; j < NBCUBEPARPIQUE; j++){
            e.jeu[i*NBCUBEPARPIQUE + j] = jeu[i*NBCUBEPARPIQUE + j];
            if(jeu[i*NBCUBEPARPIQUE + j] != 0){
                e.poids[i*NBCUBEPARPIQUE + j] = 1;
            }else
                e.poids[i*NBCUBEPARPIQUE + j] = 0;

            if(jeu[i*NBCUBEPARPIQUE + j] != 0)
                e.tetes[i]++;

        }
    }
    return e;
}


/**
 * Affiche un etat sur le terminal
*/
void afficherEtat(Etat e){
    printf("\n");
    for(int i = NBCUBEPARPIQUE - 1; i >= 0; i--){
        for(int j = 0; j < NBPIQUE; j++){
            if(e.tetes[j] >= i){
                printf("|%d", e.jeu[j*NBCUBEPARPIQUE + i]);
            }else{
                printf("| ");
            }
        }
        printf("|\n");
    }
    printf("\n");
}

/**
 * Affiche une liste d'etats sur le terminal
*/
void afficherlEtat(Etat lEtat[], int lSize){
    printf("Chemin:");
    for(int i = 0; i < lSize; i++){
        afficherEtat(lEtat[i]);
    }
    printf("------------------\n\n");
}

/**
 * Renvoie vrai si la pique de l'etat donnee est vide
*/
bool piqueVide(Etat e, int pique){
    return e.tetes[pique] == -1;
}

/**
 * Renvoie faux si la pique de l'etat donnee est pleine
*/
bool piquePleine(Etat e, int pique){
    return e.tetes[pique] == 2;
}

/**
 * Remplit la liste passee en parametre avec les destinations possibles depuis la pique de l'etat donnee
*/
void trouverDestinations(Etat e, int piqueInitial, int lDest[], int *destSize){
    *destSize = 0;
    if(!piqueVide(e, piqueInitial)){
        for(int i = 0; i < NBPIQUE; i++){
            if(i != piqueInitial && !piquePleine(e, i)){
                lDest[(*destSize)++] = i;
            }
        }
    }
}


/**
 * Deplace le cube le plus haut de la pique de depart vers la pique d'arrivee
*/
void deplacer(Etat *e, int piqueDepart, int piqueArrivee){
    if(!piqueVide(*e, piqueDepart) && !piquePleine(*e, piqueArrivee)){
        
        ++(*e).tetes[piqueArrivee];

        //Deplace le cube
        (*e).jeu[piqueArrivee*NBCUBEPARPIQUE + (*e).tetes[piqueArrivee]] = 
        (*e).jeu[piqueDepart*NBCUBEPARPIQUE + (*e).tetes[piqueDepart]];
        (*e).jeu[piqueDepart*NBCUBEPARPIQUE + (*e).tetes[piqueDepart]] = 0;

        //Deplacer son poids
        (*e).poids[piqueArrivee*NBCUBEPARPIQUE + (*e).tetes[piqueArrivee]] = 
        (*e).poids[piqueDepart*NBCUBEPARPIQUE + (*e).tetes[piqueDepart]];

        //Mettre a jour le cout du deplacement
        if(isCoutUniforme){
            (*e).g++;
        }else{
            (*e).g += (*e).poids[piqueDepart*NBCUBEPARPIQUE + (*e).tetes[piqueDepart]];
        }
        
        

        (*e).poids[piqueDepart*NBCUBEPARPIQUE + (*e).tetes[piqueDepart]] = 0;

        

        (*e).tetes[piqueDepart]--;
    }

}

/**
 * Remplit la liste d'etat passee en parametre avec les fils possibles de l'etat donne
*/
void operationsPossibles(Etat e, Etat etatsFils[], int *filsSize){
    *filsSize = 0;
    for(int i = 0; i < NBPIQUE; i++){
        int lDest[NBPIQUE - 1];
        int destSize = 0;
        trouverDestinations(e, i, lDest, &destSize);
        for(int j = 0; j < destSize; j++){
            Etat eFils = e;
            deplacer(&eFils, i, lDest[j]);
            etatsFils[(*filsSize)++] = eFils;
        }
    }
}

/**
 * Renvoie vrai si les deux etats passes en parametre son egaux
*/
bool egal(Etat e1, Etat e2){
    int i = 0;
    while(i < TAILLEJEU){
        if((e1.jeu[i] != e2.jeu[i]) || (e1.poids[i] != e2.poids[i]))
            return false;
        i++;
    }
    return true;
}


/**
 * Renvoie vrai si l'etat passe en parametre est egal au but
*/
bool estBut(Etat e){
    return egal(e, but);
}

// heuristique 0
/**
 * Renvoie le nombre de cubes de l'etat donne qui ne sont pas en place par rapport au but
*/
int nombreMalMis(Etat e){
    int nbMalMis = 0;
    for(int i = 0; i < TAILLEJEU; i++){
        if(e.jeu[i] != but.jeu[i] && but.jeu[i] != 0)
            nbMalMis++;
    }
    return nbMalMis;
}

// heuristique 1
/**
 * Renvoie la somme des pronfondeurs des cubes les plus mal places sur chaque pique
*/
int profondeurMalMis(Etat e){
    int sommeMalMis = 0;
    for(int i = 0; i < NBPIQUE; i++){
        for(int j = 0; j < NBCUBEPARPIQUE; j++){
            if(e.jeu[i*NBCUBEPARPIQUE + j] != but.jeu[i*NBCUBEPARPIQUE + j] && e.jeu[i*NBCUBEPARPIQUE + j] != 0){
                sommeMalMis += NBCUBEPARPIQUE - j;
                j = NBCUBEPARPIQUE;
            }
        }
    }
    return sommeMalMis;
}


// heuristique 2
/**
 * Idem heuristique 1 + Prend en compte le nombre de cubes sur le cube mal place le plus profond et le nombre de cubes a deplacer pour pouvoir liberer la place
*/
int profondeur2MalMis(Etat e){
    int sommeMalMis = 0;
    for(int i = 0; i < NBPIQUE; i++){
        for(int j = 0; j < NBCUBEPARPIQUE; j++){
            if(e.jeu[i*NBCUBEPARPIQUE + j] != but.jeu[i*NBCUBEPARPIQUE + j] && e.jeu[i*NBCUBEPARPIQUE + j] != 0){
                sommeMalMis += e.tetes[i]+1 - j;
                j = NBCUBEPARPIQUE;
            }
            if(e.jeu[i*NBCUBEPARPIQUE + j] != but.jeu[i*NBCUBEPARPIQUE + j] && but.jeu[i*NBCUBEPARPIQUE + j] != 0 && but.jeu[i*NBCUBEPARPIQUE + j] != 0){
                sommeMalMis += but.tetes[i]+1 - j;
                j = NBCUBEPARPIQUE;
            }
        }
    }
    return sommeMalMis;
}


/**
 * Renvoie f en fonction de g et h
*/
int heuristique(Etat e){
    switch (choixHeuristique)
    {
    case 0:
        return e.g + nombreMalMis(e);
    case 1:
        return e.g + profondeurMalMis(e);
    case 2:
        return e.g + profondeur2MalMis(e);
    
    default:
        return e.g + nombreMalMis(e);
    }
    
}

/**
 * Renvoie vrai si l'etat donne est dans la liste d'etat passee en parametre
*/
bool appartient(Etat e, Etat lEtat[], int lSize){
    int i = 0;
    while(i < lSize){
        if(egal(lEtat[i], e))
            return true;
        i++;
    }
    return false;
}

typedef struct struct_pair{
    bool found;
    int seuil;
} Pair;


/**
 * Implementation C de l'algorithme IDA*
*/
Pair profondeurBornee(Etat chemin[], int *chSize, int seuil, int *noeudsCrees, int *noeudsDev){
    Etat prochain = chemin[(*chSize) - 1];

    if(heuristique(prochain) > seuil){
        Pair p;
        p.found = false;
        p.seuil = heuristique(prochain);
        return p;
    }

    if(estBut(prochain)){
        Pair p;
        p.found = true;
        p.seuil = seuil;
        return p;
    }

    
        
    int min = INT_MAX;

    Etat etatsFils[NBPIQUE*(NBPIQUE - 1)];
    int filsSize = 0;
    operationsPossibles(prochain, etatsFils, &filsSize);
    *noeudsCrees += filsSize;
    for(int i = 0; i < filsSize; i++){
         if(!appartient(etatsFils[i], chemin, *chSize)){
            chemin[(*chSize)++] = etatsFils[i];
            (*noeudsDev)++;
            Pair p = profondeurBornee(chemin, chSize, seuil, noeudsCrees, noeudsDev);
            if(p.found)
                return p;
            
            if(p.seuil < min)
                min = p.seuil;
            (*chSize)--;
         }
    }
    Pair p;
    p.found = false;
    p.seuil = min;
    return p;
}


/**
 * Implementation C de l'algorithme IDA*
*/
bool rechercheIdaEtoile(Etat e, Etat chemin[], int *chSize, int *cout, int *nbIter, int *noeudsCrees, int *noeudsDev){
    int seuil = heuristique(e);
    *cout = 0;
    *nbIter = 0;
    *noeudsCrees = 0;
    *noeudsDev = 0;
    
    
    Pair p;
    do{
        *chSize = 0;
        chemin[(*chSize)++] = e;
        (*nbIter)++;
        p = profondeurBornee(chemin, chSize, seuil, noeudsCrees, noeudsDev);
        printf("Iteration numero: %d\nNoeuds Crees: %d\nNoeuds Developpes: %d\n\n", *nbIter, *noeudsCrees, *noeudsDev);
        if(p.found){
            *cout = seuil;
            return true;
        }
            
        if(p.seuil == INT_MAX){
            p.found = true;
        }
        seuil = p.seuil;
    }while(!p.found);
    return false;
}


int main(void){

    //Tests de fonctions

    /*int lJeu[TAILLEJEU] = {3, 2, 1, 6, 5, 4, 9, 8, 7, 0, 0, 0};
    int lPoids[TAILLEJEU] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
    but = initEtat(lJeu, lPoids);
    printf("But:");
    afficherEtat(but);

    int lJeu2[TAILLEJEU] = {3, 2, 0, 6, 5, 0, 9, 8, 7, 1, 4, 0};
    int lPoids2[TAILLEJEU] = {1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0};
    Etat eTest = initEtat(lJeu2, lPoids2);
    printf("eTest:");
    afficherEtat(eTest);

    Etat eTest2 = initEtat(lJeu2, lPoids2);
    printf("eTest2:");
    afficherEtat(eTest2);

    printf("tetes eTest:\n");
    for(int i = 0; i < NBPIQUE; i++){
        printf("%d, ", eTest.tetes[i]);
    }
    printf("\n\n");

    int lDest[NBPIQUE-1];
    int destSize = 0;
    trouverDestinations(eTest, 0, lDest, &destSize);
    printf("Destinations possibles eTest:\n0 -> ");
    for(int i = 0; i < destSize; i++){
        printf("%d, ", lDest[i]);
    }
    printf("\n\n");

    printf("eTest == eTest2 ? %d\n\n", egal(eTest, eTest2));

    deplacer(&eTest, 0, 1);
    printf("eTest:");
    afficherEtat(eTest);

    printf("tetes eTest:\n");
    for(int i = 0; i < NBPIQUE; i++){
        printf("%d, ", eTest.tetes[i]);
    }
    printf("\n\n");

    int lJeu3[TAILLEJEU] = {3, 0, 0, 6, 5, 2, 9, 8, 7, 1, 4, 0};
    int lPoids3[TAILLEJEU] = {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    eTest2 = initEtat(lJeu3, lPoids3);
    printf("eTest2:");
    afficherEtat(eTest2);

    printf("eTest == eTest2 ? %d\n\n", egal(eTest, eTest2));

    printf("nbMalMis eTest2 = %d\n\n", nombreMalMis(eTest2));

    Etat etatsFils[NBPIQUE*(NBPIQUE - 1)];
    int filsSize = 0;
    operationsPossibles(eTest2, etatsFils, &filsSize);
    printf("Fils de eTest2:");
    for(int i = 0; i < filsSize; i++){
        afficherEtat(etatsFils[i]);
    }*/





    //Tests des exemples du TP

    Etat eTest3;
    Etat solution[1000];
    int solSize;
    bool found;
    char stop;
    int cout;
    int nbIter;
    int noeudsCrees;
    int noeudsDev;
    int nbTest;


    choixHeuristique = 0; // <------------- Modifier si besoin
    isCoutUniforme = 1; // <------------- Modifier si besoin
    nbTest = 6; // <------------- Modifier si besoin

    for(int i = 0; i < nbTest; i++){
        switch (i)
        {
            
            
            
            
            
            
            
            
            case 0: //--- Situation 1 -> but 1 ---//
                
                printf("\n--- Situation 1 -> but 1 ---\n\n");

                // Initialisation de l'etat de depart
                int lJeu4[TAILLEJEU] = {3, 2, 0, 1, 0, 0, 6, 5, 4, 9, 8, 7};
                if(isCoutUniforme){
                    eTest3 = initEtat1(lJeu4);
                }else{
                    int lPds4[TAILLEJEU] = {1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1};
                    eTest3 = initEtat(lJeu4, lPds4);
                }

                // Initialisation de l'etat but
                int lJeu5[TAILLEJEU] = {3, 2, 1, 0, 0, 0, 6, 5, 4, 9, 8, 7};
                if(isCoutUniforme){
                    but = initEtat1(lJeu5);
                }else{
                    int lPds5[TAILLEJEU] = {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1};
                    but = initEtat(lJeu5, lPds5);
                }

                break;

            
            
            
            
            
            
            
            
            
            
            
            
            case 1: //--- Situation 1 -> but 2 ---//
                
                printf("\n--- Situation 1 -> but 2 ---\n\n");

                // Initialisation du nouveau but
                int lJeu6[TAILLEJEU] = {3, 2, 1, 7, 8, 9, 6, 5, 4, 0, 0, 0};
                if(isCoutUniforme){
                    but = initEtat1(lJeu6);
                }else{
                    int lPds6[TAILLEJEU] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
                    but = initEtat(lJeu6, lPds6);
                }

                break;













            case 2: //--- Situation 2 -> but 3 ---//
                
                printf("\n--- Situation 2 -> but 3 ---\n\n");

                // Initialisation du nouveau depart
                int lJeu7[TAILLEJEU] = {3, 2, 1, 6, 5, 4, 9, 8, 7, 0, 0, 0};
                if(isCoutUniforme){
                    eTest3 = initEtat1(lJeu7);
                }else{
                    int lPds7[TAILLEJEU] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
                    eTest3 = initEtat(lJeu7, lPds7);
                }
                

                // Initialisation du nouveau but
                int lJeu8[TAILLEJEU] = {3, 2, 7, 6, 4, 8, 9, 5, 1, 0, 0, 0};
                if(isCoutUniforme){
                    but = initEtat1(lJeu8);
                }else{
                    int lPds8[TAILLEJEU] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
                    but = initEtat(lJeu8, lPds8);
                }

                break;











            case 3: //--- Situation 2 -> 4 ---//
                
                printf("\n--- Situation 2 -> but 4 ---\n\n");

                // Initialisation du nouveau but
                int lJeu9[TAILLEJEU] = {3, 1, 2, 6, 4, 5, 9, 7, 8, 0, 0, 0};
                if(isCoutUniforme){
                    but = initEtat1(lJeu9);
                }else{
                    int lPds9[TAILLEJEU] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
                    but = initEtat(lJeu9, lPds9);
                }

                break;











            case 4: //--- Situation 2 -> 5 ---//
                
                printf("\n--- Situation 2 -> but 5 ---\n\n");

                // Initialisation du nouveau but
                int lJeu10[TAILLEJEU] = {3, 2, 8, 6, 4, 0, 9, 7, 5, 1, 0, 0};
                if(isCoutUniforme){
                    but = initEtat1(lJeu10);
                }else{
                    int lPds10[TAILLEJEU] = {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0};
                    but = initEtat(lJeu10, lPds10);
                }

                break;










            case 5: //--- Situation 2 -> 6 ---//
                
                printf("\n--- Situation 2 -> but 6 ---\n\n");
                
                // Initialisation du nouveau but
                int lJeu11[TAILLEJEU] = {4, 7, 1, 5, 8, 2, 6, 9, 3, 0, 0, 0};
                if(isCoutUniforme){
                    but = initEtat1(lJeu11);
                }else{
                    int lPds11[TAILLEJEU] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
                    but = initEtat(lJeu11, lPds11);
                }

                break;










        }

        // Affiche l'etat de depart et l'etat but
        printf("depart:");
        afficherEtat(eTest3);
        printf("but:");
        afficherEtat(but);

        // Pause
        scanf("%c", &stop);

        // Recherche du chemin optimal
        solSize = 0;
        cout = 0;
        nbIter = 0;
        noeudsCrees = 0;
        noeudsDev = 0;
        found = rechercheIdaEtoile(eTest3, solution, &solSize, &cout, &nbIter, &noeudsCrees, &noeudsDev);

        // Affiche les resultats
        printf("Found ? %d\n", found);
        printf("Cout: %d\nNombre d'iterations: %d\n\n", cout, nbIter);
        afficherlEtat(solution, solSize);
        
        // Pause (Presser "Entree" pour continuer)
        scanf("%c", &stop);
    }


    
    return 0;
}

