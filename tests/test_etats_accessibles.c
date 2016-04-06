#include "automate.h"
#include "outils.h"

void action_pe( const intptr_t element){
	fprintf(stdout, "%ld", element );
	fprintf(stdout, ", " );
}

int test_etats_accessibles(){
	int result = 1;

	{
		Automate * automate = creer_automate();

		ajouter_transition( automate, 1, 'b', 1 );
                ajouter_transition( automate, 1, 'a', 2 );
		ajouter_transition( automate, 2, 'a', 3 );
                ajouter_transition( automate, 3, 'a', 4 );
                
                //u-turn
                ajouter_transition( automate, 4, 'b', 2 );
                
		ajouter_etat_initial( automate, 1);
		ajouter_etat_final( automate, 4);

                Ensemble* ens1 = creer_ensemble(NULL, NULL, NULL);
                ajouter_element(ens1, 1);
                ajouter_element(ens1, 2);
                ajouter_element(ens1, 3);
                ajouter_element(ens1, 4);

                Ensemble* ens2 = creer_ensemble(NULL, NULL, NULL);
                ajouter_element(ens2, 2);
                ajouter_element(ens2, 3);
                ajouter_element(ens2, 4);
                
                print_ensemble(etats_accessibles(automate, 3), action_pe);
                
		TEST(
			1
			&& comparer_ensemble(etats_accessibles(automate, 1), ens1) == 0
                        && comparer_ensemble(etats_accessibles(automate, 2), ens2) == 0
                        && comparer_ensemble(etats_accessibles(automate, 3), ens2) == 0
                        && comparer_ensemble(etats_accessibles(automate, 4), ens2) == 0
			, result
		);
		liberer_automate( automate );
                liberer_ensemble(ens1);
                liberer_ensemble(ens2);
	}

	return result;
}


int main(){

	if( ! test_etats_accessibles() ){ return 1; };

	return 0;
	
}
