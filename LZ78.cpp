#include "lz78.h"
#include <iostream>

using namespace std;

const int TAM_MAX_TEXTO = 1024;
const int CAPACIDAD_INICIAL = 16;

struct EntradaDic {
    int prefijo;
    unsigned char caracter;
};

struct Diccionario {
    EntradaDic* entradas;
    int total;
    int capacidad;
};

static void crearDiccionario(Diccionario& d)
{
    d.capacidad = CAPACIDAD_INICIAL;
    d.entradas = new EntradaDic[d.capacidad];
    d.entradas[0].prefijo = 0;
    d.entradas[0].caracter = 0;
    d.total = 1;
}

static void liberarDiccionario(Diccionario& d)
{
    delete[] d.entradas;
    d.entradas = nullptr;
    d.total = 0;
    d.capacidad = 0;
}

static int agregarEntrada(Diccionario& d, int prefijo, unsigned char car)
{
    if (d.total == d.capacidad) {
        int nuevaCap = d.capacidad * 2;
        EntradaDic* nuevo = new EntradaDic[nuevaCap];
        for (int i = 0; i < d.total; i++)
            nuevo[i] = d.entradas[i];
        delete[] d.entradas;
        d.entradas = nuevo;
        d.capacidad = nuevaCap;
    }
    d.entradas[d.total].prefijo = prefijo;
    d.entradas[d.total].caracter = car;
    return d.total++;
}

static int buscarEntrada(const Diccionario& d, int prefijo, unsigned char car)
{
    for (int i = 1; i < d.total; i++) {
        if (d.entradas[i].prefijo == prefijo && d.entradas[i].caracter == car)
            return i;
    }
    return 0;
}

static int anchoIndice(int maxIndice)
{
    if (maxIndice < 256) return 1;
    if (maxIndice < 65536) return 2;
    if (maxIndice < 16777216) return 3;
    return 4;
}

static void escribirPar(unsigned char* buf, int& pos, int indice, unsigned char car, int ancho)
{
    for (int b = 0; b < ancho; b++)
        buf[pos++] = (unsigned char)((indice >> (8 * (ancho - 1 - b))) & 0xFF);
    buf[pos++] = car;
}

static void leerPar(const unsigned char* buf, int& pos, int ancho, int& indice, unsigned char& car)
{
    indice = 0;
    for (int b = 0; b < ancho; b++)
        indice = (indice << 8) | buf[pos++];
    car = buf[pos++];
}

static void leerEncabezado(const unsigned char* datos, int lon, int& ancho, bool& pendiente, int& numPares)
{
    if (datos == nullptr || lon == 0)
        throw "No hay datos comprimidos para procesar con LZ78";
    if (lon < 2)
        throw LZ78_ERR_FORMATO;

    ancho = datos[0];
    if (ancho < 1 || ancho > 4)
        throw LZ78_ERR_FORMATO;
    if (datos[1] > 1)
        throw LZ78_ERR_FORMATO;
    pendiente = (datos[1] == 1);

    int cuerpo = lon - 2;
    if (cuerpo % (ancho + 1) != 0)
        throw LZ78_ERR_FORMATO;

    numPares = cuerpo / (ancho + 1);
    if (numPares == 0)
        throw LZ78_ERR_FORMATO;
}

unsigned char* lz78Comprimir(const unsigned char* entrada, int lonEntrada, int& lonSalida)
{
    if (entrada == nullptr || lonEntrada == 0)
        throw "No hay datos para comprimir con LZ78";

    Diccionario dic;
    dic.entradas = nullptr;
    unsigned char* salida = nullptr;

    try {
        crearDiccionario(dic);

        int actual = 0;
        for (int i = 0; i < lonEntrada; i++) {
            int hallado = buscarEntrada(dic, actual, entrada[i]);
            if (hallado != 0) {
                actual = hallado;
            } else {
                agregarEntrada(dic, actual, entrada[i]);
                actual = 0;
            }
        }

        bool pendiente = (actual != 0);
        int entradasReales = dic.total - 1;
        int numPares = entradasReales + (pendiente ? 1 : 0);
        int ancho = anchoIndice(entradasReales);

        lonSalida = 2 + numPares * (ancho + 1);
        salida = new unsigned char[lonSalida];
        salida[0] = (unsigned char)ancho;
        salida[1] = pendiente ? 1 : 0;

        int pos = 2;
        for (int k = 1; k < dic.total; k++)
            escribirPar(salida, pos, dic.entradas[k].prefijo, dic.entradas[k].caracter, ancho);
        if (pendiente)
            escribirPar(salida, pos, actual, 0, ancho);
    }
    catch (...) {
        liberarDiccionario(dic);
        delete[] salida;
        throw;
    }

    liberarDiccionario(dic);
    return salida;
}

