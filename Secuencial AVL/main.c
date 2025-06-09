#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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


// ---------------------------------- Funciones del AVL ----------------------------------

/// pre: Requiere un nodo en argumento
///post: Retorna la altura del nodo - 0 si es NULL
int getHeight(struct Node* n) {
    if (n == NULL)
        return 0;
    return n->height;
}

/// pre: requiere un nodo, que es un puntero a una struct Node
///post: Cuenta los nodos del árbol (uso de memoria), retorna la cant de nodo, retorna 0 si el nodo es NULL
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
int max(int a, int b) {
    return (a > b) ? a : b;
}

/// pre: key es el dato, siendo un valor entero
///post: Crea e inicialia un nuevo nodo con el valor del dato key ingresado x parametro
struct Node* createNode(int key) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // <--- EL nuevo nodo se agrega en la hoja
    return node;
}

/// pre: Requiere un Nodo como parametro
///post: Calcula el balance del nodo, como la diferencia de altura entre sus subarboles izquierdo y derecho - Retorna 0 si es NULL
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

    // Realiza la rotacion
    x->right = y;
    y->left = T2;

    // Actualiza altura
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

/// pre: Requiere  un nodo como argumento
///post: Realiza una rotacion simple a la izquierda
///     para mantener el equilibrio del arbo tras cada insercion si esta desbalanceado por derecha
struct Node* leftRotate(struct Node* x) {

    struct Node* y = x->right;
    struct Node* T2 = y->left;

    // Realiza la rotacion
    y->left = x;
    x->right = T2;

    // Actualiza altura
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
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
    else // No se permiten datos duplicados
        return node;

    // Actualiza altura del nodo padre
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

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

/// pre: se ingresa el nodo raiz, y el dato a ingresar como parametro
///post: Verifica que el valor no este en el arbol, deben ser unicos, retorna 1 si lo encuentra, 0 si no y puede procederse a su insercion
int contains(struct Node* root, int key) {
    if (root == NULL) return 0;
    if (key == root->key) return 1;
    if (key < root->key)
        return contains(root->left, key);
    else
        return contains(root->right, key);
}

/// pre: Requiere un nodo raiz como parametro
///post: Realiza el recorrido in order del AVL ( izquierda - raiz - derecha ) permitiendo imprimir el dato del arbon en orden ascendente
void inOrder(struct Node* root) {
    if (root != NULL) {
        inOrder(root->left);
        printf("%d ", root->key);
        inOrder(root->right);
    }
}

//
// Encuentra el nodo con el valor mínimo (usado en la eliminación)
struct Node* minValueNode(struct Node* node) {
    struct Node* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Elimina un nodo del árbol AVL
struct Node* deleteNode(struct Node* root, int key) {
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // Nodo con solo un hijo o sin hijos
        if ((root->left == NULL) || (root->right == NULL)) {
            struct Node* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp; // Copia los datos
            }

