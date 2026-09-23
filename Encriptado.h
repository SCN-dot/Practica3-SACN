#ifndef ENCRIPTADO_H
#define ENCRIPTADO_H

const int CRYPTO_ERR_ROTACION = 1;
const int CRYPTO_ERR_CLAVE = 2;

unsigned char rotarIzquierda(unsigned char byte, int n);
unsigned char rotarDerecha(unsigned char byte, int n);

unsigned char* encriptar(const unsigned char* datos, int lon, int n, unsigned char clave);
unsigned char* desencriptar(const unsigned char* datos, int lon, int n, unsigned char clave);

void ejecutarCrypto();

#endif // ENCRIPTADO_H
