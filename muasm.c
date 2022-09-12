//@+leo-ver=5-thin
//@+node:caminhante.20220906183805.1: * @file muasm.c
//@@tabwidth -2
//@+others
//@+node:caminhante.20220906184101.1: ** /includes
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdarg.h>
//@+node:caminhante.20220908224210.1: ** /constants
#define SYMTABLE_SIZE 1000
#define VERSION_MAJOR "0"
#define VERSION_MINOR "1"
#define MONOTONIC_VERSION VERSION_MAJOR "." VERSION_MINOR
#define VERSION MONOTONIC_VERSION
#define USAGE \
  "MUASM v. "VERSION": Modified Mu0 Assembler\n" \
  "Usage: muasm [-m 4096] [-D name1=value1] arq1.asm [... arqN.asm]\n" \
  "Compiles every .asm file into an .exe file\n" \
  "Optional settings (must appear at the right order):\n" \
  " -m\tset the limit of entries for the symbol table\n" \
  " -D\tdefines symbols, one pair of '-D name=value' for each"
//@+node:caminhante.20220909191047.1: ** debug_msg
#ifdef DEBUG
  #define debug_msg(MSG,ARGS...) \
    fprintf(stderr,"#%s: " MSG,__func__,##ARGS)
#else
  #define debug_msg(MSG,ARGS...)
#endif
//@+node:caminhante.20220908213955.1: ** /types
typedef uint64_t natural;
#define alloc(SIZE,TYPE) \
  (TYPE*) calloc (SIZE, sizeof(TYPE))
#define salloc(SIZE) \
  alloc(SIZE+1,char)
#define free(POINTER) \
  free((void*) POINTER)
//@+others
//@+node:caminhante.20220909185249.1: *3* typedef string
typedef const char * string;
//@+others
//@+node:caminhante.20220908214839.1: *4* str_is_equal
// [ duas strings C binariamente idênticas -> true | false ]
bool str_is_equal (string s1, string s2) {
  if (!s1 || !s2) { return false; }
  return strcmp(s1,s2) == 0;
}
//@+node:caminhante.20220911222419.1: *4* str_starts_with
// [ a primeira string inicia-se com o conteúdo da segunda string (o prefixo) -> true | false ]
bool str_starts_with (string str, string prefix) {
  if (!str || !prefix) { return false; }
  natural slen = strlen(str), plen = strlen(prefix);
  debug_msg("str len: %lu, prefix len: %lu\n",slen,plen);
  // se o tamanho das strings for insuficiente, falha:
  if (slen < 2 || plen < 1) { return false; }
  // se o prefixo tiver tamanho maior que a string, falha:
  if (plen > slen) { return false; }
  return strncmp(str,prefix,plen) == 0;
}
//@+node:caminhante.20220911223247.1: *4* str_ends_with
// [ a primeira string termina com o conteúdo da segunda string (o sufixo) -> true | false ]
bool str_ends_with (string str, string suffix) {
  if (!str || !suffix) { return false; }
  natural s1len = strlen(str), s2len = strlen(suffix);
  // se o tamanho das strings for insuficiente, falha:
  if (s1len < 2 || s2len < 1) { return false; }
  // se o sufixo tiver tamanho maior que a string, falha:
  if (s2len > s1len) { return false; }
  return strncmp(str+s1len-s2len,suffix,s2len) == 0;
}
//@+node:caminhante.20220909174114.1: *4* str_copy
// [ string válida -> cópia | NULL ]
string str_copy (string str) {
  // se ponteiro nulo ou tamanho igual a zero, falha:
  if (!str) { return NULL; }
  natural l = strlen(str);
  if (l == 0) { return NULL; }
  char *s = salloc(l);
  return strncpy(s,str,l);
}
//@+node:caminhante.20220911215910.1: *4* str_concat
// [ duas strings -> primeira string realocada, com conteúdo da segunda adicionado ao final | NULL ]
string str_concat (string str, string append) {
  if (!str || !append) { return NULL; }
  natural slen = strlen(str), alen = strlen(append);
  if (slen+alen < 1) { return NULL; }
  string nstr = realloc((char*)str,slen+alen+1);
  if (!nstr) { return NULL; }
  strncpy((char*)nstr+slen,append,alen);
  return nstr;
}
//@+node:caminhante.20220908221947.1: *4* str_first_substring
//@+at
// [ string s, ponteiro de corte entre o início de s e o caractere finalizador ( início | corte | \0 ) ->
//   nova string possuindo o trecho entre o início de s e o ponteiro de corte ( início - corte ) ]
//@@c
string str_first_substring (string s, char* cut) {
  // se o tamanho for menor que 1, falha:
  long size = (long) ( cut-s );
  if (size < 1) { return NULL; }
  // se a alocação falhar, falha:
  char *ns = salloc(size);
  if (ns == NULL) { return NULL; }
  memcpy(ns,s,size);
  return ns;
}
//@+node:caminhante.20220908223405.1: *4* str_last_substring
//@+at
// [ string s, ponteiro de corte entre o início de s e o caractere finalizador ( início | corte | \0 ) ->
//   nova string possuindo o trecho entre o ponteiro de corte e o caractere finalizador ( corte - \0 ) ]
//@@c
string str_last_substring (string s, char* cut) {
  // se o tamanho for menor que 1, falha:
  long ssize = strlen(s);
  long size = (long) ( s+ssize-cut );
  if (size < 1) { return NULL; }
  // se a alocação falhar, falha:
  char *ns = salloc(size);
  if (ns == NULL) { return NULL; }
  memcpy(ns,cut,size);
  return ns;
}
//@-others
//@+node:caminhante.20220909164546.1: *3* struct symbol_table
struct symbol_entry { string key; string value; };
struct symbol_table { natural capacity; natural used; struct symbol_entry *symbols; };
struct symbol_table symbol_table = {0,0};
#define EMPTY_SYMBOL (struct symbol_entry){0,0}
//@+node:caminhante.20220909175809.1: *4* symbol_entry_valid
// [ símbolo contendo todos os campos ocupados com valores válidos -> true | false ]
bool symbol_entry_valid (struct symbol_entry symbol) {
  // se qualquer ponteiro for nulo, falha:
  if ( !symbol.key || !symbol.value ) { return false; }
  // se qualquer string não tiver tamanho maior que zero, falha:
  if ( !strlen(symbol.key) || !strlen(symbol.value) ) { return false; }
  return true;
}
//@+node:caminhante.20220909164642.1: *4* symbol_table_initialize
// [ tamanho da tabela de símbolos -> vetor de símbolos alocado, capacidade da tabela alterada, true | false ]
bool symbol_table_initialize (unsigned long capacity) {
  // se já há memória alocada apontada na tabela, falha:
  if (symbol_table.symbols) { return false; }
  symbol_table.symbols = alloc(capacity, struct symbol_entry);
  // se a alocação falhou, garantir capacidade=0 e falha:
  if (!symbol_table.symbols) { symbol_table.capacity = 0; return false; }
  symbol_table.capacity = capacity;
  symbol_table.used = 0;
  return true;
}
//@+node:caminhante.20220909172114.1: *4* symbol_table_grow
// [ capacidade maior do que a usada atualmente -> realocação do vetor de símbolos com mais espaço, true | false ]
//TODO pode ser útil no futuro
//@+node:caminhante.20220909173028.1: *4* symbol_table_clear
// sobrescreve o vetor de símbolos com bytes 0, zera contador de uso da capacidade
void symbol_table_clear () {
  // se a tabela não possui entradas, ignorar:
  if (!symbol_table.capacity || !symbol_table.symbols) { return; }
  // zerar contador de uso da capacidade:
  symbol_table.used = 0;
  // sobreescrever o vetor de símbolos:
  memset(symbol_table.symbols, '\0', sizeof(struct symbol_entry)*symbol_table.capacity);
}
//@+node:caminhante.20220909171837.1: *4* symbol_table_index
// [ chave encontrada na tabela de símbolos -> índice atual da entrada (contado a partir de 1) | 0 ]
natural symbol_table_index (string key) {
  // se a chave fornecida é inválida, falha:
  if (!key || !strlen(key)) { return 0; }
  // se a tabela não possui entradas, falha:
  if (!symbol_table.capacity || !symbol_table.symbols) { return 0; }
  // para cada entrada na tabela:
  struct symbol_entry *s = symbol_table.symbols;
  debug_msg("looping entries:\n");
  natural viewed = 0;
  // enquanto houver entradas não conferidas:
  for (natural i = 1; i<=symbol_table.capacity && viewed<symbol_table.used; i++) {
    debug_msg("s[i-1].key: %p\n", (void*)s[i-1].key);
    // se a entrada atual não está vazia:
    if (s[i-1].key) {
      // contar como vista:
      viewed++;
      // se a chave da entrada for igual à chave fornecida, retorne o índice:
      if (str_is_equal(s[i-1].key, key)) { return i; }} }
  return 0;
}
//@+node:caminhante.20220909173718.1: *4* symbol_table_first_empty
// [ entrada vazia na tabela -> índice da primeira entrada vazia (contado a partir de 1) | 0 ]
natural symbol_table_first_empty () {
  // se a tabela não possui entradas, falha:
  if (!symbol_table.capacity || !symbol_table.symbols) { return 0; }
  // para cada entrada na tabela:
  struct symbol_entry *s = symbol_table.symbols;
  for (natural i = 1; i<=symbol_table.capacity; i++) {
    // se a entrada está vazia, retorne o índice:
    if (s[i-1].key == 0) { return i; }}
  return 0;
}
//@+node:caminhante.20220909164521.1: *4* symbol_table_lookup
// [ chave encontrada na tabela de símbolos -> item copiado da tabela | EMPTY_SYMBOL ]
struct symbol_entry symbol_table_lookup (string key) {
  natural i = symbol_table_index(key);
  if (!i) { return EMPTY_SYMBOL; }
  return symbol_table.symbols[i-1];
}
//@+node:caminhante.20220909164430.1: *4* symbol_table_has
// [ chave encontrada na tabela -> true | false ]
bool symbol_table_has (string key) {
  natural i = symbol_table_index(key);
  return i != 0;
}
//@+node:caminhante.20220909164526.1: *4* symbol_table_insert
//@+at
// [ entrada de símbolo válida ->
//   [ chave já existente ->
//     valor na entrada da tabela substituído pelo fornecido, true |
//     [ há espaço livre na tabela de símbolos ->
//       nova entrada adicionada na tabela, contador de entradas usadas incrementado, true |
//       false ] ] |
//   false ]
//@@c
bool symbol_table_insert (struct symbol_entry symbol) {
  // se a entrada for inválida, falha:
  if (!symbol_entry_valid(symbol)) { return false; }
  // se a tabela não possui entradas, falha:
  if (!symbol_table.capacity || !symbol_table.symbols) { return false; }
  struct symbol_entry *s = symbol_table.symbols;
  // procurar pela chave na tabela:
  natural i = symbol_table_index(symbol.key);
  debug_msg("symbol_table_index(symbol.key): %lu\n",i);
  // se a chave for encontrada:
  if (i != 0) {
    // liberar memória do valor anterior:
    //NOTE o ponteiro abaixo só pode ser inválido em caso de erro de programação ou corrupção de memória
    debug_msg("s[i-1].value (must be != nil): %p\n", (void*)s[i-1].value);
    free(s[i-1].value);
  // se a chave não foi encontrada, inserir símbolo em nova entrada na tabela (desde que haja espaço livre):
  } else {
    // se não houver espaço livre, falha:
    if ( symbol_table.capacity <= symbol_table.used ) { return false; }
    // incrementar o contador de entradas ocupadas:
    symbol_table.used++;
    // redefinir i para o índice da primeira entrada vazia disponível:
    i = symbol_table_first_empty();
    // caso nenhuma entrada vazia seja encontrada, falha:
    //NOTE essa condição não é possível sem erro de programação ou corrupção de memória
    debug_msg("symbol_table_first_empty() (must be != 0): %lu\n",i);
    if (!i) { return false; }
    // substituir o campo de chave da entrada pelo do símbolo fornecido:
    s[i-1].key = str_copy(symbol.key);
  }
  // se nenhuma falha ocorreu até aqui, substituir o campo de valor da entrada pelo do símbolo fornecido:
  s[i-1].value = str_copy(symbol.value);
  // se o processo de cópia de uma das strings falhar, falha:
  if (!s[i-1].key || !s[i-1].value) { return false; }
  return true;
}
//@+node:caminhante.20220909165140.1: *4* symbol_table_delete
//@+at
// [ chave encontrada ->
//   entrada correspondente excluída da tabela de símbolos,
//   contador de entradas usadas decrementado, true |
//   false ]
//@@c
bool symbol_table_delete (string key) {
  natural i = symbol_table_index(key);
  if (!i) { return false; }
  struct symbol_entry *s = symbol_table.symbols;
  // liberar memória antes de apagar os ponteiros:
  //NOTE os ponteiros abaixo só podem ser inválidos em caso de erro de programação ou corrupção de memória
  debug_msg("s[i-1].key (must be != nil): %p\n",s[i-1].key);
  debug_msg("s[i-1].value (must be != nil): %p\n",s[i-1].value);
  free(s[i-1].key); free(s[i-1].value);
  s[i-1].key = 0; s[i-1].value = 0;
  // decrementar contador de entradas usadas:
  debug_msg("symbol_table.used (must be >0): %lu\n",symbol_table.used);
  symbol_table.used--;
  return true;
}
//@+node:caminhante.20220909165233.1: *4* symbol_table_vacuum
// reorganiza a tabela de símbolos para colocar todas as entradas preenchidas no início da tabela
void symbol_table_vacuum () {
  // natural empty, occupied;
  //TODO implementar
  // enquanto existir entrada vazia anterior à entradas preenchidas,
  // mover 01 entrada preenchida por vez para a vazia selecionada:
    // localizar a próxima vazia e a próxima preenchida após a vazia:
    // mover a preenchida selecionada para a atual vazia:
}
//@+node:caminhante.20220909190158.1: *4* symbol_table_list
// para fins de teste, lista todos os símbolos na tabela com seus valores
void symbol_table_list () {
  // se a tabela não possui entradas, ignore:
  if (!symbol_table.capacity || !symbol_table.symbols) { return; }
  // para cada entrada na tabela:
  struct symbol_entry *s = symbol_table.symbols;
  natural viewed = 0;
  printf("#listing all symbol table entries:\n");
  debug_msg("looping entries:\n");
  for (natural i = 1; i<=symbol_table.capacity && viewed<symbol_table.used; i++) {
    debug_msg("s[i-1].key: %p\n", (void*)s[i-1].key);
    // se a entrada não estiver vazia, exiba seu conteúdo na saída padrão:
    if (s[i-1].key) {
      viewed++;
      printf("#entry '%s': '%s'\n", s[i-1].key, s[i-1].value); }}
}
//@-others
//@+node:caminhante.20220906184107.1: ** main
//@+others
//@+node:caminhante.20220909163111.1: *3* warning
// [ mensagem -> mensagem explicativa na saída de erros ]
void warning (string message) {
  if (message) { fprintf(stderr,"%s\n",message); }
}
//@+node:caminhante.20220909162530.1: *3* failure
// [ mensagem -> mensagem explicativa na saída de erros, programa encerrado ]
void failure (string message) {
  if (message) { warning(message); }
  warning(USAGE); exit(EXIT_FAILURE);
}
//@+node:caminhante.20220906191922.1: *3* define_symbol
//@+others
//@-others
//@+at
// [ string no formato "<nome>=<valor>" ->
//   entrada de símbolo inserida na tabela de símbolos, true |
//   false ]
//@@c
bool define_symbol (string arg) {
  char *eq = strchr(arg,'=');
  // se a string não possuir um caractere '=', falha:
  if (!eq) { return false; }
  // se a primeira parte da string for inválida, falha:
  string key = str_first_substring(arg,eq);
  if (!key) { return false; }
  // se a segunda parte da string for inválida, falha:
  string value = str_last_substring(arg,eq+1);
  if (!value) { return false; }
  // se a inserção na tabela falhar, falha:
  struct symbol_entry s = {key,value};
  if (!symbol_table_insert(s)) { return false; }
  free(key); free(value);
  return true;
}
//@+node:caminhante.20220906192428.1: *3* compile_file
//@+others
//@+node:caminhante.20220911234346.1: *4* file_output_path
// [ caminho do arquivo de entrada -> caminho do arquivo de saída | NULL ]
string file_output_path (string input_path) {
  // obter nome do arquivo sem extensão, para adicionar a extensão nova:
  char *dot = strrchr(input_path,'.');
  string basename;
  // caso o caractere seja o primeiro da string, ignorar dot:
  if (dot == input_path) { dot = NULL; }
  // caso o início do nome do arquivo seja '..' e dot aponte para o segundo caractere, ignorar dot:
  if (str_starts_with(input_path,"..") && dot == input_path+1) { dot = NULL; }
  if (!dot) {
    debug_msg("no dot case\n");
    // caso o caractere '.' não seja encontrado, considerar o nome tal como está:
    basename = str_copy(input_path);
  } else {
    debug_msg("dot case\n");
    // se encontrado, extrair o conteúdo antes do caractere '.':
    basename = str_first_substring(input_path, dot);
  }
  debug_msg("basename: %p\n",(void*)basename);
  // se a alocação de string falhar, falha:
  if (!basename) { return NULL; }
  debug_msg("basename '%s'\n",basename);
  // concatene a nova extensão na base do nome do arquivo
  string output_path = str_concat(basename,".exe");
  // se a realocação da concatenação falhar, falha:
  if (!output_path) { return NULL; }
  return output_path;
}
//@-others
//@+at
// [ caminho válido para um arquivo contendo assembly muvm ->
//   resultado compilado em outro arquivo na mesma pasta, true |
//   false ]
//@@c
bool compile_file (string file_input) {
  debug_msg("file input: '%s'\n",file_input);
  string file_output = file_output_path(file_input);
  if (!file_output) { return false; }
  debug_msg("file output: '%s'\n",file_output);
  // preparar acessos de arquivos de entrada e saída:
  FILE* input = fopen(file_input, "r");
  if (!input) { return false; }
  FILE* output = fopen(file_output, "w");
  if (!output) { return false; }
  // obter o tamanho do arquivo para alocar o espaço do código-fonte:
  fseek(input, 0, SEEK_END);
  natural input_len = ftell(input);
  fseek(input, 0, SEEK_SET);
  // se o arquivo estiver vazio, remova o arquivo de saída e ignore o passo de compilação:
  if (input_len == 0) { remove(file_output); goto end; }
  // alocar e ler o código-fonte de uma só vez para a memória:
  string source = salloc(input_len);
  if (!source) { return false; }
  natural actual_read = fread(source, 1, input_len, input);
  // se a leitura for interrompida, falha:
  if (actual_read != input_len) { return false; }
  source[input_len] = '\0';
  // passar pelo ciclo de compilação:
  //TODO
  // enviar resultado para arquivo de saída:
  //TODO
  end:
  fclose(input); fclose(output);
  free(file_output);
  return true;
}
//@+node:caminhante.20220909163746.1: *3* tests
void tests () {
  //TODO implementar infraestrutura para o testador TAP ao longo do código-fonte
  symbol_table_list();
}
//@-others
//@+at
// [ zero ou mais símbolos, um ou mais nomes de arquivo ->
//   símbolos carregados na tabela, um ou mais arquivos compilados |
//   encerramento com falha ]
//@@c
int main (int argc, char *argv[]) {
  debug_msg("binary compiled on DEBUG mode\n");
  // se não houver item em argv além do caminho do executável, falha:
  if (argc == 1) { failure("Required: one or more arguments"); }
  debug_msg("argc: %d\n",argc);
  // para iterar os argumentos:
  natural arg = 1;
  natural table_size = SYMTABLE_SIZE;
  // se o usuário passou argumento para definir o tamanho da tabela de símbolos:
  if (str_is_equal(argv[arg],"-m")) {
    // se não houver mais um argumento além do atual, falha:
    if (arg+1 >= argc) { failure("Required: one argument for -m switch"); }
    // se o parsing do argumento seguinte falhar, falha:
    char *parse_end = NULL;
    table_size = strtoul(argv[arg+1],&parse_end,10);
    // se o valor for igual a zero, ou o ponteiro de fim não coincidir com o final da string do argumento, falha:
    //NOTE strtoul aceita valores negativos, porém faz casting corretamente...
    //NOTE o problema é que os valores ficam enormes
    //NOTE mas isso não parece muito importante porque a alocação da tabela falha conforme esperado
    //NOTE para impor um formato numérico seria preciso usar outro parser
    if (table_size == 0 || (parse_end < argv[arg+1] + strlen(argv[arg+1]) )) { failure("Error: -m argument parsing failed"); }
    arg++; }
  // se a alocação da tabela de símbolos falhar, falha:
  debug_msg("table_size: %lu\n",table_size);
  if (!symbol_table_initialize(table_size)) { failure("Could not allocate memory for the symbol table"); }
  // se ainda há argumentos e o atual é igual à "-D":
  // [ zero ou mais símbolos -> símbolos iniciais carregados na tabela de símbolos | falha ]
  while ( arg < argc && str_is_equal(argv[arg],"-D") ) {
    // se não houver mais um argumento além do atual, falha:
    if (arg+1 >= argc) { failure("Required: one argument for -D switch"); }
    // se o parsing do argumento seguinte falhar, falha:
    if (!define_symbol( argv[arg+1] )) { failure("Error: symbol definition parsing failed"); }
    arg += 2; }
  // [ um ou mais arquivos válidos -> arquivos compilados | falha ]
  // se não houver mais argumentos, mensagem e encerramento:
  debug_msg("arg: %lu\n",arg);
  if (arg == argc) { failure("Required: one or more files to compile"); }
  // para cada arquivo:
  debug_msg("iterating files:\n");
  for (;arg < argc; arg++) {
    // se o arquivo não existir, falha:
    if (access(argv[arg], F_OK) != 0) { failure("Error: file is not accessible"); }
    // se ocorrer falha ao compilar o arquivo, falha:
    if (!compile_file(argv[arg])) { failure("Error: file could not be compiled"); }
  }
#ifdef DEBUG
  tests();
#endif
  return EXIT_SUCCESS;
}
//@-others
//@-leo
