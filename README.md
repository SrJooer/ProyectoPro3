# Brick Breaker (prototipo de física y matriz)

Qt 6 Widgets + C++17. Abrir `CMakeLists.txt` con Qt Creator y elegir el kit **Desktop Qt 6.x MinGW 64-bit**.

Prototipo mínimo para probar solo dos cosas: **física vectorial** de la pelota y **matriz dinámica con puntero doble** (`Bloque**`).
Sin niveles, sin power-ups, sin tiempo, todo blanco sobre negro.

## Importante: carpeta de compilación sin tildes
Esta ruta tiene tildes y espacios (`Programación III`). `moc` (AUTOMOC) falla si la **carpeta de compilación**
tiene tildes, y el generador *Makefiles* también falla con la ruta del código. Solución en Qt Creator:
1. *Projects* (`Ctrl+5`) → *Build* → **Build directory** = `C:\qtbuild\BrickBreaker-Debug` (sin tildes ni espacios).
2. Generador **Ninja** (*Preferences → Kits → kit MinGW → CMake generator*, y en *Build → Initial Configuration* la clave `CMAKE_GENERATOR`).
3. *Build → Clear CMake Configuration*, luego *Build → Run CMake* y `Ctrl+R`.

Alternativa: mover el proyecto a una ruta simple como `C:\Proyectos\BrickBreaker`.

## Controles
| Acción | Tecla |
|---|---|
| Mover paleta | Flechas, A / D o ratón |
| Lanzar pelota | Espacio o clic |
| Mostrar / ocultar la línea del vector velocidad | V |
| Reiniciar | R |

## Qué se puede observar
- **Vector velocidad:** la línea fina que sale de la pelota es `v = (vx, vy)`. Su módulo se mantiene en `RAPIDEZ` siempre.
- **Reflexión:** en paredes y bloques se aplica `r = v - 2(v·n)n`. En la paleta, el ángulo depende del punto de impacto (centro = vertical, extremos = hasta 60°).
- **Matriz con puntero doble:** al romperse un bloque, la consola (*Application Output*) imprime `matriz[i][j] liberado`.
  Al cerrar imprime cuántas pelotas e items de bloque siguen vivos (deben ser 0).

## Clases
| Archivo | Qué hace |
|---|---|
| `Bloque` | Celda: `activo` e item gráfico |
| `MatrizBloques` | `Bloque** matriz` con `new` / `delete[]`; libera bloques individuales; colisión solo con celdas cercanas |
| `Pelota` | `p += v*dt`, reflexión vectorial, colisión círculo-rectángulo, línea del vector |
| `Paleta` | Movimiento por teclado/ratón |
| `Juego` | `QGraphicsView` + `QTimer`, eventos, HUD de vidas y puntos |
| `Constantes.h` | Valores ajustables (rapidez, ángulo máximo, tamaños, filas y columnas) |
