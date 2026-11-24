#ifndef JUEGO_H
#define JUEGO_H

#include <QGraphicsView>
#include <QElapsedTimer>
#include <QString>

class QTimer;
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;
class MatrizBloques;
class Paleta;
class Pelota;

class Juego : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Juego(QWidget* parent = nullptr);
    ~Juego() override;

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;

private slots:
    void actualizar();

private:
    enum Estado { ESPERANDO, JUGANDO, FIN };

    void crearEscenario();
    void reiniciar();
    void nuevaPelotaPegada();
    void lanzar();
    void simularPelota(double dt);
    void terminar(const QString& titulo);
    void actualizarHUD();

    QGraphicsScene* escena = nullptr;
    MatrizBloques*  matriz = nullptr;
    Paleta*         paleta = nullptr;
    Pelota*         pelota = nullptr;
    QTimer*         timer  = nullptr;
    QElapsedTimer   reloj;

    Estado estado = ESPERANDO;
    int    vidas = 0;
    int    puntos = 0;
    bool   verVector = true;

    QGraphicsSimpleTextItem* txtVidas  = nullptr;
    QGraphicsSimpleTextItem* txtPuntos = nullptr;
    QGraphicsRectItem*       velo      = nullptr;
    QGraphicsSimpleTextItem* mensaje   = nullptr;
};

#endif
