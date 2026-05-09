# PROYECTO-PROGRA3-STREAMING

### Integrantes:
- Abigail Jaslin Cabanillas Ventocilla
- Alondra Solange Obregon Carhuavilca
- Axel Roberth Portal Ruiz
- Dayron Saiyuk Cueva Loayza
- Gabriel Marcelo Diaz Carrión

### Pseudocódigo Pre Procesamiento de los datos:
#### FASE 1: LIMPIEZA DEL ARCHIVO CSV

    PROCEDIMIENTO LimpiarDatos(nombreEntrada, nombreSalida)
        Abrir archivo nombreEntrada
        Abrir archivo nombreSalida
    
        SI alguno de los archivos no pudo abrirse ENTONCES
            RETORNAR
        FIN SI
    
        Crear string linea
        Crear booleano esCabecera <- VERDADERO
    
        MIENTRAS exista una línea en el archivo HACER
    
            Leer linea
    
            SI esCabecera = VERDADERO ENTONCES
                Escribir cabecera estándar en archivo salida
    
                esCabecera <- FALSO
    
                CONTINUAR
            FIN SI
    
            campos <- SepararLinea(linea)
    
            SI tamaño(campos) >= 8 ENTONCES
    
                year     <- campos[0]
                titulo   <- ProcesarCadena(campos[1])
                origen   <- ProcesarCadena(campos[2])
                director <- ProcesarCadena(campos[3])
                reparto  <- ProcesarCadena(campos[4])
    
                SI campos[5] = "unknown" ENTONCES
                    genero <- "otros"
                SINO
                    genero <- ProcesarCadena(campos[5])
                FIN SI
    
                trama <- ProcesarCadena(campos[7])
                Escribir datos limpios en archivo salida
    
            FIN SI
    
        FIN MIENTRAS
    
        Cerrar archivo entrada
        Cerrar archivo salida
    
    FIN PROCEDIMIENTO

--------------------------------------------------

    FUNCIÓN SepararLinea(linea)
        Crear vector resultado vacío
        Crear string campo <- ""
        Crear booleano dentroComillas <- FALSO
    
        PARA cada caracter c en linea HACER
    
            SI c = '"' ENTONCES
                dentroComillas <- NO dentroComillas
            SINO SI c = ',' Y dentroComillas = FALSO ENTONCES
    
                Agregar campo a resultado
                campo <- ""
    
            SINO
                campo <- campo + c
            FIN SI
    
        FIN PARA
    
        Agregar campo a resultado
        RETORNAR resultado
    
    FIN FUNCIÓN

--------------------------------------------------

    FUNCIÓN ProcesarCadena(cadena)
        Crear string limpia <- ""
        Crear booleano ultimoFueEspacio <- FALSO
    
        PARA cada caracter c en cadena HACER
            c <- ConvertirAMinuscula(c)
    
            SI c es alfanumérico O c = espacio ENTONCES
                SI c = espacio ENTONCES
                    SI ultimoFueEspacio = FALSO ENTONCES
    
                        limpia <- limpia + c
                        ultimoFueEspacio <- VERDADERO
    
                    FIN SI
                SINO
                    limpia <- limpia + c
                    ultimoFueEspacio <- FALSO
                FIN SI
            FIN SI
        FIN PARA
        RETORNAR limpia
    
    FIN FUNCIÓN

--------------------------------------------------

#### FASE 2: CARGA DE DATOS LIMPIOS

    FUNCIÓN CargarYLimpiarDatos(nombreArchivo)
        Crear vector listaDatos vacío
    
        Abrir archivo nombreArchivo
    
        Crear string linea
        Crear booleano esCabecera <- VERDADERO
    
        MIENTRAS exista una línea en el archivo HACER
            Leer linea
    
            SI esCabecera = VERDADERO ENTONCES
                esCabecera <- FALSO
                CONTINUAR
            FIN SI
    
            campos <- SepararLinea(linea)
    
            SI tamaño(campos) >= 8 ENTONCES
                Crear DatosPelicula d
    
                d.year     <- campos[0]
                d.titulo   <- ProcesarCadena(campos[1])
                d.origen   <- ProcesarCadena(campos[2])
                d.director <- ProcesarCadena(campos[3])
                d.reparto  <- ProcesarCadena(campos[4])
    
                SI campos[5] = "unknown" ENTONCES
                    d.genero <- "otros"
                SINO
                    d.genero <- ProcesarCadena(campos[5])
                FIN SI
    
                d.trama <- ProcesarCadena(campos[7])
                Agregar d a listaDatos
    
            FIN SI
    
        FIN MIENTRAS
    
        Cerrar archivo
        
        RETORNAR listaDatos
    FIN FUNCIÓN
  