static void asegurarCapacidad(unsigned char*& buf, int& capacidad, int usado, int necesario)
{
    if (necesario <= capacidad)
        return;

    int nuevaCap = capacidad;
    while (nuevaCap < necesario)
        nuevaCap *= 2;

    unsigned char* nuevo = new unsigned char[nuevaCap];
    for (int i = 0; i < usado; i++)
        nuevo[i] = buf[i];
    delete[] buf;
    buf = nuevo;
    capacidad = nuevaCap;
}

unsigned char* lz78Descomprimir(const unsigned char* entrada, int lonEntrada, int& lonSalida)
{
    int ancho;
    bool pendiente;
    int numPares;
    leerEncabezado(entrada, lonEntrada, ancho, pendiente, numPares);

    Diccionario dic;
    dic.entradas = nullptr;
    unsigned char* salida = nullptr;
    int capacidad = 64;
    lonSalida = 0;

    try {
        crearDiccionario(dic);
        salida = new unsigned char[capacidad];

        int pos = 2;
        for (int k = 1; k <= numPares; k++) {
            int indice;
            unsigned char car;
            leerPar(entrada, pos, ancho, indice, car);

            bool sinCaracter = (pendiente && k == numPares);

            if (indice >= dic.total)
                throw LZ78_ERR_INDICE;
            if (sinCaracter && indice == 0)
                throw LZ78_ERR_FORMATO;

            int longFrase = sinCaracter ? 0 : 1;
            for (int p = indice; p != 0; p = dic.entradas[p].prefijo)
                longFrase++;

            asegurarCapacidad(salida, capacidad, lonSalida, lonSalida + longFrase);

            int fin = lonSalida + longFrase;
            int escribir = fin;
            if (!sinCaracter)
                salida[--escribir] = car;
            for (int p = indice; p != 0; p = dic.entradas[p].prefijo)
                salida[--escribir] = dic.entradas[p].caracter;
            lonSalida = fin;

            if (!sinCaracter)
                agregarEntrada(dic, indice, car);
        }
    }
    catch (...) {
        liberarDiccionario(dic);
        delete[] salida;
        lonSalida = 0;
        throw;
    }

    liberarDiccionario(dic);
    return salida;
}

static void imprimirCaracter(unsigned char c)
{
    if (c == ' ')
        cout << "[esp]";
    else if (c >= 33 && c <= 126)
        cout << (char)c;
    else
        cout << "[" << (int)c << "]";
}

void lz78MostrarPares(const unsigned char* comprimido, int lon)
{
    int ancho;
    bool pendiente;
    int numPares;
    leerEncabezado(comprimido, lon, ancho, pendiente, numPares);

    int pos = 2;
    for (int k = 1; k <= numPares; k++) {
        int indice;
        unsigned char car;
        leerPar(comprimido, pos, ancho, indice, car);

        cout << "(" << indice << ", ";
        if (pendiente && k == numPares)
            cout << "-";
        else
            imprimirCaracter(car);
        cout << ") ";
    }
    cout << endl;
}

void ejecutarLZ78()
{
    cout << "=== 5.2 Compresion y descompresion LZ78 ===" << endl;
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

    unsigned char* comprimido = nullptr;
    unsigned char* recuperado = nullptr;

    try {
        int lonComp = 0;
        int lonRec = 0;

        comprimido = lz78Comprimir(datos, lonTexto, lonComp);
        cout << "Pares (indice, caracter): ";
        lz78MostrarPares(comprimido, lonComp);
        cout << "Tamano: " << lonTexto << " bytes -> " << lonComp << " bytes" << endl;

        recuperado = lz78Descomprimir(comprimido, lonComp, lonRec);
        cout << "Texto descomprimido: ";
        for (int i = 0; i < lonRec; i++)
            cout << (char)recuperado[i];
        cout << endl;

        bool iguales = (lonRec == lonTexto);
        if (iguales) {
            for (int i = 0; i < lonTexto; i++) {
                if (recuperado[i] != datos[i]) {
                    iguales = false;
                    break;
                }
            }
        }

        if (iguales)
            cout << "Verificacion: OK, la descompresion coincide con el texto inicial." << endl;
        else
            cout << "Verificacion: ERROR, el texto recuperado es distinto del original." << endl;
    }
    catch (const char* msg) {
        cerr << "Error: " << msg << endl;
    }
    catch (int codigo) {
        cerr << "Error numero " << codigo << ": ";
        if (codigo == LZ78_ERR_FORMATO) cerr << "datos comprimidos con formato invalido" << endl;
        else if (codigo == LZ78_ERR_INDICE) cerr << "indice de diccionario inexistente" << endl;
        else cerr << "error desconocido" << endl;
    }
    catch (...) {
        cerr << "Error inesperado" << endl;
    }

    delete[] comprimido;
    delete[] recuperado;
}
