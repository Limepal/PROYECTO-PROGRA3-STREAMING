# PiliFlix — Plataforma de búsqueda de películas

Proyecto final de Programación III (2026-1), implementado completamente en C++20.

## Integrantes

- Abigail Jaslin Cabanillas Ventocilla
- Alondra Solange Obregon Carhuavilca
- Axel Roberth Portal Ruiz
- Dayron Saiyuk Cueva Loayza
- Gabriel Marcelo Díaz Carrión

> La consigna admite como máximo cuatro integrantes. El equipo debe confirmar con
> el docente cuáles cuatro integrantes serán evaluados antes de la entrega.

## Funcionalidades

- Lectura y limpieza paralela del CSV.
- Suffix Tree comprimido para buscar cualquier subcadena en títulos.
- Índice invertido para palabras y subpalabras en sinopsis.
- Frases con semántica OR: se combinan las coincidencias de cada término.
- Tags mediante `director:`, `reparto:`, `casting:`, `genero:`, `origen:` y `year:`.
- Ranking propio, resultados paginados de cinco en cinco y detalle de la película.
- Like, Ver más tarde y recomendaciones basadas en género y términos compartidos.
- Persistencia automática del perfil en `perfil_usuario.csv`.

## Compilación y ejecución

Requisitos: CMake 3.20 o superior y un compilador con C++20.

```bash
cmake -S . -B build
cmake --build build
```

Coloque `wiki_movie_plots_deduped.csv` en el directorio desde el cual ejecutará
el programa. En CLion, la configuración habitual usa `cmake-build-debug`, por
lo que el CSV puede estar allí.

```bash
cd build
./PiliFlix
```

En Windows/MinGW el ejecutable se llama `PiliFlix.exe`.

## Manual de uso

1. Espere a que concluya el preprocesamiento y la indexación.
2. En el menú, seleccione Buscar películas.
3. Escriba una palabra, frase o fragmento, por ejemplo `bar`, `barco fantasma`.
4. Para buscar tags use, por ejemplo, `director:chaplin` o `genero:drama`.
5. Use `+` y `-` para cambiar de página y un número para abrir el detalle.
6. En el detalle puede alternar Like y Ver más tarde.
7. Al volver a iniciar, el perfil se restaura y la portada muestra pendientes
   y recomendaciones.

Las entradas inválidas no terminan el programa. Un EOF cierra el menú
limpiamente. Si no se encuentra el CSV, el programa informa la ruta esperada.

## Arquitectura y patrones de diseño

| Patrón | Implementación | Responsabilidad |
|---|---|---|
| State | `EstadoInterfaz` y estados concretos | Modela cada pantalla y sus transiciones |
| Strategy | `EstrategiaRelevancia` | Encapsula el algoritmo intercambiable de ranking |
| Memento | `PerfilMemento` y `CuidadorPerfil` | Captura y restaura Likes y Ver más tarde |
| Observer | `ObservadorPerfil` | Notifica desacopladamente los cambios del perfil |

Además, `contiene<Contenedor, Valor>` demuestra programación genérica y evita
duplicar búsquedas sobre contenedores.

### Corrección del flujo del menú

Las transiciones State son diferidas: el estado activo termina `ejecutar()`
antes de ser destruido. Esto evita el comportamiento indefinido que hacía que
el menú apareciera de forma intermitente.

## Estructuras y algoritmos

### Suffix Tree comprimido

Cada título normalizado genera todos sus sufijos. Las aristas guardan
subcadenas comprimidas y los nodos conservan IDs de películas. Esto permite
buscar prefijos, palabras y fragmentos internos.

- Construcción: O(n²) por título en el peor caso.
- Consulta: O(m + r), donde `m` es la consulta y `r` los resultados.
- Memoria: proporcional al número de nodos y referencias a películas.

Se eligió frente a un Trie convencional porque este último solo resuelve
prefijos. La compresión reduce nodos respecto de un Suffix Trie carácter por
carácter.

### Índice invertido

La sinopsis se tokeniza y almacena como pares `(palabra, id)`. Tras ordenar con
`std::sort`, se agrupan IDs y se usa `std::lower_bound` para coincidencias
exactas. Para subpalabras se recorre el vocabulario único, no todas las
películas, y se unen IDs con `std::sort` + `std::unique`.

### Ranking

La estrategia ponderada asigna:

- 3.0 por coincidencia en título;
- 2.0 por sinopsis;
- 2.5 por tag;
- 1.0 si un término inicia el título;
- bonificaciones pequeñas por año, lista pendiente y géneros preferidos.

### Recomendaciones

Para cada Like se buscan películas que comparten términos significativos de la
sinopsis. Se suma 0.3 por término compartido y 1.0 si coincide el género. Los
Likes existentes quedan excluidos.

## Programación paralela

Se utiliza `std::async`, `std::future` y un máximo de ocho trabajadores:

- limpieza y carga del CSV por bloques;
- un Suffix Tree independiente por trabajador, sin mutex global;
- tokenización de sinopsis en buffers locales sin contención;
- IDs estables según la posición original del CSV;
- ordenamiento simultáneo de cada buffer local;
- fusión k-way de buffers mediante una cola de prioridad.

Las búsquedas consultan todos los árboles y deduplican los IDs. Así los
trabajadores no esperan para insertar en un único árbol compartido.

### Comparación de tiempos

La medición cubre el procesamiento completo de las películas: limpieza del
CSV, carga de datos, construcción de los Suffix Trees y construcción del índice
invertido. Ambas versiones se ejecutaron con el mismo archivo y equipo.

| Versión | Rama | Tiempo (ms) | Tiempo (s) |
|---|---|---:|---:|
| Con programación concurrente | `main` | 28 349.925 | 28.350 |
| Sin programación concurrente | `MainSinConcurrencia` | 49 371.418 | 49.371 |

La versión concurrente obtuvo un speedup aproximado de **1.74x** y redujo el
tiempo total en aproximadamente **42.6%**. La rama
`MainSinConcurrencia` conserva la implementación secuencial utilizada como
línea base de la comparación.

## Organización

- `PreprocesamientoDatos.*`: parsing, limpieza y carga paralela.
- `MotorBusqueda.*`: Suffix Tree, índice invertido y tags.
- `InterfazStreaming.*`: ranking, recomendaciones, perfil y vistas.
- `EstadoInterfaz.h`, `EstadosConcretos.h`: patrón State.
- `EstrategiaRelevancia.h`: patrón Strategy.
- `PerfilMemento.*`: patrón Memento y persistencia del estado.
- `ObservadorPerfil.h`: patrón Observer para eventos del perfil.

## Referencias

Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2022).
*Introduction to algorithms* (4th ed.). MIT Press.

Gamma, E., Helm, R., Johnson, R., & Vlissides, J. (1994). *Design patterns:
Elements of reusable object-oriented software*. Addison-Wesley Professional.

Williams, A. (2019). *C++ concurrency in action: Practical multithreading*
(2nd ed.). Manning Publications.

Kleinberg, J., & Tardos, É. (2006). *Algorithm design*. Pearson.

Gusfield, D. (1997). *Algorithms on Strings, Trees, and Sequences: Computer
Science and Computational Biology*. Cambridge University Press.

Ukkonen, E. (1995). On-line construction of suffix trees. *Algorithmica,
14*(3), 249–260.