### Pseudocódigo Motor Búsqueda:

    -Crear constructor con su nodo de árbol
    -Crear función normalizarToken con parámetro constante y referencia string  
    	Crear variable vacía de string (resultado)
    	Para cada carácter c en palabra
            	Si c es alfanumérico Entonces
                	resultado ← resultado + convertirAMinuscula(c)
            	Fin
        	Fin-retornar resultado
    -Con esta función construimos un camino para el Trie letra por letra. 
    
    Es decir, recorremos letra por letra un título, si ese carácter existe 
    en un nodo hijo entonces se avanza al siguiente carácter. En caso no exista, 
    se crea ese nodo hijo con ese carácter y se inserta en la posición correcta.
    
    Procedimiento insertarEnTrie(titulo: Cadena, id: Entero)
        Definir actual como NodoTrie
        actual = raizTrie
    
        Para cada carácter c en titulo hacemos:
            n = convertirAMinuscula(c)
    
            // Buscar posición donde debería estar n (manteniendo el orden)
            posicion = buscarPosicionOrdenada(actual.hijos, n)
    
            Si posicion existe y posicion.caracter = n, entonces
                actual = posicion.nodo
            Sino
                nuevo = crear NodoTrie
                insertarEnPosicion(actual.hijos, posicion, (n, nuevo))
                actual = nuevo
            
        Fin del Bucle For
    
        actual.esFin ← Verdadero
        agregar(actual.peliculasIDs, id)
    Fin de la función
    
    -Guardamos en un buffer (bufferIndexacion) para procesarlo después. 
    
    Si se encuentra un espacio o final de texto entonces terminó la palabra. 
    Guardamos palabras con mayores caracteres de 2 y no vacías y le asignamos un id
    
    void indexarTrama(trama: Cadena, id: Entero)
        Definir palabra como cadena
        palabra = ""
    
        Para i desde 0 hasta longitud(trama) hacemos:
            Si i = longitud(trama) O trama[i] es espacio, entonces:
    
                Si palabra no está vacía, entonces
                    limpia = normalizarToken(palabra)
    
                    Si longitud(limpia) > 2 Entonces
                      agregar(bufferIndexacion, (limpia, id)) //guardamos el par
    		 // con su id
                    Fin de este if
                    palabra = ""
    
                Fin de este if
    
            Sino
                palabra = palabra + trama[i]
            Fin de este if
        Fin del for
    Fin
    
    Ejemplo:
    Si insertamos: "Pepe el pollo"
    La salida en buffer sería:
    ("pepe", id)
    ("pollo", id)
    
    -Obtenemos un índice invertido ordenado y sin duplicados
    
    
    void finalizarIndexacion()
        
        Si bufferIndexacion está vacío (caso trivial) entonces:
            Retornar
        Fin
    
        // Ordenar el buffer
        ordenar(bufferIndexacion)
    
        // Construir índice invertido
        limpiar(indiceInvertido)
    
        Para cada par en bufferIndexacion hacer:
            
            Si indiceInvertido está vacío 
               O ultimaPalabra(indiceInvertido) != par.palabra, entonces:
    
                agregar(indiceInvertido, (par.palabra, [par.id]))
    
            Sino [en caso exista, creamos y asignamos id]
                // es decir la misma palabra
                Si ultimoID(indiceInvertido) ≠ par.id, entonces
                    agregarID(indiceInvertido, par.id)
                Fin
            Fin
    
        Fin
    
        // Limpiar buffer
        limpiar(bufferIndexacion)
        reducirCapacidad(bufferIndexacion)
    
    Fin
    
    Ejemplo visual de la función:
    Buffer inicial (desordenado)
    ("mundo", 2)
    ("hola", 1)
    ("mundo", 1)
    ("hola", 1)
    ("codigo", 3)
    Después de ordenar aplicando la función
    ("codigo", 3)
    ("hola", 1)
    ("hola", 1)
    ("mundo", 1)
    ("mundo", 2)
    Resultado: índice invertido
    codigo = [3]
    hola   = [1]
    mundo  = [1, 2]
    
    -Guardamos la película y la dejamos lista para ser buscada por título y contenido
    
    void agregarPelicula(p: Pelicula)
    
        Si baseDatos está vacía, entonces:
            reservar(baseDatos, 35000)
        Fin
    
        nuevoId = tamaño(baseDatos) // tamaño actual del vector
    
        agregar(baseDatos, p)
    
        insertarEnTrie(p.titulo, nuevoId)
    
        indexarTrama(p.trama, nuevoId)
    
    Fin
    Es decir, la palabra del titulo que tiene un ID x, se crea en el buffer de indexación 
    las palabras de la trama con el id del titulo.
    
    -Recorremos el subárbol del Trie para recolectar los Id de las películas.
    void recolectarIDs(nodo: NodoTrie, resultados: Lista de Enteros)
    
        // Agregar IDs del nodo actual
        Para cada id en nodo.peliculasIDs hacemos:
            agregar(resultados, id)
        Fin
    
        // Recorrer hijos recursivamente
        Para cada hijo en nodo.hijos Hacer
            recolectarIDs(hijo.nodo, resultados)
        Fin
    
    Fin
    Ojo: como siempre se asgina id para palabras mayores a 2, 
    entonces una palabra con 3 carácter recién tendría un id. 
    Ejemplo con esta función:
    m
    └── a
        ├── t → [ID 1]   ("mat")
        │   └── r → [ID 2] ("matr")
        └── p → [ID 3]   ("map")
    
    recolectarIDs(nodo = "ma", resultados)
    Paso a paso:
    	Nodo "ma"
    	no tiene IDs porque solo tiene 2 caracteres
    	Baja a "mat"
    	agrega: [1]
    	Baja a "matr"
    	agrega: [2]
    	Baja a "map"
    	agrega: [3]
    Resultado final:
    peliculas con [1, 2, 3] de id
    
    -Retorna una lista con los id de las película respecto al prefijo insertado.
    vector<int> buscarPorTitulo(prefijo: Cadena) : Lista de Enteros
    
        Definir actual Como NodoTrie
        Definir resultados Como Lista de Enteros
    
        actual = raizTrie
    
        // 1. Navegar en el Trie
        Para cada caracter c en prefijo hacer:
            n = convertirAMinuscula(c)
    
            posicion = buscarPosicionOrdenada(actual.hijos, n)
    
            Si posicion existe Y posicion.caracter = n Entonces
                actual = posicion.nodo
            Sino
                Retornar listaVacía
            Fin
        Fin
    
        // 2. Recolectar IDs
        recolectarIDs(actual, resultados)
    
        // 3. Eliminar duplicados
        ordenar(resultados)
        eliminarDuplicados(resultados)
    
        Retornar resultados
    
    Fin
    
    -Con esto hacemos una búsqueda exacta de una palabra dentro del índice invertido.
    vector<int>buscarEnTrama (termino: Cadena)
    
        busqueda = normalizarToken(termino)
    
        posicion = busquedaBinaria(indiceInvertido, busqueda)
    
        Si posicion existe Y posicion.palabra = busqueda Entonces
            Retornar posicion.ids
        Sino
            Retornar listaVacía
        Fin
    
    Fin
    
    Ejemplo:
    Índice invertido
    "codigo" -> [3]
    "hola"   -> [1, 4]
    "mundo"  -> [1, 2]
    Búsqueda
    termino = "Mundo!!"
    Paso 1: Normalización
    busqueda = "mundo"
    Paso 2: Búsqueda binaria
    Se encuentra: "mundo" -> [1, 2]
    Resultado final: [1, 2]
    
    - Acceso directo a la base de datos usando id
    Pelicula obtenerPelicula (id: Entero)
    
        Si id >= 0 Y id < tamaño(baseDatos) Entonces
            retornar baseDatos[id]
        Sino
    	// en caso de que incumpla la restricción anterior solo retornamos:
            retornar Pelicula(-1, "", "", "", "") 
        Fin
    
    Fin
    Es decir, siempre que sea válido la salida será: Pelicula(-1, "", "", "", "") 

