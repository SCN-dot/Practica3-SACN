#include "Encriptado.h".h"
#include <iostream>

using namespace std;

const int TAM_MAX_TEXTO = 1024;

static void validarRotacion(int n)
{
    if (n <= 0 || n >= 8)
        throw CRYPTO_ERR_ROTACION;
}

unsigned char rotarIzquierda(unsigned char byte, int n)
{
    validarRotacion(n);
    return (unsigned char)(((byte << n) | (byte >> (8 - n))) & 0xFF);
}

unsigned char rotarDerecha(unsigned char byte, int n)
{
    validarRotacion(n);
    return (unsigned char)(((byte >> n) | (byte << (8 - n))) & 0xFF);
}

unsigned char* encriptar(const unsigned char* datos, int lon, int n, unsigned char clave)
{
    if (datos == nullptr || lon == 0)
        throw "No hay datos para encriptar";
    validarRotacion(n);

    unsigned char* resultado = new unsigned char[lon];
    for (int i = 0; i < lon; i++)
        resultado[i] = rotarIzquierda(datos[i], n) ^ clave;

    return resultado;
}

unsigned char* desencriptar(const unsigned char* datos, int lon, int n, unsigned char clave)
{
    if (datos == nullptr || lon == 0)
        throw "No hay datos para desencriptar";
    validarRotacion(n);

    unsigned char* resultado = new unsigned char[lon];
    for (int i = 0; i < lon; i++)
        resultado[i] = rotarDerecha(datos[i] ^ clave, n);

    return resultado;
}

static int leerEntero(const char* mensaje)
{
    int valor;
    cout << mensaje;

    bool ok = (bool)(cin >> valor);
    if (!ok)
        cin.clear();
    cin.ignore(100000, '\n');

    if (!ok)
        throw "La entrada no es un numero valido";
    return valor;
}

void ejecutarCrypto()
{
    cout << "Encriptacion y desencriptacion " << endl;
    cout << "Ingrese el texto: ";

    char texto[TAM_MAX_TEXTO];
    cin.getline(texto, TAM_MAX_TEXTO);
    if (cin.fail()) {
        cin.clear();
        cin.ignore(100000, '\n');
        cout << "(El texto era muy largo; se usan solo los primeros " << TAM_MAX_TEXTO - 1 << " caracteres)" << endl;
    }

    int lonTexto = 0;
    while (texto[lonTexto] != '\0')
        lonTexto++;

    const unsigned char* datos = reinterpret_cast<const unsigned char*>(texto);

    unsigned char* cifrado = nullptr;
    unsigned char* recuperado = nullptr;

    try {
        int n = leerEntero("Rotacion n (0 < n < 8): ");
        int k = leerEntero("Clave K (0 a 255): ");
        if (k < 0 || k > 255)
            throw CRYPTO_ERR_CLAVE;

        cifrado = encriptar(datos, lonTexto, n, (unsigned char)k);
        cout << "Texto original: " << texto << endl;
        cout << "Encriptado (valores): ";
        for (int i = 0; i < lonTexto; i++)
            cout << (int)cifrado[i] << " ";
        cout << endl;

        recuperado = desencriptar(cifrado, lonTexto, n, (unsigned char)k);
        cout << "Desencriptado: ";
        for (int i = 0; i < lonTexto; i++)
            cout << (char)recuperado[i];
        cout << endl;

        bool iguales = true;
        for (int i = 0; i < lonTexto; i++) {
            if (recuperado[i] != datos[i]) {
                iguales = false;
                break;
            }
        }

        if (iguales)
            cout << "Verificacion: OK, el texto desencriptado coincide con el original." << endl;
        else
            cout << "Verificacion: ERROR, el texto desencriptado es distinto del original." << endl;
    }
    catch (const char* msg) {
        cerr << "Error: " << msg << endl;
    }
    catch (int codigo) {
        cerr << "Error numero " << codigo << ": ";
        if (codigo == CRYPTO_ERR_ROTACION) cerr << "n debe cumplir 0 < n < 8" << endl;
        else if (codigo == CRYPTO_ERR_CLAVE) cerr << "la clave debe estar entre 0 y 255" << endl;
        else cerr << "error desconocido" << endl;
    }
    catch (...) {
        cerr << "Error inesperado" << endl;
    }

    delete[] cifrado;
    delete[] recuperado;
}
