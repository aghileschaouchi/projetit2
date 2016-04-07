#include "automate.h"
#include "outils.h"

int test_union(){
	int result = 1; 
        
        Automate * automate_1 = mot_to_automate("a");
        Automate * automate_2 = mot_to_automate("b");

        Automate * aut_union = creer_union_des_automates( automate_1, automate_2);

        print_automate(automate_1);
        print_automate(automate_2);
        print_automate(aut_union);
        
        TEST(
                1
                && aut_union
                && le_mot_est_reconnu( aut_union, "a" )
                && le_mot_est_reconnu( aut_union, "b" )
                && ! le_mot_est_reconnu( aut_union, "ab" )
                && ! le_mot_est_reconnu( aut_union, "abb" )
                && ! le_mot_est_reconnu( aut_union, "aa" )
                && ! le_mot_est_reconnu( aut_union, "aaa" )
                && ! le_mot_est_reconnu( aut_union, "aba" )
                && ! le_mot_est_reconnu( aut_union, "bba" )
                , result
        );
        liberer_automate( automate_1 );
        liberer_automate( automate_2 );
        liberer_automate( aut_union );

	return result;
}

int test_union2(){ // le bon test
	int result = 1; 
        
        Automate * automate_1 = creer_automate();

		ajouter_transition( automate_1, 1, 'a', 2 );
		ajouter_transition( automate_1, 2, 'a', 2 );
                ajouter_transition( automate_1, 2, 'b', 2 );
		ajouter_etat_initial( automate_1, 1);
		ajouter_etat_final( automate_1, 2);
                
        Automate * automate_2 = creer_automate();

		ajouter_transition( automate_2, 3, 'a', 3 );
		ajouter_transition( automate_2, 3, 'b', 4 );
                ajouter_transition( automate_2, 4, 'a', 3 );
                ajouter_transition( automate_2, 4, 'b', 4 );
		ajouter_etat_initial( automate_2, 3);
		ajouter_etat_final( automate_2, 4);

        Automate * aut_union = creer_union_des_automates( automate_1, automate_2);

        print_automate(automate_1);
        print_automate(automate_2);
        print_automate(aut_union);
        
        TEST(
                1
                && aut_union
                && le_mot_est_reconnu( aut_union, "a" )
                && le_mot_est_reconnu( aut_union, "b" )
                && !le_mot_est_reconnu( automate_1, "baab")
                && le_mot_est_reconnu( automate_2, "baab")
                && le_mot_est_reconnu( aut_union, "baab")
                && !le_mot_est_reconnu( automate_1, "baababa")
                && !le_mot_est_reconnu( aut_union, "baababa" )
                && !le_mot_est_reconnu( automate_2, "baababa" )
                && !le_mot_est_reconnu( automate_1, "baa")
                && !le_mot_est_reconnu( automate_2, "baa" )
                && !le_mot_est_reconnu( aut_union, "baa" )
                && le_mot_est_reconnu( aut_union, "bbb" )
                && !le_mot_est_reconnu( automate_1, "bbb")
                && le_mot_est_reconnu( automate_2, "bbb")
                && le_mot_est_reconnu( aut_union, "baaab" )
                && !le_mot_est_reconnu( automate_1, "baaab" )
                && le_mot_est_reconnu( automate_2, "baaab" )
                && le_mot_est_reconnu( automate_1, "aa" )
                && !le_mot_est_reconnu( automate_2, "aa" )
                && le_mot_est_reconnu( aut_union, "aa" )
                && le_mot_est_reconnu( automate_1, "aba" )
                && !le_mot_est_reconnu( automate_2, "aba" )
                && le_mot_est_reconnu( aut_union, "aba" )
                && le_mot_est_reconnu( automate_1, "abab" )
                && le_mot_est_reconnu( automate_2, "abab" )
                && le_mot_est_reconnu( aut_union, "abab" )
                
                , result
        );
        liberer_automate( automate_1 );
        liberer_automate( automate_2 );
        liberer_automate( aut_union );

	return result;
}


int main(){

	if( ! test_union2() ){ return 1; };

	return 0;
	
}