### Pseudocódigo Interface: 

    Inicio del programa e interfaz principal
    
    Al iniciar el programa se van a mostrar las películas que el usuario seleccionó para ver más tarde y
    las películas a las que el usuario le dio like.
    
    Crear vector listaVerMasTarde donde van a estar las películas que se marcaron para "Ver más tarde" por el usuario.
    Crear una función guardar Ver Más Tarde que se va a encargar de almacenar las películas que el usuario marcó para ver más tarde
    
    void guardarVerMastarde(película)
    		se guarda en el vector listaVerMastarde
        
    Crear una función mostrarTitulo(película) que va a mostrar solo los títulos de las primeras 5 peliculas 
    que se almacenan en el vector listaVerMasTarde.
    
    ----"Ver más tarde"----
    
    void mostrarTitulo(pelicula)
    		mostrar solo el titulo de la pelicula
        
    Crear vector listaPeliculasConLike donde van a estar las peliculas a las que el usuario le dio like.
    Crear función guardarPeliculasConLike que se va a encargar de introducir en el vector listaPeliculasConLike 
    aquellas peliculas que el usuario les de like
    
    void guardarPeliculasConLike(película)
    		introducir la película en el vector listaPeliculasConLike
        
    Usamos la misma función mostrarTitulo e imprimimos las primeras 5.
    ----"Le diste like"----
    se muestran las 5 últimas películas a las que el usuario le dio like
    
    Interfaz de búsqueda de películas
    
    Cuando el usuario seleccione la opción de buscar película:
    
    se solicita ingresar una palabra, frase o sub-palabra
    se guarda la búsqueda ingresada en una variable consulta
    La consulta ingresada se envía al motor de búsqueda.
    
    El motor de búsqueda:
    busca coincidencias en los títulos usando el Trie
    busca coincidencias en las tramas usando el índice invertido
    
    Los resultados obtenidos:
    se unen en un solo vector resultadosBusqueda
    se ordenan según relevancia
    
    Crear vector resultadosBusqueda donde se almacenarán las películas encontradas.
    Crear función buscarPelicula(consulta)
    
    void buscarPelicula(consulta)
    buscar coincidencias por título
    buscar coincidencias por trama
    unir resultados encontrados
    ordenar resultados por relevancia
    
    La relevancia de las películas se calcula considerando:
    coincidencias en el título
    likes recibidos por la película
    
    Usamos la funcion mostrarTitulo(pelicula) para mostrar el titulo de las películas más relevantes
    ---- “Peliculas que coinciden con tu búsqueda” ----
       recorrer las primeras 5 películas del vector
           mostrarTitulo(pelicula)
           
    También habrá una opción para ver las siguientes cinco mejor coincidencias de películas, 
    las cuales se extraen también del vector resultadosBusqueda
    
    ---- “Más coincidencias relevantes” ----
       recorrer las siguientes 5 películas
           mostrar título de la película
           
    Luego de mostrar los resultados el usuario puede seleccionar una película
    
    Crear función seleccionarPelicula()
    void seleccionarPelicula()
       solicitar índice o número de película
       obtener película seleccionada desde resultadosBusqueda
       abrir interfaz de visualización de película
       
    Si no existen coincidencias cuando ingresa el string se va a mostrar:
       "No se encontraron películas relacionadas con la búsqueda"
       
    Interfaz de visualización de película
    
    Cuando el usuario seleccione una película desde los resultados de búsqueda:
    
    se obtiene el id de la película seleccionada
    se accede a la base de datos usando el id
    se recuperan los datos completos de la película
    
    Crear función mostrarPelicula(pelicula)
    —”Película seleccionada”---
    
    void mostrarPelicula(pelicula)
    	mostrar título de la película
      mostrar año de la película
      mostrar género de la película
      mostrar sinopsis de la película
      
    Luego de mostrar la información de la película se muestran las opciones de interacción del usuario con la película.
    mostrar:
        1. Dar Like
        2. Guardar en Ver más tarde
    
    Si el usuario selecciona la opción Like:
    
    la película se agrega al vector listaPeliculasConLike
    Usamos la función guardarPeliculasConLike(pelicula)
    
    void guardarPeliculasConLike(pelicula)
        introducir película en listaPeliculasConLike
        
    Si el usuario selecciona la opción Ver más tarde:
    
      la película se almacena en el vector listaVerMasTarde
      Usamos la función guardarVerMasTarde(pelicula)
    
    void guardarVerMasTarde(pelicula)
        guardar película en listaVerMasTarde

