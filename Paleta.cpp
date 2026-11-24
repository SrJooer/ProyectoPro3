#include "Paleta.h"
#include "Constantes.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

#include <algorithm>

Paleta::Paleta(QGraphicsScene* esc)
    : escena(esc), item(nullptr), x(0), ancho(Const::PALETA_ANCHO), izq(false), der(false)
{
    item = new QGraphicsRectItem(0, 0, ancho, Const::PALETA_ALTO);
    item->setBrush(QBrush(Qt::white));
    item->setPen(Qt::NoPen);
    item->setZValue(10);
    escena->addItem(item);
    setCentroX(Const::ANCHO / 2.0);
}

Paleta::~Paleta()
{
    if (item) {
        escena->removeItem(item);
        delete item;
        item = nullptr;
    }
}

void Paleta::mover(double dt)
{
    const double dir = (der ? 1.0 : 0.0) - (izq ? 1.0 : 0.0);
    if (dir != 0.0)
        setCentroX(centroX() + dir * Const::PALETA_VEL * dt);
}

void Paleta::setCentroX(double cx)
{
    x = std::clamp(cx - ancho / 2.0, 0.0, Const::ANCHO - ancho);
    item->setPos(x, Const::PALETA_Y);
}

QRectF Paleta::rect() const
{
    return QRectF(x, Const::PALETA_Y, ancho, Const::PALETA_ALTO);
}

double Paleta::arriba() const
{
    return Const::PALETA_Y;
}
