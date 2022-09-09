//@+leo-ver=5-thin
//@+node:caminhante.20220906183805.1: * @file muasm.c
//@@tabwidth -2
//@+others
//@+node:caminhante.20220906184101.1: ** /includes
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
//@+node:caminhante.20220908224210.1: ** /constants
#define SYMTABLE_SIZE 1000
//@+node:caminhante.20220908213955.1: ** /types
typedef const char * string;
struct symbol_entry { string name; string value; };
struct symbol_entry *symbol_table = {0};
#define alloc(SIZE,TYPE) \
  (TYPE*) calloc (SIZE, sizeof(TYPE))
#define salloc(SIZE) \
  alloc(SIZE+1,char)
//@+node:caminhante.20220906184107.1: ** main
//@+others
//@+node:caminhante.20220908214839.1: *3* str_is_equal
// [ duas strings C binariamente idênticas -> true | false ]
bool str_is_equal (string s1, string s2) {
  return strcmp(s1,s2) == 0;
}
//@+node:caminhante.20220906191922.1: *3* define_symbol
//@+others
//@+node:caminhante.20220908221947.1: *4* str_first_substring
//@+at
// // [ string s, ponteiro de corte entre o início de s e o caractere finalizador ->
//      nova string possuindo o trecho entre o início de s e o ponteiro de corte ]
//@@c
string str_first_substring (string s, char* cut) {
  // se o tamanho não for razoável, retornar falha:
  long size = (long) ( cut-s );
  if (size < 1) { return NULL; }
  // se a alocação falhar, retornar falha:
  char *ns = salloc(size);
  if (ns == NULL) { return NULL; }
  memcpy(ns,s,size);
  return ns;
}
//@+node:caminhante.20220908223405.1: *4* str_last_substring
//@+at
// // [ string s, ponteiro de corte entre o início de s e o caractere finalizador ->
//      nova string possuindo o trecho entre o ponteiro de corte e o caractere finalizador ]
//@@c
string str_last_substring (string s, char* cut) {
  // se o tamanho não for razoável, retornar falha:
  long ssize = strlen(s);
  long size = (long) ( s+ssize-cut );
  if (size < 1) { return NULL; }
  // se a alocação falhar, retornar falha:
  char *ns = salloc(size);
  if (ns == NULL) { return NULL; }
  memcpy(ns,cut,size);
  return ns;
}
//@-others
//@+at
// [ string no formato "<nome>=<valor>" ->
//   chave-valor salva na tabela de símbolos, true |
//   false ]
//@@c
bool define_symbol (string arg) {
  // se a string possuir um caractere '=', considerar seu ponteiro:
  char *eq = strchr(arg,'=');
  if (!eq) { return false; }
  // se a primeira parte da string não possuir caracteres, retornar falha:
  string key = str_first_substring(arg,eq);
  if (!key) { return false; }
  // se a segunda parte da string não possuir caracteres, retornar falha:
  string value = str_last_substring(arg,eq+1);
  if (!value) { return false; }
  //TODO armazenar na tabela
  return true;
}
//@+node:caminhante.20220906192428.1: *3* compile_file
//@+at
// [ caminho válido para um arquivo contendo assembly muvm ->
//   resultado compilado em outro arquivo na mesma pasta, true |
//   false ]
//@@c
bool compile_file (string arg) {
  return true;
}
//@+node:caminhante.20220906184311.1: *3* usage
// [ nenhuma entrada -> mensagem explicativa na saída padrão ]
void usage () {
  puts("Usage:\n"
       "muasm [-D name1=value1] arq1.asm [... arqN.asm]\n"
       "\n"
       "Compiles every .asm file into an .exe file\n"
       "You must compile at least one file\n"
       "-D defines symbols");
}
//@-others
//@+at
// [ zero ou mais símbolos, um ou mais nomes de arquivo ->
//   símbolos carregados na tabela, um ou mais arquivos compilados |
//   mensagem explicativa na saída padrão ]
//@@c
int main (int argc, char *argv[]) {
  // se não houver item em argv além do caminho do executável, mensagem e encerramento:
  if (argc == 1) { usage(); return EXIT_FAILURE; }
  // para iterar os argumentos:
  int arg = 1;
  symbol_table = alloc(SYMTABLE_SIZE, struct symbol_entry);
  // [ zero ou mais símbolos -> símbolos iniciais carregados na tabela de símbolos ]
  // se ainda há argumentos e o atual é igual à "-D":
  while ( arg < argc && str_is_equal(argv[arg],"-D") ) {
    // se não houver mais um argumento além do atual, mensagem e encerramento:
    if (arg+1 >= argc) { usage(); return EXIT_FAILURE; }
    // se o parsing do argumento seguinte falhar, mensagem e encerramento:
    if (!define_symbol( argv[arg+1] )) { usage(); return EXIT_FAILURE; }
    arg += 2;
  }
  // [ um ou mais arquivos válidos -> arquivos compilados | mensagem explicativa ]
  // se não houver mais argumentos, mensagem e encerramento:
  if (arg == argc) { usage(); return EXIT_FAILURE; }
  // para iterar os arquivos:
  //TODO iterar os arquivos
  return EXIT_SUCCESS;
}
//@-others
//@-leo