## Explicación de la estructura de datos elegida, inserción y búsqueda

### Suffix Trie

La estructura de datos elegida es Suffix Trie porque es la que permite cumplir con los requisitos del Proyecto. Un Suffix Trie es una variante de un Trie que almacena todos los sufijos de una palabra o texto. A diferencia del Trie tradicional, este nos permite buscar por subcadenas, no solo prefijos. 

El proyecto requiere que busquemos películas mediante palabras completas (como ‘barco’),  frases (como ‘barco fantasma’), fragmentos de palabras (como ‘bar’) y por un tag. Por ello, es mejor emplear Suffix Trie, pues este almacena todos los sufijos posibles, permitiendo encontrar subcadenas (como ‘asma’ en ‘fantasma’)

### Funcionamiento de Suffix Trie

La estructura está formada por nodos conectados entre sí, donde cada nodo representa un carácter. Cada camino dentro del árbol representa una secuencia de caracteres perteneciente a algún sufijo insertado. Además, cada nodo almacena los identificadores de las películas que contienen dicha secuencia.

La estructura de un nodo es la siguiente: 

    struct TrieNode {
        unordered_map<char, TrieNode*> children;
        unordered_set<int> movieIDs;
        bool isEndOfWord;
    };

children guarda los hijos del nodo actual. La clave es un carácter y el valor es un puntero al siguiente nodo. 
movie IDs contiene los IDs de las películas que incluyen esa secuencia de caracteres. Esto permite recuperar rápidamente todas las películas relacionadas con una búsqueda.
Finalmente, como el nombre lo dice, isEndOfWord indica si el nodo representa el final de una palabra completa. 

