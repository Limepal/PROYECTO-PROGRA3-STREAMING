# Patrones de diseño implementados

## 1. Proxy: `MotorBusquedaProxy`

### Problema en el contexto del proyecto
El sistema de streaming usa un `MotorBusqueda` que consulta dos estructuras principales:

- Suffix Tree para buscar coincidencias por título.
- Índice invertido para buscar coincidencias por trama/sinopsis.

La interfaz puede repetir búsquedas iguales cuando el usuario consulta términos similares, vuelve a páginas anteriores o cuando el sistema de recomendaciones consulta palabras repetidas de películas con like. Recalcular siempre esas búsquedas genera trabajo innecesario.

### Solución aplicada
Se agregó la clase `MotorBusquedaProxy`, que envuelve al `MotorBusqueda` real y controla el acceso a las operaciones de búsqueda. El proxy guarda en caché los resultados de:

- `buscarPorTitulo`
- `buscarEnTrama`

Si una consulta ya fue ejecutada, se retorna directamente el resultado guardado. Si no existe en caché, se consulta al motor real y luego se almacena.

### Justificación
Este patrón es adecuado porque el motor de búsqueda es una parte central y potencialmente costosa del sistema. El Proxy permite agregar caché sin modificar el algoritmo interno del Suffix Tree ni el índice invertido. Además, la interfaz sigue dependiendo de un objeto tipo `MotorBusqueda`, por lo que el cambio queda desacoplado.

### Archivos relacionados

- `MotorBusquedaProxy.h`
- `MotorBusquedaProxy.cpp`
- `MotorBusqueda.h`
- `main.cpp`

---

## 2. Observer: eventos de interacción del usuario

### Problema en el contexto del proyecto
El usuario puede realizar acciones sobre una película, por ejemplo:

- Dar like.
- Quitar like.
- Guardar en Ver más tarde.
- Quitar de Ver más tarde.

Antes, la interfaz debía encargarse directamente de actualizar las listas y también de actualizar las estadísticas de géneros favoritos. Eso hace que `InterfazStreaming` tenga demasiadas responsabilidades.

### Solución aplicada
Se agregó el patrón Observer mediante:

- `IInteraccionObserver`: interfaz base de observadores.
- `ObservadorGenerosFavoritos`: actualiza la frecuencia de géneros según los likes.
- `ObservadorHistorialInteracciones`: registra contadores de likes y ver-más-tarde.
- `EventoInteraccion`: estructura que representa la acción realizada por el usuario.

Ahora `InterfazStreaming` notifica eventos como `LIKE_AGREGADO`, `LIKE_REMOVIDO`, `VMT_AGREGADO` y `VMT_REMOVIDO`, y los observadores reaccionan sin que la interfaz conozca todos los detalles internos.

### Justificación
Observer encaja naturalmente en una aplicación tipo streaming porque las acciones del usuario son eventos. Este patrón permite que nuevas funcionalidades futuras, como notificaciones, historial, métricas o recomendaciones más avanzadas, se agreguen creando nuevos observadores sin reescribir la lógica principal de la interfaz.

### Archivos relacionados

- `InteraccionObserver.h`
- `InteraccionObserver.cpp`
- `InterfazStreaming.h`
- `InterfazStreaming.cpp`

---

## Cambios adicionales menores

- Se ajustó `MotorBusqueda` para que sus métodos de búsqueda sean `virtual`, permitiendo que `MotorBusquedaProxy` pueda sustituirlo polimórficamente.
- Se corrigió la asignación del `id` real de cada película al insertarla en `MotorBusqueda`.
- Se corrigieron dos cadenas mal formadas en la paginación de resultados.
- Se bajó la versión mínima de CMake a `3.20` para mejorar compatibilidad con CLion manteniendo `C++20`.
