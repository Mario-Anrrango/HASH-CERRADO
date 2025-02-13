#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp> // Librería SFML para gráficos

using namespace std;
using namespace sf;

// Función para verificar si es seguro colocar una reina en la posición (fila, col)
bool esSeguro(vector<vector<int>>& tablero, int fila, int col, int N) {
    // Verificar la fila hacia la izquierda
    for (int i = 0; i < col; i++) {
        if (tablero[fila][i]) {
            return false;
        }
    }

    // Verificar la diagonal superior izquierda
    for (int i = fila, j = col; i >= 0 && j >= 0; i--, j--) {
        if (tablero[i][j]) {
            return false;
        }
    }

    // Verificar la diagonal inferior izquierda
    for (int i = fila, j = col; i < N && j >= 0; i++, j--) {
        if (tablero[i][j]) {
            return false;
        }
    }

    return true;
}

// Función para dibujar el tablero y guardar la imagen
void guardarTablero(const vector<vector<int>>& tablero, int N, int paso) {
    const int tamanoCelda = 50; // Tamaño de cada celda del tablero
    RenderWindow window(VideoMode(N * tamanoCelda, N * tamanoCelda), "N Reinas");

    // Crear un rectángulo para las celdas del tablero
    RectangleShape celda(Vector2f(tamanoCelda, tamanoCelda));
    celda.setOutlineThickness(1);
    celda.setOutlineColor(Color::Black);

    // Crear un círculo para representar las reinas
    CircleShape reina(tamanoCelda / 2.5f);
    reina.setFillColor(Color::Red);

    // Crear un rectángulo para resaltar las celdas no seguras en verde
    RectangleShape resaltar(Vector2f(tamanoCelda, tamanoCelda));
    resaltar.setFillColor(Color(0, 255, 0, 100)); // Verde con transparencia para las celdas no seguras

    // Dibujar el tablero
    window.clear(Color::White);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Alternar colores de las celdas
            celda.setPosition(j * tamanoCelda, i * tamanoCelda);
            celda.setFillColor((i + j) % 2 == 0 ? Color::White : Color::Black);
            window.draw(celda);

            // Resaltar las celdas no seguras
            if (!esSeguro(const_cast<vector<vector<int>>&>(tablero), i, j, N)) {
                resaltar.setPosition(j * tamanoCelda, i * tamanoCelda);
                window.draw(resaltar);
            }

            // Dibujar la reina si está presente
            if (tablero[i][j] == 1) {
                reina.setPosition(j * tamanoCelda + (tamanoCelda - reina.getRadius() * 2) / 2,
                                   i * tamanoCelda + (tamanoCelda - reina.getRadius() * 2) / 2);
                window.draw(reina);
            }
        }
    }

    // Guardar la imagen en la carpeta "Soluciones"
    Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    if (!texture.copyToImage().saveToFile("Soluciones/(" + to_string(paso) + ").png")) {
        cerr << "Error al guardar la imagen." << endl;
    }

    window.close();
}

// Función recursiva para resolver el problema de las N reinas
bool resolverNReinas(vector<vector<int>>& tablero, int col, int N, int& paso) {
    // Si todas las reinas están colocadas, retornar verdadero
    if (col >= N) {
        return true;
    }

    // Intentar colocar una reina en cada fila de la columna actual
    for (int i = 0; i < N; i++) {
        if (esSeguro(tablero, i, col, N)) {
            // Colocar la reina en la posición (i, col)
            tablero[i][col] = 1;

            // Guardar el estado actual del tablero como una imagen
            guardarTablero(tablero, N, paso++);

            // Recursivamente intentar colocar el resto de las reinas
            if (resolverNReinas(tablero, col + 1, N, paso)) {
                return true;
            }

            // Si no se puede colocar la reina en (i, col), retroceder (backtracking)
            tablero[i][col] = 0;

            // Guardar el estado actual del tablero después de retroceder
            guardarTablero(tablero, N, paso++);
        }
    }

    // Si no se puede colocar la reina en ninguna fila de esta columna, retornar falso
    return false;
}

// Función principal para resolver el problema de las N reinas
void resolverProblemaNReinas() {
    int N;
    
    // Solicitar al usuario el número de reinas
    do {
        cout << "Ingrese el numero de reinas (N) (debe ser entre 1 y 8): ";
        cin >> N;

        if (N < 1 || N > 8) {
            cout << "Número inválido. Por favor ingrese un número entre 1 y 8." << endl;
        }
    } while (N < 1 || N > 8); // Validación del número de reinas

    // Crear un tablero de N x N inicializado con 0s
    vector<vector<int>> tablero(N, vector<int>(N, 0));

    // Borrar la carpeta "Soluciones" si ya existe para evitar solapamiento de imágenes
    system("rmdir /S /Q Soluciones");

    // Crear la carpeta "Soluciones"
    system("mkdir Soluciones");

    // Contador para el número de pasos (imágenes guardadas)
    int paso = 1;

    // Intentar resolver el problema
    if (resolverNReinas(tablero, 0, N, paso)) {
        cout << "Solucion encontrada. Imagenes guardadas en la carpeta 'Soluciones'." << endl;
    } else {
        cout << "No existe solucion para el problema de las " << N << " reinas." << endl;
    }
}

int main() {
    // Resolver el problema de las N reinas
    resolverProblemaNReinas();

    return 0;
}