### Algoritmo de inserción
La inserción en un Suffix Trie consiste en almacenar todos los sufijos de cada palabra del texto.
Por ejemplo, insertar la palabra ‘barco’ sería de la siguiente manera:

    barco
    arco
    rco
    co
    o
    
Esto nos permite encontrar ‘barco’ mediante ‘bar’, ‘arco’, etc.
Sin embargo, antes de almacenar el nombre de una película, es necesario limpiarlo, es decir, convertir todo a minúsculas, eliminar caracteres especiales y palabras o espacios innecesarios. Posteriormente, un nombre de una película se separa por palabras y se generan los sufijos de cada una. 

El pseudo código para insertar es el siguiente:

    for each palabra in texto:
        for i desde 0 hasta palabra.length():
            sufijo = palabra.substr(i)
            nodo = root
            for each caracter in sufijo:
                if caracter no existe:
                    crear nodo
                avanzar al siguiente nodo
                agregar movieID

La complejidad de este algoritmo es O(n^2). Aunque el costo de construcción es elevado, este proceso se realiza únicamente una vez al cargar la base de datos. 

### Algoritmo de Búsqueda:
La búsqueda consiste en recorrer el Trie siguiendo los caracteres del patrón buscado. Por ejemplo, si se quiere buscar ‘tasma’ en ‘fantasma’, se sigue el camino desde el caracter ‘t’ hasta el último ‘a’. Si en el camino no existe algún caracter, no hay ninguna coincidencia. A través del recorrido se obtienen las claves (IDs) de las películas que contiene la subcadena.

A continuación, el pseudocódigo:

    nodo = root
    
    for each caracter in patron:
    
        if caracter no existe:
            return vacío
    
        nodo = siguiente nodo
    
    return nodo.movieIDs

La complejidad depende únicamente del tamaño del patrón buscado, por lo que la complejidad es O(m), donde m es la longitud del texto buscado. Esto hace que las búsquedas sean rápidas incluso con una gran cantidad de películas.

Cuando el usuario busca una frase completa, el sistema:
divide la frase en palabras,
busca cada término individualmente,
combina resultados,
calcula relevancia mediante un sistema de ranking.
