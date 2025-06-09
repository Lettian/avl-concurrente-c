#include <stdio.h>
#include <stdlib.h>
#include <windows.h>    // Para crear y gestionar hilos y mutex en Windows - permite precision en obtener milisegundos

// Mutex Son mecanismos de sincronización que controlan el acceso a recursos compartidos
// Garantizan que solo un hilo pueda acceder a un recurso compartido en un momento dado
// Se usa para evitar conflictos y asegurar integracion de datos en la programacion multihilo

#include <time.h>       // Para inicializar el generador de numeros aleatorios


double medirTiempo(void (*func)(void*), void* args) {
    LARGE_INTEGER start, end, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    func(args);  // Ejecuta la función que querés medir

    QueryPerformanceCounter(&end);
    return (double)(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;

}


// ---------- Definición del nodo AVL ----------

// Estructura del Nodo arbol AVL
// key: representa el dato.
// posee un puntero a sus hijos, siendo otro nodo, izq y derecho
// height: representa la altura, y permite calcular el balance del arbol
/// Estructura NODO - key: es el dato siendo un entero - poseen punteros a sus hijos left y rigth, siendo nodos del lado izq y der

struct Node {
    int key;
    struct Node* left;
    struct Node* right;
    int height;
};

// Estructura para almacenar los tiempos de operaciones
struct TiemposAVL {
    double tiempoInsercion;
    double tiempoMostrar;
    double tiempoBusqueda;
    double tiempoEliminacion;
} tiempos = {0, 0, 0, 0};


// ---------------------------------- AVL Global ----------------------------------

// Nodo raiz del arbol
struct Node* root = NULL;

// ---------------------------------- Mutex para sincronización ----------------------------------
/// Mutex para evitar condiciones de carrera al insertar desde múltiples hilos
HANDLE tree_mutex;

// ---------------------------------- Funciones del AVL ----------------------------------
/// pre: Requiere un nodo en argumento
///post: Retorna la altura del nodo - 0 si es NULL
int getHeight(struct Node* n) {
    return (n == NULL) ? 0 : n->height;
}

// Cuenta los nodos del árbol (uso de memoria)
int contarNodos(struct Node * nodo) {
    if (nodo == NULL) return 0;
    return 1 + contarNodos(nodo->left) + contarNodos(nodo->right);
}

// Calcula la altura real del árbol
/// pre: requiere un nodo, que es un puntero a una struct Node
///post: retorna la altura del arbol, siendo este un valor entero
int calcularAltura(struct Node * nodo) {
    if (nodo == NULL) return 0;
    int altIzq = calcularAltura(nodo->left);
    int altDer = calcularAltura(nodo->right);
    return 1 + (altIzq > altDer ? altIzq : altDer);
}

// Buscar un valor en el árbol y devolver su profundidad
/// pre: requiere un nodo, que es un puntero a una struct Node - valor: dato a buscar - nivel: nivel del arbol
int buscarConProfundidad(struct Node* raiz, int valor, int nivel) {
    if (raiz == NULL) {
        return -1;  // No encontrado
    }
    if (valor == raiz->key) {
        return nivel;  // Encontrado en este nivel
    } else if (valor < raiz->key) {
        return buscarConProfundidad(raiz->left, valor, nivel + 1);
    } else {
        return buscarConProfundidad(raiz->right, valor, nivel + 1);
    }
}

/// pre: a y b son valores enteros
///post: Compara a y b, retornando el valor mas grande
int mayor(int a, int b) {
    return (a > b) ? a : b;
}

/// pre: key es el dato, siendo un valor entero
///post: Crea e inicialia un nuevo nodo con el valor del dato key ingresado x parametro
struct Node* createNode(int key) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

/// pre: Requiere un Nodo como parametro
///post: Calcula el balance del nodo para saber si hay que rotar
///         como la diferencia de altura entre sus subarboles izquierdo y derecho - Retorna 0 si es NULL
int getBalanceFactor(struct Node* n) {
    if (n == NULL)
        return 0;
    return getHeight(n->left) - getHeight(n->right);
}

/// pre: Requiere  un nodo como argumento
///post: Realiza una rotacion simple a la derecha
///     para mantener el equilibrio del arbo tras cada insercion si esta desbalanceado por izquierda
struct Node* rightRotate(struct Node* y) {
    struct Node* x = y->left;
    struct Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = mayor(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = mayor(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

/// pre: Requiere  un nodo como argumento
///post: Realiza una rotacion simple a la izquierda
///     para mantener el equilibrio del arbo tras cada insercion si esta desbalanceado por derecha
struct Node* leftRotate(struct Node* x) {
    struct Node* y = x->right;
    struct Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = mayor(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = mayor(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

/// pre: Requiere un puntero al nodo y el dato siendo un entero que se quiera insertar en el AVL
///post: funcion recursiva, inserta un dato al arbol.
///     Si el nodo actual es NULL, crea uno.
///     Si el valor existe, no se inserta por no admitir duplicados
///     Al insertar el dato, se actualiza la altura del nodo y se calcula su balance.
///     Si esta desquilibrado, se ejecutan 1 de las 4 posibles rotaciones necesarias.
struct Node* insert(struct Node* node, int key) {

    // Realiza la insercion del arbol de busqueda binario
    if (node == NULL)
        return createNode(key); // Caso base: nodo vacío, se crea uno nuevo

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else    // No se permiten datos duplicados
        return node; // No se permiten duplicados

    // Actualiza altura del nodo padre
    node->height = 1 + mayor(getHeight(node->left), getHeight(node->right));

    // Obtiene el factor de equilibrio del nodo padre para corroborar que no se haya producido un desequilibrio
    int balance = getBalanceFactor(node);

    // Existen 4 casos si el nodo se encuentra desquilibrado:

    // 1. Caso rotacion doble a la izquierda
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // 2. Caso rotacion doble a la derecha
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // 3. Caso rotacion izquierda-derecha
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // 4. Caso rotacion derecha-izquierda
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node; // Nodo ya balanceado
}


// Buscar un valor en un árbol AVL
/// pre: Requiere un puntero a una estructura nodo, y un valor entero  que se desea buscar como 2do parametro
///post: Busca un valor en el arbol AVL - Retorna 0 si no lo encuentra - Retorna 1 si esta en el arbol
int buscarAVL(struct Node* raiz, int key) {

    if (raiz == NULL) {
        return 0;  // No encontrado
    }
    if (key == raiz->key) {
        return 1;  // Encontrado
    } else if (key < raiz->key) {
        return buscarAVL(raiz->left, key);
    } else {
        return buscarAVL(raiz->right, key);
    }
}

/// pre: Requiere un nodo raiz como parametro
///post: Realiza el recorrido in order del AVL ( izquierda - raiz - derecha ) permitiendo imprimir el dato del arbon en orden ascendente
void printInOrder(struct Node* node) {
    if (node == NULL)
        return;
    printInOrder(node->left);
    printf("%d ", node->key);
    printInOrder(node->right);
}

/// pre: Requiere un nodo
///post: Retorna el nodo con el valor mínimo (más a la izquierda), usado para reemplazo en borrado
struct Node* minValueNode(struct Node* node) {
    struct Node* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

/// pre: Requiere un nodo y una clave a eliminar
///post: Elimina el nodo con la clave del AVL y lo rebalancea
struct Node* deleteNode(struct Node* root, int key) {
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // Nodo con un hijo o ninguno
        if ((root->left == NULL) || (root->right == NULL)) {
            struct Node* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }

            free(temp);
        } else {
            // Nodo con dos hijos: obtener sucesor en inorden
            struct Node* temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    if (root == NULL)
        return root;

    // Actualizar altura
    root->height = 1 + mayor(getHeight(root->left), getHeight(root->right));

    // Obtener balance
    int balance = getBalanceFactor(root);

    // Rebalancear si es necesario
    if (balance > 1 && getBalanceFactor(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalanceFactor(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}


// ---------------------------------- Argumentos para los hilos ----------------------------------
struct ThreadArgs {
    int cantidad;   // Cantidad de valores a insertar
    int min;        // Valor minimo del rango
    int max;        // Valor maximo del rango
};

// ---------------------------------- Función que ejecuta cada hilo ----------------------------------
/// pre:
///post: Cada hilo genera cantidad números aleatorios entre min y max, y los inserta en el árbol usando mutex para evitar colisiones
DWORD WINAPI threadInsert(LPVOID args) {

    struct ThreadArgs* ta = (struct ThreadArgs*)args;
    int inserted = 0;

    while (inserted < ta->cantidad) {
        int val = rand() % (ta->max - ta->min + 1) + ta->min;

        WaitForSingleObject(tree_mutex, INFINITE);  // Bloquear mutex, Bloquea el acceso al árbol

        if (!buscarAVL(root, val)) {    // Solo insertamos dato  no existe
            root = insert(root, val);   // Inserta valor
            inserted++;
        }

        ReleaseMutex(tree_mutex);                   // Liberar mutex, libera el arbol
    }

    return 0;
}

void medirTiempoBusqueda(void* arg) {
    int* val = (int*)arg;
    buscarAVL(root, *val);
}

void medirTiempoEliminacion(void* arg) {
    int* val = (int*)arg;
    root = deleteNode(root, *val);
}

void medirTiempoMostrar(void* arg) {
    printInOrder(root);
}

void medirInsercionConHilos(int total, int threads, int min, int max) {
    HANDLE hilos[threads];
    struct ThreadArgs argumentos[threads];

    for (int i = 0; i < threads; i++) {
        argumentos[i].cantidad = total / threads;
        argumentos[i].min = min;
        argumentos[i].max = max;
    }

    tiempos.tiempoInsercion = medirTiempo((void (*)(void*))threadInsert, &argumentos[0]);

    for (int i = 1; i < threads; i++) {
        hilos[i] = CreateThread(NULL, 0, threadInsert, &argumentos[i], 0, NULL);
    }

    // Espera que terminen los hilos
    for (int i = 1; i < threads; i++) {
        WaitForSingleObject(hilos[i], INFINITE);
        CloseHandle(hilos[i]);
    }
}


// ---------- Función principal ----------
int main() {
int opcion;
    int valor;
    int total, threads, min, max;
    DWORD start, end;

    tree_mutex = CreateMutex(NULL, FALSE, NULL); // Inicializar mutex global

    do {
        printf("\n======= MENU AVL CONCURRENTE =======\n");
        printf("1. Crear un nuevo arbol AVL\n");
        printf("2. Mostrar recorrido InOrder\n");
        printf("3. Buscar valor\n");
        printf("4. Eliminar valor\n");
        printf("5. Mostrar altura y cantidad de nodos\n");
        printf("6. Reiniciar arbol AVL\n");
        printf("7. Mostrar tabla de tiempos y guardar en .txt\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:{
                // Crear nuevo árbol
                printf("Cuantos elementos desea insertar en el árbol AVL?: ");
                scanf("%d", &total);
                printf("Cantidad de hilos que usará el programa?: ");
                scanf("%d", &threads);
                printf("Ingrese el valor minimo del rango: ");
                scanf("%d", &min);
                printf("Ingrese el valor maximo del rango: ");
                scanf("%d", &max);

                if ((max - min + 1) < total) {
                    printf("El rango es demasiado pequeño para insertar %d valores únicos.\n", total);
                    break;
                }

                srand((unsigned int)time(NULL));  // Inicializar aleatorio

                HANDLE hilos[threads];
                struct ThreadArgs args[threads];

                int porHilo = total / threads;
                int resto = total % threads;

                //start = GetTickCount();
                // medir tiempo insercion
                clock_t start = clock();
                for (int i = 0; i < threads; i++) {
                    args[i].cantidad = porHilo + (i < resto ? 1 : 0);
                    args[i].min = min;
                    args[i].max = max;

                    hilos[i] = CreateThread(NULL, 0, threadInsert, &args[i], 0, NULL);
                }

                for (int i = 0; i < threads; i++) {
                    WaitForSingleObject(hilos[i], INFINITE);
                    CloseHandle(hilos[i]);
                }

                //end = GetTickCount();
                clock_t end = clock();
                tiempos.tiempoInsercion = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("Tiempo total de inserción: %.4lf milisegundos\n", tiempos.tiempoInsercion);
                break;
        }
            case 2:{
                if (root == NULL) {
                    printf("El árbol está vacío.\n");
                } else {
                    //start = GetTickCount();
                    clock_t start = clock();
                    printf("Recorrido InOrder del árbol: ");
                    printInOrder(root);
                    printf("\n");
                    //end = GetTickCount();
                    clock_t end = clock();
                    tiempos.tiempoMostrar = ((double)(end - start)) / CLOCKS_PER_SEC;
                    printf("Tiempo de recorrido InOrder: %.4lf milisegundos\n", tiempos.tiempoMostrar);

                }
                break;
            }
            case 3:{
                if (root == NULL) {
                    printf("El árbol está vacío.\n");
                } else {
                    printf("Ingrese valor a buscar: ");
                    scanf("%d", &valor);
                    //start = GetTickCount();
                    clock_t start = clock();
                    int nivel = buscarConProfundidad(root, valor, 0);
                    clock_t end = clock();
                    tiempos.tiempoBusqueda = ((double)(end - start)) / CLOCKS_PER_SEC;

                if (nivel != -1)
                    printf("Valor encontrado en el nivel %d.\n", nivel);
                else
                    printf("Valor no encontrado.\n");

                printf("Tiempo de busqueda: %.8lf milisegundos\n", tiempos.tiempoBusqueda);
                }
                break;
            }
            case 4:{
                if (root == NULL) {
                    printf("El árbol está vacío.\n");
                } else {
                        int valor;
                        printf("Ingrese valor a eliminar: ");
                        scanf("%d", &valor);
                        //start = GetTickCount();
                        clock_t start = clock();
                    if (buscarAVL(root, valor)) {
                        root = deleteNode(root, valor);
                        //end = GetTickCount();
                        clock_t end = clock();
                        tiempos.tiempoEliminacion = ((double)(end - start)) / CLOCKS_PER_SEC;
                        printf("Valor eliminado correctamente.\n");
                    } else {
                        //end = GetTickCount();
                        clock_t end = clock();
                        tiempos.tiempoEliminacion = ((double)(end - start)) / CLOCKS_PER_SEC;
                        printf("El valor no existe en el árbol.\n");
                    }
                    printf("Tiempo de busqueda y eliminacion: %.8lf  ns milisegundos\n", end - start);
                }
                break;
            }
            case 5:{
                // Mostrar cantidad de nodos, altura y memoria utilizada
                if (root == NULL) {
                    printf("El árbol está vacío.\n");
                } else {
                    int altura = calcularAltura(root);
                    int nodos = contarNodos(root);
                    size_t memoria = nodos * sizeof(struct Node);
                    printf("Altura del árbol: %d\n", altura);
                    printf("Cantidad de nodos: %d\n", nodos);
                    printf("Uso aproximado de memoria: %zu bytes\n", memoria);
                }
                break;
            }
            case 6:{
                // Reinicia el árbol borrando todos los nodos
                if (root != NULL) {
                    // Liberar memoria recursivamente
                    while (root != NULL) {
                        root = deleteNode(root, root->key);
                    }
                    printf("Árbol reiniciado correctamente.\n");
                } else {
                    printf("El árbol ya está vacío.\n");
                }
                break;
            }
            case 0:{
                printf("Saliendo del programa...\n");
                break;
            }
            case 7:{

            // Mostrar tabla de tiempos y guardar en archivo
            printf("\n======== RESUMEN DE TIEMPOS ========\n");
            printf("| %-25s | %-10s |\n", "Operacion", "Tiempo (ms)");
            printf("|---------------------------|------------|\n");
            printf("| %-25s | %-10.4lf |\n", "Insercion", tiempos.tiempoInsercion);
            printf("| %-25s | %-10.4lf |\n", "Mostrar (InOrder)", tiempos.tiempoMostrar);
            printf("| %-25s | %-10.4lf |\n", "Busqueda", tiempos.tiempoBusqueda);
            printf("| %-25s | %-10.4lf |\n", "Busqueda + Eliminacion", tiempos.tiempoEliminacion);

            // Guardar resultados en archivo
            FILE* archivo = fopen("concurrente_tiempos_avl.txt", "w");
            if (archivo != NULL) {
                fprintf(archivo, "======== RESUMEN DE TIEMPOS ========\n");
                fprintf(archivo, "| %-25s | %-10s |\n", "Operacion", "Tiempo (ms)");
                fprintf(archivo, "|---------------------------|------------|\n");
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Insercion", tiempos.tiempoInsercion);
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Mostrar (InOrder)", tiempos.tiempoMostrar);
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Busqueda", tiempos.tiempoBusqueda);
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Busqueda + Eliminacion", tiempos.tiempoEliminacion);
                fclose(archivo);
                printf("Los tiempos fueron guardados en 'concurrente_tiempos_avl.txt'.\n");
            } else {
                printf("No se pudo guardar el archivo.\n");
            }

            break;}
            default:
                printf("Opción inválida. Intente de nuevo.\n");
        }

    } while (opcion != 0);

    // Liberar recursos
    CloseHandle(tree_mutex);            // Cierra el mutex

    return 0;
}
