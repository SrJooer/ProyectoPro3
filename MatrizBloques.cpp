#include "MatrizBloques.h"
#include "Constantes.h"
#include "Pelota.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QDebug>

#include <algorithm>
#include <cmath>

int MatrizBloques::items = 0;

MatrizBloques::MatrizBloques(QGraphicsScene* esc)
    : escena(esc), matriz(nullptr), filas(0), columnas(0), activos(0)
{
}

MatrizBloques::~MatrizBloques()
{
    liberar();
}

void MatrizBloques::liberar()
{
    if (!matriz) return;

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            Bloque& b = matriz[i][j];
            if (b.item) {
                escena->removeItem(b.item);
                delete b.item;
                b.item = nullptr;
                --items;
            }
        }
        delete[] matriz[i];
        matriz[i] = nullptr;
    }
    delete[] matriz;
    matriz = nullptr;
    filas = columnas = activos = 0;
}

void MatrizBloques::construir()
{
    liberar();

    filas    = Const::FILAS;
    columnas = Const::COLUMNAS;

    matriz = new Bloque*[filas];
    for (int i = 0; i < filas; ++i)
        matriz[i] = new Bloque[columnas];

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            Bloque& b = matriz[i][j];
            b.activo = true;
            b.item = new QGraphicsRectItem(rectCelda(i, j));
            b.item->setBrush(QBrush(Qt::white));
            b.item->setPen(Qt::NoPen);
            escena->addItem(b.item);
            ++items;
            ++activos;
        }
    }
    qDebug() << "Matriz creada:" << filas << "x" << columnas << "=" << activos << "bloques";
}

void MatrizBloques::destruirBloque(int i, int j)
{
    Bloque& b = matriz[i][j];
    if (b.item) {
        escena->removeItem(b.item);
        delete b.item;
        b.item = nullptr;
        --items;
    }
    b.activo = false;
    --activos;
    qDebug() << "matriz[" << i << "][" << j << "] liberado | quedan" << activos;
}

QRectF MatrizBloques::rectCelda(int i, int j) const
{
    return QRectF(Const::GRILLA_X + j * (Const::BLOQUE_ANCHO + Const::BLOQUE_SEP),
                  Const::GRILLA_Y + i * (Const::BLOQUE_ALTO  + Const::BLOQUE_SEP),
                  Const::BLOQUE_ANCHO, Const::BLOQUE_ALTO);
}

bool MatrizBloques::procesarColision(Pelota& p)
{
    if (!matriz) return false;

    const double paso_x = Const::BLOQUE_ANCHO + Const::BLOQUE_SEP;
    const double paso_y = Const::BLOQUE_ALTO  + Const::BLOQUE_SEP;
    int c0 = (int)std::floor((p.x() - p.radio() - Const::GRILLA_X) / paso_x);
    int c1 = (int)std::floor((p.x() + p.radio() - Const::GRILLA_X) / paso_x);
    int f0 = (int)std::floor((p.y() - p.radio() - Const::GRILLA_Y) / paso_y);
    int f1 = (int)std::floor((p.y() + p.radio() - Const::GRILLA_Y) / paso_y);
    if (c1 < 0 || c0 >= columnas || f1 < 0 || f0 >= filas) return false;
    c0 = std::max(c0, 0);  c1 = std::min(c1, columnas - 1);
    f0 = std::max(f0, 0);  f1 = std::min(f1, filas - 1);

    for (int i = f0; i <= f1; ++i) {
        for (int j = c0; j <= c1; ++j) {
            if (!matriz[i][j].activo) continue;
            if (p.colisionarRect(rectCelda(i, j))) {
                destruirBloque(i, j);
                return true;
            }
        }
    }
    return false;
}
