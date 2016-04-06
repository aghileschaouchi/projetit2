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


int main(){

	if( ! test_union() ){ return 1; };

	return 0;
	
}
