#ifndef LZ78_H
#define LZ78_H

const int LZ78_ERR_FORMATO = 1;
const int LZ78_ERR_INDICE = 2;

unsigned char* lz78Comprimir(const unsigned char* entrada, int lonEntrada, int& lonSalida);
unsigned char* lz78Descomprimir(const unsigned char* entrada, int lonEntrada, int& lonSalida);
void lz78MostrarPares(const unsigned char* comprimido, int lon);
void ejecutarLZ78();

#endif // LZ78_H
