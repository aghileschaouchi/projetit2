/*
 *   Ce fichier fait partie d'un projet de programmation donné en Licence 3 
 *   à l'Université de Bordeaux
 *
 *   Copyright (C) 2014, 2015 Adrien Boussicault
 *
 *    This Library is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This Library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "automate.h"
#include "table.h"
#include "ensemble.h"
#include "outils.h"
#include "fifo.h"

#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> 

#include <assert.h>

#include <math.h>
int i = 0; // global
void action_get_max_etat( const intptr_t element, void* data ){
	int * max = (int*) data;
	if( *max < element ) *max = element;
}

int get_max_etat( const Automate* automate ){
    int max = INT_MIN;
    
    pour_tout_element( automate->etats, action_get_max_etat, &max);
    
    return max;
}

void action_get_min_etat( const intptr_t element, void* data ){
	int * min = (int*) data;
	if( *min > element ) *min = element;
}

int get_min_etat( const Automate* automate ){
	int min = INT_MAX;

	pour_tout_element( automate->etats, action_get_min_etat, &min );

	return min;
}


int comparer_cle(const Cle *a, const Cle *b) {
	if( a->origine < b->origine )
		return -1;
	if( a->origine > b->origine )
		return 1;
	if( a->lettre < b->lettre )
		return -1;
	if( a->lettre > b->lettre )
		return 1;
	return 0;
}

void print_cle( const Cle * a){
	printf( "(%d, %c)" , a->origine, (char) (a->lettre) );
}

void supprimer_cle( Cle* cle ){
	xfree( cle );
}

void initialiser_cle( Cle* cle, int origine, char lettre ){
	cle->origine = origine;
	cle->lettre = (int) lettre;
}

Cle * creer_cle( int origine, char lettre ){
	Cle * result = xmalloc( sizeof(Cle) );
	initialiser_cle( result, origine, lettre );
	return result;
}

Cle * copier_cle( const Cle* cle ){
	return creer_cle( cle->origine, cle->lettre );
}

Automate * creer_automate(){
	Automate * automate = xmalloc( sizeof(Automate) );
	automate->etats = creer_ensemble( NULL, NULL, NULL );
	automate->alphabet = creer_ensemble( NULL, NULL, NULL );
	automate->transitions = creer_table(
		( int(*)(const intptr_t, const intptr_t) ) comparer_cle , 
		( intptr_t (*)( const intptr_t ) ) copier_cle,
		( void(*)(intptr_t) ) supprimer_cle
	);
	automate->initiaux = creer_ensemble( NULL, NULL, NULL );
	automate->finaux = creer_ensemble( NULL, NULL, NULL );
	automate->vide = creer_ensemble( NULL, NULL, NULL ); 
	return automate;
}

Automate * translater_automate_entier( const Automate* automate, int translation ){
	Automate * res = creer_automate();

	Ensemble_iterateur it;
	for( 
		it = premier_iterateur_ensemble( get_etats( automate ) );
		! iterateur_ensemble_est_vide( it );
		it = iterateur_suivant_ensemble( it )
	){
		ajouter_etat( res, get_element( it ) + translation );
	}

	for( 
		it = premier_iterateur_ensemble( get_initiaux( automate ) );
		! iterateur_ensemble_est_vide( it );
		it = iterateur_suivant_ensemble( it )
	){
		ajouter_etat_initial( res, get_element( it ) + translation );
	}

	for( 
		it = premier_iterateur_ensemble( get_finaux( automate ) );
		! iterateur_ensemble_est_vide( it );
		it = iterateur_suivant_ensemble( it )
	){
		ajouter_etat_final( res, get_element( it ) + translation );
	}

	// On ajoute les lettres
	for(
		it = premier_iterateur_ensemble( get_alphabet( automate ) );
		! iterateur_ensemble_est_vide( it );
		it = iterateur_suivant_ensemble( it )
	){
		ajouter_lettre( res, (char) get_element( it ) );
	}

	Table_iterateur it1;
	Ensemble_iterateur it2;
	for(
		it1 = premier_iterateur_table( automate->transitions );
		! iterateur_est_vide( it1 );
		it1 = iterateur_suivant_table( it1 )
	){
		Cle * cle = (Cle*) get_cle( it1 );
		Ensemble * fins = (Ensemble*) get_valeur( it1 );
		for(
			it2 = premier_iterateur_ensemble( fins );
			! iterateur_ensemble_est_vide( it2 );
			it2 = iterateur_suivant_ensemble( it2 )
		){
			int fin = get_element( it2 );
			ajouter_transition(
				res, cle->origine + translation, cle->lettre, fin + translation
			);
		}
	};

	return res;
}


void liberer_automate( Automate * automate ){
	assert( automate );
	liberer_ensemble( automate->vide );
	liberer_ensemble( automate->finaux );
	liberer_ensemble( automate->initiaux );
	pour_toute_valeur_table(
		automate->transitions, ( void(*)(intptr_t) ) liberer_ensemble
	);
	liberer_table( automate->transitions );
	liberer_ensemble( automate->alphabet );
	liberer_ensemble( automate->etats );
	xfree(automate);
}

const Ensemble * get_etats( const Automate* automate ){
	return automate->etats;
}

const Ensemble * get_initiaux( const Automate* automate ){
	return automate->initiaux;
}

const Ensemble * get_finaux( const Automate* automate ){
	return automate->finaux;
}

const Ensemble * get_alphabet( const Automate* automate ){
	return automate->alphabet;
}

void ajouter_etat( Automate * automate, int etat ){
	ajouter_element( automate->etats, etat );
}

void ajouter_lettre( Automate * automate, char lettre ){
	ajouter_element( automate->alphabet, lettre );
}

void ajouter_transition(
	Automate * automate, int origine, char lettre, int fin
){
	ajouter_etat( automate, origine );
	ajouter_etat( automate, fin );
	ajouter_lettre( automate, lettre );

	Cle cle;
	initialiser_cle( &cle, origine, lettre );
	Table_iterateur it = trouver_table( automate->transitions, (intptr_t) &cle );
	Ensemble * ens;
	if( iterateur_est_vide( it ) ){
		ens = creer_ensemble( NULL, NULL, NULL );
		add_table( automate->transitions, (intptr_t) &cle, (intptr_t) ens );
	}else{
		ens = (Ensemble*) get_valeur( it );
	}
	ajouter_element( ens, fin );
}

void ajouter_etat_final(
	Automate * automate, int etat_final
){
	ajouter_etat( automate, etat_final );
	ajouter_element( automate->finaux, etat_final );
}

void ajouter_etat_initial(
	Automate * automate, int etat_initial
){
	ajouter_etat( automate, etat_initial );
	ajouter_element( automate->initiaux, etat_initial );
}

const Ensemble * voisins( const Automate* automate, int origine, char lettre ){
	Cle cle;
	initialiser_cle( &cle, origine, lettre );
	Table_iterateur it = trouver_table( automate->transitions, (intptr_t) &cle );
	if( ! iterateur_est_vide( it ) ){
		return (Ensemble*) get_valeur( it );
	}else{
		return automate->vide;
	}
}

Ensemble * delta1(
	const Automate* automate, int origine, char lettre
){
	Ensemble * res = creer_ensemble( NULL, NULL, NULL );
	ajouter_elements( res, voisins( automate, origine, lettre ) );
	return res; 
}

Ensemble * delta(
	const Automate* automate, const Ensemble * etats_courants, char lettre
){
	Ensemble * res = creer_ensemble( NULL, NULL, NULL );

	Ensemble_iterateur it;
	for( 
		it = premier_iterateur_ensemble( etats_courants );
		! iterateur_ensemble_est_vide( it );
		it = iterateur_suivant_ensemble( it )
	){
		const Ensemble * fins = voisins(
			automate, get_element( it ), lettre
		);
		ajouter_elements( res, fins );
	}

	return res;
}

Ensemble * delta_star(
	const Automate* automate, const Ensemble * etats_courants, const char* mot
){
	int len = strlen( mot );
	int i;
	Ensemble * old = copier_ensemble( etats_courants );
	Ensemble * new = old;
	for( i=0; i<len; i++ ){
		new = delta( automate, old, *(mot+i) );
		liberer_ensemble( old );
		old = new;
	}
	return new;
}

void pour_toute_transition(
	const Automate* automate,
	void (* action )( int origine, char lettre, int fin, void* data ),
	void* data
){
	Table_iterateur it1;
	Ensemble_iterateur it2;
	for(
		it1 = premier_iterateur_table( automate->transitions );
		! iterateur_est_vide( it1 );
		it1 = iterateur_suivant_table( it1 )
	){
		Cle * cle = (Cle*) get_cle( it1 );
		Ensemble * fins = (Ensemble*) get_valeur( it1 );
		for(
			it2 = premier_iterateur_ensemble( fins );
			! iterateur_ensemble_est_vide( it2 );
			it2 = iterateur_suivant_ensemble( it2 )
		){
			int fin = get_element( it2 );
			action( cle->origine, cle->lettre, fin, data );
		}
	};
}

Automate* copier_automate( const Automate* automate ){
	Automate * res = creer_automate();
	Ensemble_iterateur it1;
	// On ajoute les états de l'automate
	for(
		it1 = premier_iterateur_ensemble( get_etats( automate ) );
		! iterateur_ensemble_est_vide( it1 );
		it1 = iterateur_suivant_ensemble( it1 )
	){
		ajouter_etat( res, get_element( it1 ) );
	}
	// On ajoute les états initiaux
	for(
		it1 = premier_iterateur_ensemble( get_initiaux( automate ) );
		! iterateur_ensemble_est_vide( it1 );
		it1 = iterateur_suivant_ensemble( it1 )
	){
		ajouter_etat_initial( res, get_element( it1 ) );
	}
	// On ajoute les états finaux
	for(
		it1 = premier_iterateur_ensemble( get_finaux( automate ) );
		! iterateur_ensemble_est_vide( it1 );
		it1 = iterateur_suivant_ensemble( it1 )
	){
		ajouter_etat_final( res, get_element( it1 ) );
	}
	// On ajoute les lettres
	for(
		it1 = premier_iterateur_ensemble( get_alphabet( automate ) );
		! iterateur_ensemble_est_vide( it1 );
		it1 = iterateur_suivant_ensemble( it1 )
	){
		ajouter_lettre( res, (char) get_element( it1 ) );
	}
	// On ajoute les transitions
	Table_iterateur it2;
	for(
		it2 = premier_iterateur_table( automate->transitions );
		! iterateur_est_vide( it2 );
		it2 = iterateur_suivant_table( it2 )
	){
		Cle * cle = (Cle*) get_cle( it2 );
		Ensemble * fins = (Ensemble*) get_valeur( it2 );
		for(
			it1 = premier_iterateur_ensemble( fins );
			! iterateur_ensemble_est_vide( it1 );
			it1 = iterateur_suivant_ensemble( it1 )
		){
			int fin = get_element( it1 );
			ajouter_transition( res, cle->origine, cle->lettre, fin );
		}
	}
	return res;
}

Automate * translater_automate(
	const Automate * automate, const Automate * automate_a_eviter
){
	if(
		taille_ensemble( get_etats(automate) ) == 0 ||
		taille_ensemble( get_etats(automate_a_eviter) ) == 0
	){
		return copier_automate( automate );
	}
	
	int translation = 
		get_max_etat( automate_a_eviter ) - get_min_etat( automate ) + 1; 

	return translater_automate_entier( automate, translation );
	
}

int est_une_transition_de_l_automate(
	const Automate* automate,
	int origine, char lettre, int fin
){
	return est_dans_l_ensemble( voisins( automate, origine, lettre ), fin );
}

int est_un_etat_de_l_automate( const Automate* automate, int etat ){
	return est_dans_l_ensemble( get_etats( automate ), etat );
}

int est_un_etat_initial_de_l_automate( const Automate* automate, int etat ){
	return est_dans_l_ensemble( get_initiaux( automate ), etat );
}

int est_un_etat_final_de_l_automate( const Automate* automate, int etat ){
	return est_dans_l_ensemble( get_finaux( automate ), etat );
}

int est_une_lettre_de_l_automate( const Automate* automate, char lettre ){
	return est_dans_l_ensemble( get_alphabet( automate ), lettre );
}

void print_ensemble_2( const intptr_t ens ){
	print_ensemble( (Ensemble*) ens, NULL );
}

void print_lettre( intptr_t c ){
	printf("%c", (char) c );
}

void print_automate( const Automate * automate ){
	printf("- Etats : ");
	print_ensemble( get_etats( automate ), NULL );
	printf("\n- Initiaux : ");
	print_ensemble( get_initiaux( automate ), NULL );
	printf("\n- Finaux : ");
	print_ensemble( get_finaux( automate ), NULL );
	printf("\n- Alphabet : ");
	print_ensemble( get_alphabet( automate ), print_lettre );
	printf("\n- Transitions : ");
	print_table( 
		automate->transitions,
		( void (*)( const intptr_t ) ) print_cle, 
		( void (*)( const intptr_t ) ) print_ensemble_2,
		""
	);
	printf("\n");
}

int le_mot_est_reconnu( const Automate* automate, const char* mot ){
	Ensemble * arrivee = delta_star( automate, get_initiaux(automate) , mot ); 
	
	int result = 0;

	Ensemble_iterateur it;
	for(
		it = premier_iterateur_ensemble( arrivee );
		! iterateur_ensemble_est_vide( it );
		it = iterateur_suivant_ensemble( it )
	){
		if( est_un_etat_final_de_l_automate( automate, get_element(it) ) ){
			result = 1;
			break;
		}
	}
	liberer_ensemble( arrivee );
	return result;
}

Automate * mot_to_automate( const char * mot ){
	Automate * automate = creer_automate();
	int i = 0;
	int size = strlen( mot );
	for( i=0; i < size; i++ ){
		ajouter_transition( automate, i, mot[i], i+1 );
	}
	ajouter_etat_initial( automate, 0 );
	ajouter_etat_final( automate, size );
	return automate;
}

//cette structure comprend un automate source et un automate de destination pour creer l'union de deux automates, 
// on varie l'automate source selon nos besoins

typedef struct data_union_t {
    const Automate *automate_source;
    Automate *automate_union;
} data_union_t;

//fonction action qui va créer des etats dans l'automate d'union

void action_union_automate(const intptr_t etat, void* data) {
    data_union_t *data_union = (data_union_t*) data;
    //si l'etat est initial dans l'automate source
    if (est_dans_l_ensemble(data_union->automate_source->initiaux, etat))
        //l'etat est initial dans l'automate union
        ajouter_etat_initial(data_union->automate_union, etat);
        //si l'etat est final dans l'automate source
    else if (est_dans_l_ensemble(data_union->automate_source->finaux, etat))
        //l'etat est final dans l'automate union
        ajouter_etat_final(data_union->automate_union, etat);
    else
        ajouter_etat(data_union->automate_union, etat);
}
/*
 * fonction action qui va créer les transitions dans l'automate d'union
 */
