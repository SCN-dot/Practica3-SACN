#include <iostream>

#include "rle.h"
#include "lz78.h"
#include "Encriptado.h"

using namespace std;

int main()
{
    int opcion = 0;

    cout << "1. RLE " << endl;
    cout << "2. LZ78 " << endl;
    cout << "3. Encriptacion " << endl;
    cout << "Ingrese un numero: ";
    cin >> opcion;

    cin.clear();
    cin.ignore(100000, '\n');

    switch (opcion) {
    case 1:
        ejecutarRLE();
        break;
    case 2:
        ejecutarLZ78();
        break;
    case 3:
        ejecutarCrypto();
        break;
    default:
        cout << "Opcion no valida." << endl;
    }

    return 0;
}
