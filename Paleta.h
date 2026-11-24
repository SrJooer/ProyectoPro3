#ifndef PALETA_H
#define PALETA_H

#include <QRectF>

class QGraphicsScene;
class QGraphicsRectItem;

class Paleta
{
public:
    explicit Paleta(QGraphicsScene* escena);
    ~Paleta();

    Paleta(const Paleta&) = delete;
    Paleta& operator=(const Paleta&) = delete;

    void mover(double dt);
    void setIzquierda(bool activa) { izq = activa; }
    void setDerecha(bool activa)   { der = activa; }
    void setCentroX(double cx);

    QRectF rect() const;
    double centroX() const { return x + ancho / 2.0; }
    double arriba() const;

private:
    QGraphicsScene*    escena;
    QGraphicsRectItem* item;
    double x;
    double ancho;
    bool   izq, der;
};

#endif