void action_union_automate_transitions(int origine, char lettre, int fin, void* data) {
    data_union_t *data_union = (data_union_t*) data;
    
    //toutes transitions dans l'automate_1 ou l'automate_2 seront copiées dans l'automate union
    ajouter_transition(data_union->automate_union, origine, lettre, fin);
}

Automate * creer_union_des_automates(
        const Automate * automate_1, const Automate * automate_2
        ) {
    //on crée un nouvel automate
    Automate *automate_union = creer_automate();
    
    //on declare une structure qui comporte un automate source et un automate d'union
    data_union_t data_union;
    data_union.automate_source = automate_1; //au début l'automate source est automate_1
    data_union.automate_union = automate_union;
    
    //pour tout etat de l'automate_1 on appelle la fonction action_union_automate
    pour_tout_element(automate_1->etats, action_union_automate, &data_union);
    
    data_union.automate_source = automate_2;
    
    //pour tout etat de l'automate_2 on appelle la fonction action_union_automate
    pour_tout_element(automate_2->etats, action_union_automate, &data_union);

    data_union.automate_source = automate_1;
    
    //pour toute transition de l'automate_1 on appelle la fonction action_union_automate_transitions
    pour_toute_transition(automate_1, action_union_automate_transitions, &data_union);
    
    data_union.automate_source = automate_2;
    
    //pour toute transition de l'automate_2 on appelle la fonction action_union_automate_transitions
    pour_toute_transition(automate_2, action_union_automate_transitions, &data_union);

    return automate_union;
}

