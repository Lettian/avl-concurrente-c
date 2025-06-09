# Árbol AVL Secuencial y Concurrente en C

Este proyecto implementa un Árbol AVL en lenguaje C en dos versiones: una secuencial y otra concurrente utilizando hilos en Windows. 
Permite insertar, buscar, eliminar y recorrer nodos, y compara el rendimiento entre ambas implementaciones.

## 📌 Funcionalidades

- Inserción de elementos únicos
- Recorrido inOrder
- Búsqueda y eliminación
- Inserción concurrente con hilos
- Uso de mutex para evitar condiciones de carrera
- Medición de tiempo por operación (ms)
- Exportación de resultados a archivo `.txt`
- Menú interactivo por consola

## 🧪 Casos de prueba

Se realizaron pruebas con volúmenes de:  
10, 100, 1.000, 10.000, 20.000 y 30.000 elementos, usando 4 hilos en la versión concurrente.

🔗 Ver resultados detallados:  
📄 [casos_prueba_avl.pdf](https://github.com/user-attachments/files/20647580/casos_prueba_avl.pdf)

## 🖥️ Requisitos

- Sistema: Windows 10/11
- Compilador: MinGW o compatible con Windows API
- Editor sugerido: Code::Blocks o Visual Studio Code
de::Blocks o Visual Studio Code

## 📦 Compilación

Compilar con:

```bash
gcc programa.c -o avl.exe -lpthread

NOTA: En Windows, asegurarse de incluir windows.h y compilar con las librerías adecuadas para hilos (CreateThread, HANDLE, WaitForSingleObject, etc.).

