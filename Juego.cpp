#include "Juego.h"
#include "Constantes.h"
#include "MatrizBloques.h"
#include "Paleta.h"
#include "Pelota.h"

#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QRandomGenerator>
#include <QDebug>
#include <QFont>
#include <QPen>
#include <QBrush>

#include <algorithm>

Juego::Juego(QWidget* parent)
    : QGraphicsView(parent)
{
    escena = new QGraphicsScene(0, 0, Const::ANCHO, Const::ALTO);
    setScene(escena);
    setFixedSize(Const::ANCHO, Const::ALTO);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setWindowTitle("Brick Breaker - prototipo de fisica");

    crearEscenario();
    matriz = new MatrizBloques(escena);
    paleta = new Paleta(escena);
    reiniciar();

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, &Juego::actualizar);
    reloj.start();
    timer->start(16);
}

Juego::~Juego()
{
    timer->stop();
    delete pelota;  pelota = nullptr;
    delete matriz;  matriz = nullptr;
    delete paleta;  paleta = nullptr;
    qDebug() << "Pelotas vivas:" << Pelota::vivas()
             << "| Items de bloque vivos:" << MatrizBloques::itemsVivos()
             << "(ambos deben ser 0)";
    setScene(nullptr);
    delete escena;  escena = nullptr;
}

void Juego::crearEscenario()
{
    escena->setBackgroundBrush(QBrush(Qt::black));

    const QPen pared(Qt::white, 1);
    escena->addLine(0, Const::ZONA_HUD, Const::ANCHO, Const::ZONA_HUD, pared);
    escena->addLine(0.5, Const::ZONA_HUD, 0.5, Const::ALTO, pared);
    escena->addLine(Const::ANCHO - 0.5, Const::ZONA_HUD, Const::ANCHO - 0.5, Const::ALTO, pared);

    const QFont fuente("Consolas", 12);
    txtVidas = escena->addSimpleText("", fuente);
    txtVidas->setBrush(QBrush(Qt::white));
    txtVidas->setPos(16, 7);
    txtPuntos = escena->addSimpleText("", fuente);
    txtPuntos->setBrush(QBrush(Qt::white));

    velo = escena->addRect(0, Const::ZONA_HUD, Const::ANCHO, Const::ALTO - Const::ZONA_HUD,
                           Qt::NoPen, QBrush(QColor(0, 0, 0, 210)));
    velo->setZValue(100);
    velo->hide();
    mensaje = escena->addSimpleText("", QFont("Consolas", 22));
    mensaje->setBrush(QBrush(Qt::white));
    mensaje->setZValue(101);
    mensaje->hide();
}

void Juego::actualizarHUD()
{
    txtVidas->setText(QString("VIDAS %1").arg(vidas));
    txtPuntos->setText(QString("PUNTOS %1").arg(puntos));
    txtPuntos->setPos(Const::ANCHO - 16 - txtPuntos->boundingRect().width(), 7);
}

void Juego::reiniciar()
{
    vidas = Const::VIDAS;
    puntos = 0;
    matriz->construir();
    paleta->setCentroX(Const::ANCHO / 2.0);
    velo->hide();
    mensaje->hide();
    nuevaPelotaPegada();
    actualizarHUD();
}

void Juego::nuevaPelotaPegada()
{
    delete pelota;
    pelota = new Pelota(escena, paleta->centroX(), paleta->arriba() - Const::PELOTA_RADIO - 1);
    pelota->mostrarVector(verVector);
    estado = ESPERANDO;
    reloj.restart();
}

void Juego::lanzar()
{
    if (estado != ESPERANDO || !pelota) return;
    const double angulo = (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * 0.35;
    pelota->lanzar(angulo);
    estado = JUGANDO;
    reloj.restart();
}

void Juego::terminar(const QString& titulo)
{
    estado = FIN;
    mensaje->setText(titulo + "\nPUNTOS " + QString::number(puntos) + "\n\nR = reiniciar");
    mensaje->setPos((Const::ANCHO - mensaje->boundingRect().width()) / 2, 230);
    velo->show();
    mensaje->show();
}

void Juego::actualizar()
{
    double dt = reloj.restart() / 1000.0;
    dt = std::min(dt, 0.033);

    if (estado != FIN)
        paleta->mover(dt);

    if (estado == ESPERANDO) {
        pelota->ponerEn(paleta->centroX(), paleta->arriba() - Const::PELOTA_RADIO - 1);
    } else if (estado == JUGANDO) {
        simularPelota(dt);
        if (!pelota->estaViva()) {
            if (--vidas <= 0) {
                delete pelota;  pelota = nullptr;
                terminar("GAME OVER");
            } else {
                nuevaPelotaPegada();
            }
        } else if (matriz->restantes() == 0) {
            terminar("COMPLETADO");
        }
    }
    actualizarHUD();
}

void Juego::simularPelota(double dt)
{
    const QRectF area(0, Const::ZONA_HUD, Const::ANCHO, Const::ALTO - Const::ZONA_HUD);
    const int subpasos = 2;
    const double h = dt / subpasos;
    for (int s = 0; s < subpasos && pelota->estaViva(); ++s) {
        pelota->mover(h);
        pelota->rebotarParedes(area);
        if (!pelota->estaViva()) break;
        pelota->rebotarPaleta(paleta->rect());
        if (matriz->procesarColision(*pelota))
            puntos += 10;
    }
}

void Juego::keyPressEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat()) return;
    switch (e->key()) {
    case Qt::Key_Left:  case Qt::Key_A: paleta->setIzquierda(true); break;
    case Qt::Key_Right: case Qt::Key_D: paleta->setDerecha(true);   break;
    case Qt::Key_Space:                 lanzar();                   break;
    case Qt::Key_R:                     reiniciar();                break;
    case Qt::Key_V:
        verVector = !verVector;
        if (pelota) pelota->mostrarVector(verVector);
        break;
    default: QGraphicsView::keyPressEvent(e);
    }
}

void Juego::keyReleaseEvent(QKeyEvent* e)
{
    if (e->isAutoRepeat()) return;
    switch (e->key()) {
    case Qt::Key_Left:  case Qt::Key_A: paleta->setIzquierda(false); break;
    case Qt::Key_Right: case Qt::Key_D: paleta->setDerecha(false);   break;
    default: QGraphicsView::keyReleaseEvent(e);
    }
}

void Juego::mouseMoveEvent(QMouseEvent* e)
{
    if (estado == FIN) return;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPoint pos = e->position().toPoint();
#else
    const QPoint pos = e->pos();
#endif
    paleta->setCentroX(mapToScene(pos).x());
}

void Juego::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) lanzar();
}