/*============================ETATS ACCESSIBLES============================*/

struct data_etats_t {
    const Automate* automate;
    Ensemble* etats;
    int etat;
};

//custom headers
void action_etats_accessibles_directs(int origine, char lettre, int fin, void* data);
void etats_accessibles_recursive(const Automate * automate, int etat, Ensemble* etats_accessibles);

void action_etats_accessibles_directs(int origine, char lettre, int fin, void* data) {
    struct data_etats_t *dt = (struct data_etats_t*) data;

    //si l'état est l'origine de la transition et l'état destination n'est pas encore visité
    if (origine == dt->etat && !est_dans_l_ensemble(dt->etats, fin)) {
        ajouter_element(dt->etats, fin);
        
        //parcourir les états accessibles depuis "fin"
        etats_accessibles_recursive(dt->automate, fin, dt->etats);
    }
}

void etats_accessibles_recursive(const Automate * automate, int etat, Ensemble* etats_accessibles) {
    //init la structure data
    struct data_etats_t data;
    data.automate = automate;
    data.etats = etats_accessibles;
    data.etat = etat;

    //parcourir les transitions de l'automate
    pour_toute_transition(automate, action_etats_accessibles_directs, &data);
}

Ensemble* etats_accessibles(const Automate * automate, int etat) {
    //créer l'ensemble résultat
    Ensemble* ens = creer_ensemble(NULL, NULL, NULL);
    
    //un état est accessible depuis lui-meme
    ajouter_element(ens, etat);
    
    //parcourir les états depuis "etat"
    etats_accessibles_recursive(automate, etat, ens);
    
    return ens;
}

