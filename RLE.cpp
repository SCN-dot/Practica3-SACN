#include "rle.h"
#include <iostream>

using namespace std;

const int MAX_CORRIDA = 9;

string rleComprimir(const string& texto)
{
    if (texto.empty())
        throw "El texto a comprimir esta vacio";

    string comprimido;
    int n = (int)texto.size();
    int i = 0;

    while (i < n) {
        char simbolo = texto[i];
        int cuenta = 1;

        while (i + cuenta < n &&
               texto[i + cuenta] == simbolo &&
               cuenta < MAX_CORRIDA) {
            cuenta++;
        }

        comprimido += (char)('0' + cuenta);
        comprimido += simbolo;
        i += cuenta;
    }

    return comprimido;
}

string rleDescomprimir(const string& comprimido)
{
    if (comprimido.empty())
        throw "La cadena comprimida esta vacia";

    int n = (int)comprimido.size();
    if (n % 2 != 0)
        throw RLE_ERR_TRUNCADO;

    string texto;

    for (int i = 0; i < n; i += 2) {
        char c = comprimido[i];

        if (c < '1' || c > '9')
            throw RLE_ERR_CONTEO;

        int cuenta = c - '0';
        for (int j = 0; j < cuenta; j++)
            texto += comprimido[i + 1];
    }

    return texto;
}

void ejecutarRLE()
{
    cout << "=== 5.1 Compresion y descompresion RLE ===" << endl;
    cout << "Ingrese el texto: ";

    string original;
    getline(cin, original);

    try {
        string comprimido = rleComprimir(original);
        cout << "Texto original:  " << original << endl;
        cout << "Texto comprimido: " << comprimido << endl;
        cout << "Longitud: " << original.size() << " -> " << comprimido.size() << " caracteres" << endl;

        string recuperado = rleDescomprimir(comprimido);
        cout << "Texto descomprimido: " << recuperado << endl;

        if (recuperado == original)
            cout << "Verificacion: OK, la descompresion coincide con el texto inicial." << endl;
        else
            cout << "Verificacion: ERROR, el texto recuperado es distinto del original." << endl;
    }
    catch (const char* msg) {
        cerr << "Error: " << msg << endl;
    }
    catch (int codigo) {
        cerr << "Error numero " << codigo << ": ";
        if (codigo == RLE_ERR_TRUNCADO) cerr << "cadena comprimida incompleta" << endl;
        else if (codigo == RLE_ERR_CONTEO) cerr << "cuenta invalida en la cadena comprimida" << endl;
        else cerr << "error desconocido" << endl;
    }
    catch (...) {
        cerr << "Error inesperado" << endl;
    }
}
