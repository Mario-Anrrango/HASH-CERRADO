#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

const int TABLE_SIZE = 29;
vector<int> hashTable(TABLE_SIZE, -1);
int collisionMethod = 1; // 1 = Linear, 2 = Quadratic, 3 = Double Hash

// Función de hash primaria
int hashFunction(int key) {
    return key % TABLE_SIZE;
}

// Segunda función de hash para doble hashing
int hash2(int key) {
    return 7 - (key % 7); // Un número primo menor a TABLE_SIZE
}

// Métodos de resolución de colisiones
int linearProbing(int index, int i) {
    return (index + i) % TABLE_SIZE;
}

int quadraticProbing(int index, int i) {
    return (index + i * i) % TABLE_SIZE;
}

int doubleHashing(int index, int key, int i) {
    return (index + i * hash2(key)) % TABLE_SIZE;
}

// Insertar elemento en la tabla según el método seleccionado
void insert(int key) {
    int index = hashFunction(key);
    int i = 0;
    while (hashTable[(collisionMethod == 1 ? linearProbing(index, i) : 
                     collisionMethod == 2 ? quadraticProbing(index, i) : 
                     doubleHashing(index, key, i))] != -1) {
        i++;
        if (i == TABLE_SIZE) return; // Evitar bucles infinitos
    }
    hashTable[(collisionMethod == 1 ? linearProbing(index, i) : 
               collisionMethod == 2 ? quadraticProbing(index, i) : 
               doubleHashing(index, key, i))] = key;
}

// Reiniciar la tabla
void resetTable() {
    fill(hashTable.begin(), hashTable.end(), -1);
}

// Dibujar la tabla hash con botones
void drawTable(RenderWindow &window, Font &font) {
    int columns = 10; // Ajuste para mejor alineación
    for (int i = 0; i < TABLE_SIZE; i++) {
        RectangleShape cell(Vector2f(70, 60)); // Tamaño del rectángulo aumentado para mejor separación
        cell.setPosition(20 + (i % columns) * 90, 100 + (i / columns) * 90); // Ajuste de separación
        cell.setOutlineThickness(2);
        cell.setOutlineColor(Color::Black);
        cell.setFillColor(Color::White);
        window.draw(cell);

        Text indexText(to_string(i), font, 14);
        indexText.setPosition(cell.getPosition().x + 25, cell.getPosition().y + 70); // Ajustado para mejor visibilidad
        indexText.setFillColor(Color::Blue);
        window.draw(indexText);

        if (hashTable[i] != -1) {
            Text text(to_string(hashTable[i]), font, 20);
            text.setPosition(cell.getPosition().x + 20, cell.getPosition().y + 15);
            text.setFillColor(Color::Black);
            window.draw(text);
        }
    }
}

int main() {
    RenderWindow window(VideoMode(1300, 700), "Closed Hashing Visualization"); // Aumentado el tamaño de la ventana
    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/Ebrima.ttf")) {
        cerr << "Error cargando fuente\n";
        return -1;
    }

    Text inputText("", font, 20);
    inputText.setPosition(20, 20);
    inputText.setFillColor(Color::Black);
    string inputValue = "";

    // Definir botones
    vector<pair<RectangleShape, string>> buttons;
    vector<Color> buttonColors = {Color::Yellow, Color::Yellow, Color::Yellow, Color::Yellow};
    vector<string> buttonLabels = {"Reset", "F(i) =i", "F(i)= i*i", "F(i) = i*hash2(key)"};
    int buttonX = 1000, buttonY = 100, buttonWidth = 190, buttonHeight = 70, spacing = 10;
    
    for (int i = 0; i < 4; i++) {
        RectangleShape button(Vector2f(buttonWidth, buttonHeight));
        button.setPosition(buttonX, buttonY + i * (buttonHeight + spacing));
        button.setFillColor(buttonColors[i]);
        buttons.push_back({button, buttonLabels[i]});
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            
            if (event.type == Event::TextEntered) {
                if (event.text.unicode >= '0' && event.text.unicode <= '9')
                    inputValue += static_cast<char>(event.text.unicode);
                else if (event.text.unicode == 8 && !inputValue.empty())
                    inputValue.pop_back();
            }
            
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter && !inputValue.empty()) {
                insert(stoi(inputValue));
                inputValue = "";
            }
            
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                for (int i = 0; i < buttons.size(); i++) {
                    if (buttons[i].first.getGlobalBounds().contains(mousePos)) {
                        resetTable(); 
                        if (i == 0) {
                            for (int j = 1; j < buttons.size(); j++) {
                                buttons[j].first.setFillColor(buttonColors[j]);
                            }
                        } else {
                            collisionMethod = i;
                            for (int j = 1; j < buttons.size(); j++) {
                                buttons[j].first.setFillColor(j == i ? Color::Green : buttonColors[j]);
                            }
                        }
                    }
                }
            }
        }

        window.clear(Color::White);
        drawTable(window, font);
        inputText.setString("Input: " + inputValue);
        window.draw(inputText);
        
        for (auto &button : buttons) {
            window.draw(button.first);
            Text buttonText(button.second, font, 20);
            buttonText.setPosition(button.first.getPosition().x + 20, button.first.getPosition().y + 15);
            buttonText.setFillColor(Color::Black);
            window.draw(buttonText);
        }
        
        window.display();
    }
    return 0;
}