/*==========================FIN ETATS ACCESSIBLES==========================*/

void action_accessibles(const intptr_t etat, void* data) {
    struct data_etats_t* dt = (struct data_etats_t*) data;
    ajouter_elements(dt->etats, etats_accessibles(dt->automate, etat));
}

Ensemble* accessibles(const Automate * automate) {
    Ensemble* ens = creer_ensemble(NULL, NULL, NULL);

    struct data_etats_t data;
    data.automate = automate;
    data.etats = ens;

    pour_tout_element(automate->initiaux, action_accessibles, &data);

    return ens;
}

/**
 * Supprimer les transitions si son origine est un état inaccessible;
 * ajouter dans le nouvel automate sinon.
 */
void action_automate_accessible(int origine, char lettre, int fin, void* data) {
    struct data_etats_t* dt = (struct data_etats_t*) data;

    if (est_dans_l_ensemble(dt->etats, origine) || est_dans_l_ensemble(dt->etats, fin))
        return;
    
    ajouter_transition((Automate*) dt->automate, origine, lettre, fin);
}

Automate *automate_accessible( const Automate * automate ){
    Automate* result = creer_automate();
    
    //l'ensemble des états accessibles depuis les états initiaux
    Ensemble* etats_accessibles = accessibles(automate);
    
    //..et la différence entre celle là et l'esemble des états d' l'automate
    Ensemble* etats_non_accessibles = creer_difference_ensemble(automate->etats, etats_accessibles);
    liberer_ensemble(etats_accessibles);
    
    //init data structure
    struct data_etats_t data;
    data.automate = result;
    data.etats = etats_non_accessibles;
    
    //Parcourir les transitions
    pour_toute_transition(automate, action_automate_accessible, &data);

    Ensemble* diff = NULL;
    
    //ajouter les états initiaux
    diff = creer_difference_ensemble(automate->initiaux, etats_non_accessibles);
    ajouter_elements(result->initiaux, diff);
    liberer_ensemble(diff);
    
    //ajouter les états initiaux
    diff = creer_difference_ensemble(automate->finaux, etats_non_accessibles);
    ajouter_elements(result->finaux, diff);
    liberer_ensemble(diff);
    
    return result;
}

