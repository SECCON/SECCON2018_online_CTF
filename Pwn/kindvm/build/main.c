#include <stdio.h>

#include "kindvm.h"
#include "util.h"

// 1. Input "flag.txt" as username
// 2. Obtain user_name pointer
// 3. Overwrite banner_file_name
// 4. Obtain greeting pointer
// 5. Overwrite farewell pointer


int main(void){
	
	ctf_setup();
	kindvm_setup();

	input_insn();

	kc->greeting();

	while(!kc->isstop){
		exec_insn();
	}

	kc->farewell();

	return 0;
}
