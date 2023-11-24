#include "../inc/linker.hpp"




int main(int argc, char *argv[]){
  try{


    string op=argv[0];
    if(op=="./linker"){
      Linker *l1=new Linker(argc,argv);
      delete(l1);
    }else {
      
    }

  }catch (exception& e) {

		cout << e.what();
    
	}

  return 0;
}