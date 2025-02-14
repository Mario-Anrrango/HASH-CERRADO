#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <unordered_map>
#include <conio.h>

using namespace std;

const float RADIO = 30.0f;
const float PI = 3.14159265359f;

class Grafo {
protected:
    unordered_map<string, vector<string>> listaAdyacencia;
    unordered_map<string, unordered_map<string, int>> matrizAdyacencia;
    unordered_map<string, sf::Vector2f> posiciones;
    vector<string> nodos;

public:
    Grafo(int n) {
        for (int i = 0; i < n; i++) {
            string nombre;
            cout << "Ingrese el nombre del nodo " << i + 1 << ": ";
            cin >> nombre;
            nodos.push_back(nombre);
        }
        distribuirNodos();
    }
    
    virtual void agregarArista(string origen, string destino) = 0;
    virtual void mostrarRepresentacion() = 0;

    void distribuirNodos() {
        float centroX = 400, centroY = 300;
        float radioCirculo = 200;
        for (size_t i = 0; i < nodos.size(); i++) {
            float angulo = (2 * PI * i) / nodos.size();
            posiciones[nodos[i]] = sf::Vector2f(centroX + radioCirculo * cos(angulo),
                                                centroY + radioCirculo * sin(angulo));
        }
    }

    void mostrarMatrizAdyacencia() {
        cout << "Matriz de Adyacencia:\n";
        cout << "   ";
        for (const auto &nodo : nodos) {
            cout << nodo << " ";
        }
        cout << endl;
        for (const auto &origen : nodos) {
            cout << origen << " ";
            for (const auto &destino : nodos) {
                cout << (matrizAdyacencia[origen][destino] ? "1 " : "0 ");
            }
            cout << endl;
        }
    }

    void dibujarFlecha(sf::RenderWindow& window, sf::Vector2f origen, sf::Vector2f destino, sf::Color color) {
        // Dibujar la línea de la arista
        sf::Vertex line[] = {
            sf::Vertex(origen, color),
            sf::Vertex(destino, color)
        };
        window.draw(line, 2, sf::Lines);

        // Calcular la dirección de la flecha
        float angulo = atan2(destino.y - origen.y, destino.x - origen.x);
        float flechaLongitud = 15.0f;
        sf::Vector2f puntoFinal = sf::Vector2f(
            destino.x - RADIO * cos(angulo),
            destino.y - RADIO * sin(angulo)
        );

        // Dibujar la flecha (triángulo)
        sf::ConvexShape flecha;
        flecha.setPointCount(3);
        flecha.setPoint(0, puntoFinal);
        flecha.setPoint(1, sf::Vector2f(
            puntoFinal.x - flechaLongitud * cos(angulo - PI / 6),
            puntoFinal.y - flechaLongitud * sin(angulo - PI / 6))
        );
        flecha.setPoint(2, sf::Vector2f(
            puntoFinal.x - flechaLongitud * cos(angulo + PI / 6),
            puntoFinal.y - flechaLongitud * sin(angulo + PI / 6))
        );
        flecha.setFillColor(color);
        window.draw(flecha);
    }

    void dibujarCurva(sf::RenderWindow& window, sf::Vector2f origen, sf::Color color) {
        // Usaremos varios puntos para crear una curva (un lazo)
        sf::VertexArray curva(sf::LinesStrip, 20);
        
        // Calculamos los puntos de la curva para que se cierre de vuelta al nodo
        for (int i = 0; i < 20; i++) {
            float t = (float)i / 19.0f; // Normalizamos el parámetro t entre 0 y 1
            float angulo = t * 2 * PI; // Ángulo para la curva
            
            // La curva se mueve en un círculo alrededor del nodo (pero sin salirse de la ventana)
            float radioCurva = 50.0f; // Radio de la curva para asegurarnos de que no se aleje mucho
            float x = origen.x + radioCurva * cos(angulo);
            float y = origen.y + radioCurva * sin(angulo);
            
            // Añadimos los puntos al array de la curva
            curva[i].position = sf::Vector2f(x, y);
            curva[i].color = color;
        }
    
        // Dibujamos la curva
        window.draw(curva);
    }
    
    void representarSFML(bool dirigido) {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Grafo");
        sf::Font font;
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            cerr << "Error al cargar la fuente. Asegúrate de tener arial.ttf en el directorio.\n";
            return;
        }
    
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
    
            window.clear(sf::Color::White);
    
