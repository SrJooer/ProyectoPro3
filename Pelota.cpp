#include "Pelota.h"
#include "Constantes.h"

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QBrush>
#include <QPen>

#include <algorithm>
#include <cmath>

int Pelota::contador = 0;

Pelota::Pelota(QGraphicsScene* esc, double x, double y)
    : px(x), py(y), vx(0), vy(0), rad(Const::PELOTA_RADIO), viva(true),
      escena(esc), item(nullptr), lineaVector(nullptr)
{
    item = new QGraphicsEllipseItem(-rad, -rad, 2 * rad, 2 * rad);
    item->setBrush(QBrush(Qt::white));
    item->setPen(Qt::NoPen);
    item->setZValue(10);
    escena->addItem(item);

    lineaVector = new QGraphicsLineItem();
    lineaVector->setPen(QPen(QColor(255, 255, 255, 110), 1));
    lineaVector->setZValue(9);
    escena->addItem(lineaVector);

    ++contador;
    sincronizar();
}

Pelota::~Pelota()
{
    if (lineaVector) {
        escena->removeItem(lineaVector);
        delete lineaVector;
        lineaVector = nullptr;
    }
    if (item) {
        escena->removeItem(item);
        delete item;
        item = nullptr;
    }
    --contador;
}

void Pelota::lanzar(double anguloRad)
{
    vx =  Const::RAPIDEZ * std::sin(anguloRad);
    vy = -Const::RAPIDEZ * std::cos(anguloRad);
    sincronizar();
}

void Pelota::ponerEn(double x, double y)
{
    px = x;
    py = y;
    sincronizar();
}

void Pelota::mover(double dt)
{
    px += vx * dt;
    py += vy * dt;
    sincronizar();
}

void Pelota::mostrarVector(bool visible)
{
    lineaVector->setVisible(visible);
}

void Pelota::reflejar(double nx, double ny)
{
    const double punto = vx * nx + vy * ny;
    if (punto < 0) {
        vx -= 2.0 * punto * nx;
        vy -= 2.0 * punto * ny;
    }
}

void Pelota::normalizar()
{
    const double m = std::hypot(vx, vy);
    if (m > 1e-9) {
        vx *= Const::RAPIDEZ / m;
        vy *= Const::RAPIDEZ / m;
    }
}

void Pelota::evitarTrayectoriaHorizontal()
{
    const double minVy = 0.30 * Const::RAPIDEZ;
    if (std::fabs(vy) < minVy) {
        const double signoY = (vy < 0 || vy == 0) ? -1.0 : 1.0;
        const double signoX = (vx < 0) ? -1.0 : 1.0;
        vy = signoY * minVy;
        vx = signoX * std::sqrt(Const::RAPIDEZ * Const::RAPIDEZ - vy * vy);
    }
}

void Pelota::rebotarParedes(const QRectF& area)
{
    if (px - rad < area.left()) {
        px = area.left() + rad;
        reflejar(1, 0);
    } else if (px + rad > area.right()) {
        px = area.right() - rad;
        reflejar(-1, 0);
    }
    if (py - rad < area.top()) {
        py = area.top() + rad;
        reflejar(0, 1);
    }
    if (py - rad > area.bottom())
        viva = false;

    normalizar();
    evitarTrayectoriaHorizontal();
    sincronizar();
}

bool Pelota::rebotarPaleta(const QRectF& r)
{
    if (vy <= 0) return false;

    const double cx = std::clamp(px, r.left(), r.right());
    const double cy = std::clamp(py, r.top(),  r.bottom());
    const double dx = px - cx, dy = py - cy;
    if (dx * dx + dy * dy > rad * rad) return false;

    if (py <= r.center().y()) {
        double rel = (px - r.center().x()) / (r.width() / 2.0);
        rel = std::clamp(rel, -1.0, 1.0);
        const double ang = rel * Const::ANGULO_MAX;
        vx =  Const::RAPIDEZ * std::sin(ang);
        vy = -Const::RAPIDEZ * std::cos(ang);
        py = r.top() - rad - 0.01;
    } else {
        const double nx = (px < r.center().x()) ? -1.0 : 1.0;
        px = (nx < 0) ? r.left() - rad : r.right() + rad;
        reflejar(nx, 0);
        normalizar();
    }
    sincronizar();
    return true;
}

bool Pelota::colisionarRect(const QRectF& r)
{
    const double cx = std::clamp(px, r.left(), r.right());
    const double cy = std::clamp(py, r.top(),  r.bottom());
    const double dx = px - cx, dy = py - cy;
    const double d2 = dx * dx + dy * dy;
    if (d2 > rad * rad) return false;

    double nx, ny;
    if (d2 > 1e-9) {
        const double d = std::sqrt(d2);
        nx = dx / d;
        ny = dy / d;
        px = cx + nx * (rad + 0.01);
        py = cy + ny * (rad + 0.01);
    } else {
        const double izq = px - r.left(),  der = r.right()  - px;
        const double arr = py - r.top(),   aba = r.bottom() - py;
        const double m = std::min(std::min(izq, der), std::min(arr, aba));
        if      (m == izq) { nx = -1; ny =  0; px = r.left()   - rad; }
        else if (m == der) { nx =  1; ny =  0; px = r.right()  + rad; }
        else if (m == arr) { nx =  0; ny = -1; py = r.top()    - rad; }
        else               { nx =  0; ny =  1; py = r.bottom() + rad; }
    }

    const bool iba_contra = (vx * nx + vy * ny) < 0;
    reflejar(nx, ny);
    normalizar();
    evitarTrayectoriaHorizontal();
    sincronizar();
    return iba_contra;
}

void Pelota::sincronizar()
{
    if (item) item->setPos(px, py);
    if (lineaVector)
        lineaVector->setLine(px, py, px + vx * Const::ESCALA_VECTOR, py + vy * Const::ESCALA_VECTOR);
}
