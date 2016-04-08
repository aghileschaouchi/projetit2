#include "automate.h"
#include "outils.h"

void action_pe( const intptr_t element){
	fprintf(stdout, "%ld", element );
	fprintf(stdout, ", " );
}

int test_accessibles(){
	int result = 1;

	{
		Automate * automate = creer_automate();

		ajouter_transition( automate, 1, 'b', 1 );
                ajouter_transition( automate, 1, 'a', 2 );
		ajouter_transition( automate, 2, 'a', 3 );
                ajouter_transition( automate, 3, 'a', 4 );
                
                ajouter_transition( automate, 4, 'b', 2 );
                
                ajouter_etat_initial( automate, 3);
		ajouter_etat_final( automate, 4);

                Ensemble* ens3 = creer_ensemble(NULL, NULL, NULL);
                ajouter_element(ens3, 3);
                ajouter_element(ens3, 4);
                ajouter_element(ens3, 2);
                
                print_ensemble(etats_accessibles(automate, 3), action_pe);
                
		TEST(
			1
			&& comparer_ensemble(accessibles(automate), ens3) == 0
			, result
		);
		liberer_automate( automate );
                liberer_ensemble(ens3);
	}

	return result;
}


int main(){

	if( ! test_accessibles() ){ return 1; };

	return 0;
	
}