/**
 * Ajouter une transition à un automate en la renversant
 * @param origine
 * @param lettre
 * @param fin
 * @param data
 */
void action_renverser_transitions(int origine, char lettre, int fin, void* data) {
    ajouter_transition((Automate*) data, fin, lettre, origine);
}

Automate *miroir(const Automate * automate) {
    //Initialiser un nouvel automate
    Automate* mir = creer_automate();

    //Pour toute transition de "automate": renverser et ajouter la à "mir"
    pour_toute_transition(automate, action_renverser_transitions, mir);

    //Ajouter les états initiaux de "automate" en tant qu'états finaux à "mir" et vice versa
    ajouter_elements(mir->finaux, automate->initiaux);
    ajouter_elements(mir->initiaux, automate->finaux);
    
    return mir;
}

/*==========================AUTOMATE MELANGE==========================*/

typedef struct data_melange_t {
    const Automate* automate_1;
    const Automate* automate_2;
    Automate* melange;
    Table* new_etats;
    int etat1, etat2;
    int mel_origine;
    intptr_t lettre;
} data_melange_t;

typedef struct Cle_couple {
	int etat1;
	int etat2;
} Cle_couple;

typedef struct data_boucle_t {
	intptr_t etat;
	Ensemble* ens2;
	void* extra_data;
	void (*action)(int etat1, int etat2, void* data);
} data_boucle_t;

