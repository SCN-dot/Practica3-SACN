#ifndef RLE_H
#define RLE_H

#include <string>

const int RLE_ERR_TRUNCADO = 1;
const int RLE_ERR_CONTEO = 2;

std::string rleComprimir(const std::string& texto);
std::string rleDescomprimir(const std::string& comprimido);
void ejecutarRLE();

#endif