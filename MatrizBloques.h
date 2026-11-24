#ifndef MATRIZBLOQUES_H
#define MATRIZBLOQUES_H

#include "Bloque.h"
#include <QRectF>

class QGraphicsScene;
class Pelota;

class MatrizBloques
{
public:
    explicit MatrizBloques(QGraphicsScene* escena);
    ~MatrizBloques();

    MatrizBloques(const MatrizBloques&) = delete;
    MatrizBloques& operator=(const MatrizBloques&) = delete;

    void construir();

    bool procesarColision(Pelota& pelota);

    int restantes() const { return activos; }
    static int itemsVivos() { return items; }

private:
    void   liberar();
    void   destruirBloque(int i, int j);
    QRectF rectCelda(int i, int j) const;

    QGraphicsScene* escena;
    Bloque**        matriz;
    int             filas;
    int             columnas;
    int             activos;

    static int items;
};

#endif