            for (auto &nodo : listaAdyacencia) {
                string origen = nodo.first;
                for (string destino : nodo.second) {
                    if (origen == destino) {
                        // Dibujar curva si el origen y destino son el mismo
                        dibujarCurva(window, posiciones[origen], sf::Color::Black);
                    } else {
                        // Dibujar arista con flecha si es dirigido
                        if (dirigido) {
                            dibujarFlecha(window, posiciones[origen], posiciones[destino], sf::Color::Black);
                        } else {
                            // Dibujar línea simple si no es dirigido
                            sf::Vertex line[] = {
                                sf::Vertex(posiciones[origen], sf::Color::Black),
                                sf::Vertex(posiciones[destino], sf::Color::Black)
                            };
                            window.draw(line, 2, sf::Lines);
                        }
                    }
                }
            }
    
            for (auto &pos : posiciones) {
                sf::CircleShape circle(RADIO);
                circle.setFillColor(sf::Color::Blue);
                circle.setPosition(pos.second.x - RADIO, pos.second.y - RADIO);
                window.draw(circle);
    
                sf::Text text;
                text.setFont(font);
                text.setString(pos.first);
                text.setCharacterSize(15);
                text.setFillColor(sf::Color::White);
                text.setPosition(pos.second.x - 10, pos.second.y - 10);
                window.draw(text);
            }
    
            window.display();
        }
    }  
};

class GrafoDirigido : public Grafo {
public:
    GrafoDirigido(int n) : Grafo(n) {}
    
    void agregarArista(string origen, string destino) override {
        listaAdyacencia[origen].push_back(destino);
        matrizAdyacencia[origen][destino] = 1;
    }
    
    void mostrarRepresentacion() override {
        cout << "Lista de Adyacencia:\n";
        for (auto &par : listaAdyacencia) {
            cout << par.first << " -> ";
            for (auto &ady : par.second) {
                cout << ady << " ";
            }
            cout << endl;
        }
        mostrarMatrizAdyacencia();
    }
};

class GrafoNoDirigido : public Grafo {
public:
    GrafoNoDirigido(int n) : Grafo(n) {}
    
    void agregarArista(string origen, string destino) override {
        listaAdyacencia[origen].push_back(destino);
        listaAdyacencia[destino].push_back(origen);
        matrizAdyacencia[origen][destino] = 1;
        matrizAdyacencia[destino][origen] = 1;
    }
    
    void mostrarRepresentacion() override {
        cout << "Lista de Adyacencia:\n";
        for (auto &par : listaAdyacencia) {
            cout << par.first << " -> ";
            for (auto &ady : par.second) {
                cout << ady << " ";
            }
            cout << endl;
        }
        mostrarMatrizAdyacencia();
    }
};

int mostrarMenu() {
    vector<string> opciones = {"Grafo Dirigido", "Grafo No Dirigido", "Salir"};
    int seleccion = 0;
    char tecla;
    
    while (true) {
        system("cls");
        cout << "Seleccione el tipo de grafo:\n";
        for (size_t i = 0; i < opciones.size(); i++) {
            if (i == seleccion)
                cout << "--> " << opciones[i] << "\n";
            else
                cout << "    " << opciones[i] << "\n";
        }
        tecla = _getch();
        if (tecla == 'w' && seleccion > 0)
            seleccion--;
        else if (tecla == 's' && seleccion < opciones.size() - 1)
            seleccion++;
        else if (tecla == 13) {
            // Limpiar el búfer de entrada después de presionar ENTER
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return seleccion + 1;
        }
    }
}

int main() {
    while (true) {
        int opcion = mostrarMenu();
        
        if (opcion == 3) {
            cout << "Saliendo del programa..." << endl;
            break;
        }
        
        int numNodos, numAristas;
        
        cout << "Ingrese la cantidad de nodos: ";
        cin >> numNodos;
        // Limpiar el búfer después de leer la cantidad de nodos
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        Grafo* grafo;
        if (opcion == 1) {
            grafo = new GrafoDirigido(numNodos);
        } else {
            grafo = new GrafoNoDirigido(numNodos);
        }
        
        cout << "Ingrese la cantidad de conexiones: ";
        cin >> numAristas;
        // Limpiar el búfer después de leer la cantidad de aristas
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (int i = 0; i < numAristas; i++) {
            string origen, destino;
            cout << "Ingrese el nodo origen: ";
            cin >> origen;
            cout << "Ingrese el nodo destino: ";
            cin >> destino;
            grafo->agregarArista(origen, destino);
        }
        
        grafo->mostrarRepresentacion();
        grafo->representarSFML(opcion == 1);
        
        delete grafo;
        
        cout << "Presione cualquier tecla para volver al menu..." << endl;
        _getch();
    }
    
    return 0;
}
