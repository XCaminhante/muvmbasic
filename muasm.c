//@+leo-ver=5-thin
//@+node:caminhante.20220906183805.1: * @file muasm.c
//@@tabwidth -2
//@+others
//@+node:caminhante.20220906184101.1: ** /includes
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
//@+node:caminhante.20220906184107.1: ** main
//@+others
//@+node:caminhante.20220906191922.1: *3* define_symbol
bool define_symbol (char *arg) {
  return false;
}
//@+node:caminhante.20220906192428.1: *3* compile_file
bool compile_file (char *arg) {
  return true;
}
//@+node:caminhante.20220906184311.1: *3* usage
void usage () {
  puts("Usage:\n"
       "muasm [-D name1=value1] arq1.asm [... arqN.asm]\n"
       "Compiles every .asm file into an .exe file\n"
       "-D defines symbols");
}
//@-others
//@+at
// [ zero ou mais símbolos, um ou mais nomes de arquivo ->
// símbolos carregados na tabela de símbolos, um ou mais arquivos compilados
// | mensagem explicando como usar o programa ]
//@@c
int main (int argc, char *argv[]) {
  // [ nenhum argumento além do caminho do executável -> mensagem explicativa ]
  if (argc == 1) { usage(); return EXIT_FAILURE; }
  // para iterar os argumentos:
  int arg = 1;
  // [ zero ou mais símbolos -> símbolos iniciais carregados na tabela de símbolos ]
  while ( arg < argc && define_symbol(argv[arg++]) ) {};
  // [ um ou mais arquivos -> arquivos compilados | mensagem explicativa ]
  bool compilation_success = true;
  do { compilation_success = compile_file(argv[arg++]); } while (arg < argc && compilation_success);
  if (arg < argc && !compilation_success) { usage(); return EXIT_FAILURE; }
  return EXIT_SUCCESS;
}
//@-others
//@-leo