int comparer_cle_couple(const Cle_couple *a, const Cle_couple *b) {
    if (a->etat1 < b->etat1)
        return -1;
    if (a->etat1 > b->etat1)
        return 1;
    if (a->etat2 < b->etat2)
        return -1;
    if (a->etat2 > b->etat2)
        return 1;
    
    return 0;
}

Cle_couple * copier_cle_couple(const Cle_couple* cle) {
    	Cle_couple * result = xmalloc( sizeof(Cle) );
	result->etat1 = cle->etat1;
        result->etat2 = cle->etat2;
	return result;
}

void supprimer_cle_couple(Cle_couple* cle) {
    xfree(cle);
}

/**
 * Récupérer le nom d'un état de l'automate de mélange à partir d'un couple (etat1, etat2) des automates sources
 */
int get_nom_etat(Table* new_etat, int etat1, int etat2){
    Cle_couple cle;
    cle.etat1 = etat1;
    cle.etat2 = etat2;
    
    Table_iterateur it = trouver_table(new_etat, (const intptr_t) &cle);
    if (iterateur_est_vide(it)){
        printf("[%d - %d]\n", etat1, etat2);
        return -1; // element non trouvé
    }
    
    return (int)get_valeur(it);
}

void action_produit_cartesien2( const intptr_t element, void* data_boucle ){
    data_boucle_t* dt = (data_boucle_t*) data_boucle;
    
    dt->action(dt->etat, element, dt->extra_data);
}

void action_produit_cartesien1( const intptr_t element, void* data_boucle ){
    data_boucle_t* dt = (data_boucle_t*) data_boucle;
    dt->etat = element;
    
    pour_tout_element((Ensemble*)dt->ens2, action_produit_cartesien2, dt);
}

/**
 * Cette fonction permet de parcourir les produits cartésiens de deux ensembles
 */
void produit_cartesien(Ensemble* ens1, Ensemble* ens2, void (*action)(int etat1, int etat2, void* data), void* data) {
    data_boucle_t data_boucle;
    data_boucle.ens2 = ens2;
    data_boucle.action = action;
    data_boucle.extra_data = data;

    pour_tout_element(ens1, action_produit_cartesien1, &data_boucle);
}

void action_ajouter_new_etat(int etat1, int etat2, void* data) {
    Cle_couple cle;
    cle.etat1 = etat1;
    cle.etat2 = etat2;

    int taille = taille_table((Table*)data);

	//ajouter un élément à la table dont la clé est un couple (etat1, etat2)
	//et la valeur est la taille courante de la table
    add_table((Table*)data, (intptr_t) & cle, taille);
}

void action_ajouter_transitions4( const intptr_t etat, void* data ) {
    data_melange_t* dt = (data_melange_t*)data;
    
    int mel_fin = get_nom_etat(dt->new_etats, dt->etat1, etat);
    ajouter_transition(dt->melange, dt->mel_origine, dt->lettre, mel_fin);
}

void action_ajouter_transitions3( const intptr_t etat, void* data ) {
    data_melange_t* dt = (data_melange_t*)data;
    
    int mel_fin = get_nom_etat(dt->new_etats, etat, dt->etat2);
    ajouter_transition(dt->melange, dt->mel_origine, dt->lettre, mel_fin);
}

