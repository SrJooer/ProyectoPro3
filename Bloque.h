#ifndef BLOQUE_H
#define BLOQUE_H

class QGraphicsRectItem;

class Bloque
{
public:
    Bloque();

    bool               activo;
    QGraphicsRectItem* item;
};

#endif