            free(temp);
        } else {
            // Nodo con dos hijos: obtiene el sucesor in-order
            struct Node* temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    // Si el árbol tenía un solo nodo
    if (root == NULL)
        return root;

    // Actualiza la altura
    root->height = 1 + max(getHeight(root->left), getHeight(root->right));

    // Obtiene el balance
    int balance = getBalanceFactor(root);

    // Rebalanceo
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

// Función para liberar memoria del árbol AVL recursivamente
void liberarArbol(struct Node* nodo) {
    if (nodo == NULL) return;
    liberarArbol(nodo->left);
    liberarArbol(nodo->right);
    free(nodo);
}
//

//      ----------------------------------  FUNCION MAIN  ----------------------------------

int main() {
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estructura para almacenar los tiempos de operaciones
struct TiemposAVL {
    double tiempoInsercion;
    double tiempoMostrar;
    double tiempoBusqueda;
    double tiempoEliminacion;
} tiempos = {0, 0, 0, 0};

struct Node* root = NULL;
int opcion;
int cantidad, min, max;

do {
    printf("\n======= MENU AVL =======\n");
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
    printf("\n");

    switch (opcion) {
        case 1: {
            // Verifica si ya existe un árbol y pide confirmación para sobreescribir
            if (root != NULL) {
                printf("Ya existe un arbol. ¿Desea sobrescribirlo? (1=Si / 0=No): ");
                int confirm;
                scanf("%d", &confirm);
                if (!confirm) break;

                liberarArbol(root); // Libera la memoria del árbol anterior
                root = NULL;
            }

            printf("Cuantos elementos desea insertar en el arbol AVL? ");
            scanf("%d", &cantidad);
            printf("Ingrese el valor minimo del rango: ");
            scanf("%d", &min);
            printf("Ingrese el valor maximo del rango: ");
            scanf("%d", &max);

            // Valida que el rango permita insertar valores únicos
            if (max - min + 1 < cantidad) {
                printf("Rango demasiado pequenio para insertar %d valores unicos.\n", cantidad);
                break;
            }

            srand(time(NULL));
            int inserted = 0;

            // Medir tiempo de inserción
            clock_t start = clock();

            // Inserta valores aleatorios únicos
            while (inserted < cantidad) {
                int value = (rand() % (max - min + 1)) + min;
                if (!contains(root, value)) {
                    root = insert(root, value);
                    inserted++;
                }
            }

            clock_t end = clock();
            tiempos.tiempoInsercion = ((double)(end - start) *1000) / CLOCKS_PER_SEC;
            printf("Tiempo total de insercion: %.4lf mili segundos\n", tiempos.tiempoInsercion);
            break;
        }
        case 2: {
            // Muestra recorrido InOrder del árbol
            if (root == NULL) {
                printf("El arbol esta vacio.\n");
            } else {
                printf("Recorrido InOrder: ");
                // Medir tiempo del recorrido
                clock_t start = clock();
                inOrder(root);
                clock_t end = clock();
                tiempos.tiempoMostrar = ((double)(end - start) *1000 ) / CLOCKS_PER_SEC;
                printf("\nTiempo de recorrido: %.4lf mili segundos\n", tiempos.tiempoMostrar);
            }
            break;
        }
        case 3: {
            // Buscar valor e indicar el nivel donde se encuentra
            if (root == NULL) {
                printf("El arbol esta vacio.\n");
            } else {
                int valor;
                printf("Ingrese el valor a buscar: ");
                scanf("%d", &valor);
                clock_t start = clock();
                int nivel = buscarConProfundidad(root, valor, 0);
                clock_t end = clock();
                tiempos.tiempoBusqueda = ((double)(end - start) *1000 ) / CLOCKS_PER_SEC;

                if (nivel != -1)
                    printf("Valor encontrado en el nivel %d.\n", nivel);
                else
                    printf("Valor no encontrado.\n");

                printf("Tiempo de busqueda: %.4lf mili segundos\n", tiempos.tiempoBusqueda);
            }
            break;
        }
        case 4: {
            // Eliminar un valor del árbol
            if (root == NULL) {
                printf("El arbol esta vacio.\n");
            } else {
                int valor;
                printf("Ingrese el valor a eliminar: ");
                scanf("%d", &valor);
                clock_t start = clock();
                if (buscarAVL(root, valor)) {
                    root = deleteNode(root, valor);
                    clock_t end = clock();
                    tiempos.tiempoEliminacion = ((double)(end - start) *1000 ) / CLOCKS_PER_SEC;
                    printf("Valor eliminado.\n");
                } else {
                    clock_t end = clock();
                    tiempos.tiempoEliminacion = ((double)(end - start) *1000 ) / CLOCKS_PER_SEC;
                    printf("El valor no se encuentra en el arbol.\n");
                }
                printf("Tiempo de busqueda y eliminacion: %.4lf en mili segundos\n", tiempos.tiempoEliminacion);
            }
            break;
        }
        case 5: {
            // Mostrar cantidad de nodos, altura y memoria utilizada
            if (root == NULL) {
                printf("El arbol esta vacio.\n");
            } else {
                int totalNodos = contarNodos(root);
                int altura = calcularAltura(root);
                size_t memoria = totalNodos * sizeof(struct Node);
                printf("Total de nodos: %d\n", totalNodos);
                printf("Altura del arbol: %d\n", altura);
                printf("Uso aproximado de memoria: %zu bytes\n", memoria);
            }
            break;
        }
        case 6: {
            // Reinicia el árbol borrando todos los nodos
            liberarArbol(root);
            root = NULL;
            printf("Arbol reiniciado exitosamente.\n");
            break;
        }
        case 7: {
            // Mostrar tabla de tiempos y guardar en archivo
            printf("\n======== RESUMEN DE TIEMPOS ========\n");
            printf("| %-25s | %-10s |\n", "Operacion", "Tiempo (ms)");
            printf("|---------------------------|------------|\n");
            printf("| %-25s | %-10.4lf |\n", "Insercion", tiempos.tiempoInsercion);
            printf("| %-25s | %-10.4lf |\n", "Mostrar (InOrder)", tiempos.tiempoMostrar);
            printf("| %-25s | %-10.4lf |\n", "Busqueda", tiempos.tiempoBusqueda);
            printf("| %-25s | %-10.4lf |\n", "Busqueda + Eliminacion", tiempos.tiempoEliminacion);

            // Guardar resultados en archivo
            FILE* archivo = fopen("tiempos_avl.txt", "w");
            if (archivo != NULL) {
                fprintf(archivo, "======== RESUMEN DE TIEMPOS ========\n");
                fprintf(archivo, "| %-25s | %-10s |\n", "Operacion", "Tiempo (ms)");
                fprintf(archivo, "|---------------------------|------------|\n");
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Insercion", tiempos.tiempoInsercion);
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Mostrar (InOrder)", tiempos.tiempoMostrar);
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Busqueda", tiempos.tiempoBusqueda);
                fprintf(archivo, "| %-25s | %-10.4lf |\n", "Busqueda + Eliminacion", tiempos.tiempoEliminacion);
                fclose(archivo);
                printf("Los tiempos fueron guardados en 'tiempos_avl.txt'.\n");
            } else {
                printf("No se pudo guardar el archivo.\n");
            }

            break;
        }
        case 0: {
            // Salida del programa, libera memoria antes de salir
            liberarArbol(root);
            printf("Saliendo del programa...\n");
            break;
        }
        default:
            printf("Opcion invalida. Intente de nuevo.\n");
    }

} while (opcion != 0);



    return 0;
}