void action_ajouter_transitions2( const intptr_t lettre, void* data ){
    data_melange_t* dt = (data_melange_t*) data;
    
    dt->mel_origine = get_nom_etat(dt->new_etats, dt->etat1, dt->etat2);
    dt->lettre = lettre;
    
    Cle cle;
    cle.lettre = lettre;
    cle.origine = dt->etat1;

	//On cherche les transitions de l'automate_1 dont l'origin est etat1
    Table_iterateur it = trouver_table(dt->automate_1->transitions, (const intptr_t) & cle);
    if (!iterateur_est_vide(it)){
		//si trouvé, on ajoute une nouvelle transition dans l'automate de mélange
        pour_tout_element((Ensemble*)get_valeur(it), action_ajouter_transitions3, dt);
    }

	//on fait pareillement avec l'atomate_2 et etat2
    cle.origine = dt->etat2;
    it = trouver_table(dt->automate_2->transitions, (const intptr_t) & cle);
    if (!iterateur_est_vide(it)){
        pour_tout_element((Ensemble*)get_valeur(it), action_ajouter_transitions4, dt);
    }
}

void action_ajouter_transitions1(int etat1, int etat2, void* data) {
    data_melange_t* dt = (data_melange_t*) data;
    dt->etat1 = etat1;
    dt->etat2 = etat2;

	//pour toute lettre de chaque alphabet:
    pour_tout_element(dt->automate_1->alphabet, action_ajouter_transitions2, dt);
    pour_tout_element(dt->automate_2->alphabet, action_ajouter_transitions2, dt);
}

void action_ajouter_initiaux(int etat1, int etat2, void* data) {
    data_melange_t* dt = (data_melange_t*) data;

	//récupérer le nom du nouvel état dans le map
    int initial = get_nom_etat(dt->new_etats, etat1, etat2);

	//ajouter cet état comme état initial à l'automate de mélange
    ajouter_etat_initial(dt->melange, initial);
}

void action_ajouter_finaux(int etat1, int etat2, void* data) {
    data_melange_t* dt = (data_melange_t*) data;

	//récupérer le nom du nouvel état dans le map
    int final = get_nom_etat(dt->new_etats, etat1, etat2);

	//ajouter cet état comme état final à l'automate de mélange
    ajouter_etat_final(dt->melange, final);
}

Automate * creer_automate_du_melange(
        const Automate* automate_1, const Automate* automate_2
        ) {
	//créer l'automate résultat
    Automate* melange = creer_automate();
    
    //créer un map pour gérer les noms d'états du nouvel automate
	//(etat1, etat2) -> nouveau_nom
    Table* new_etats = creer_table(
            (int(*)(const intptr_t, const intptr_t)) comparer_cle_couple,
            (intptr_t(*)(const intptr_t)) copier_cle_couple,
            (void(*)(intptr_t)) supprimer_cle_couple
            );
    
    //remplir le map avec les couples d'états automate_1->etats X automate_2->etats
    produit_cartesien(automate_1->etats, automate_2->etats, action_ajouter_new_etat, new_etats);

    //init la structure
    data_melange_t data;
    data.automate_1 = automate_1;
    data.automate_2 = automate_2;
    data.new_etats = new_etats;
    data.melange = melange;

	//créer les transitions à partir des automates sources
	//transition_melange(couple(etat1, etat2), lettre) = {
	//    couple(transition1(etat1, lettre), etat2),
	//    couple(etat1, transition2(etat2, lettre)
	// }
    produit_cartesien(automate_1->etats, automate_2->etats, action_ajouter_transitions1, &data);

	//créer les états initiaux à partir des automates sources
	//melange->initiaux = automate_1->initiaux X automate_2->initiaux
    produit_cartesien(automate_1->initiaux, automate_2->initiaux, action_ajouter_initiaux, &data);

	//créer les états finaux à partir des automates sources
	//melange->finaux = automate_1->finaux X automate_2->finaux
    produit_cartesien(automate_1->finaux, automate_2->finaux, action_ajouter_finaux, &data);
    
    liberer_table(new_etats);
    
    return melange;
}
/*==========================FIN AUTOMATE MELANGE==========================*/