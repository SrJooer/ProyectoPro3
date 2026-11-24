#ifndef PELOTA_H
#define PELOTA_H

#include <QRectF>

class QGraphicsScene;
class QGraphicsEllipseItem;
class QGraphicsLineItem;

class Pelota
{
public:
    Pelota(QGraphicsScene* escena, double x, double y);
    ~Pelota();

    Pelota(const Pelota&) = delete;
    Pelota& operator=(const Pelota&) = delete;

    void lanzar(double anguloRad);
    void ponerEn(double x, double y);
    void mover(double dt);
    void mostrarVector(bool visible);

    void rebotarParedes(const QRectF& area);
    bool rebotarPaleta(const QRectF& paleta);
    bool colisionarRect(const QRectF& r);

    double x() const { return px; }
    double y() const { return py; }
    double radio() const { return rad; }
    bool   estaViva() const { return viva; }

    static int vivas() { return contador; }

private:
    void reflejar(double nx, double ny);
    void normalizar();
    void evitarTrayectoriaHorizontal();
    void sincronizar();

    double px, py;
    double vx, vy;
    double rad;
    bool   viva;
    QGraphicsScene*       escena;
    QGraphicsEllipseItem* item;
    QGraphicsLineItem*    lineaVector;

    static int contador;
};

#endif
