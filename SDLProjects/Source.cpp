#include <SDL.h>
#include <iostream>

using namespace std;

// classe Vector3 para facilitar as operações com vetores
class Vector3 {
public:
    double x;
    double y;
    double z;

    Vector3() : x(0.0), y(0.0), z(0.0) {}
    Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    // Operação de soma
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Operação de subtração
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Operação de produto escalar
    double operator*(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};

// Função para testar a interseção de uma reta com uma esfera
bool sphere_ray_intersect(Vector3 ray_start, Vector3 ray_direction, Vector3 sphere_center, double sphere_radius) {
    Vector3 s = ray_start - sphere_center;
    double b = s * ray_direction;
    double c = (s * s) - sphere_radius - sphere_radius;
    double h = b * b - c;
    if (h < 0.0) {
        return false;
    }
    return true;
}

const int screenWidth = 800;
const int screenHeight = 800;

// Dimensões da janela em metros
const double wJanela = 1;
const double hJanela = 1;
const double dJanela = 0.5;

// Coordenadas e raio da esfera
const double rEsfera = 0.5;
const double xEsfera = 0;
const double yEsfera = 0;
const double zEsfera = -(dJanela + 3 * rEsfera);
const Vector3 sphere_center(xEsfera, yEsfera, zEsfera);

// Cores esfera e fundo
const SDL_Color esfColor = { 255, 0, 0, 255 };
const SDL_Color bgColor = { 100, 100, 100, 255 };

// Número de linhas e colunas para a matriz de cores
const int nCol = 400;
const int nLin = 400;

int main(int argc, char* argv[]) {

    // Inicializar a biblioteca para poder usar suas funções
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Não foi possível inicializar o SDL! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    // Criar uma janela
    SDL_Window* window = SDL_CreateWindow(
        "CG I - Raycaster",                 // Título da Janela
        SDL_WINDOWPOS_UNDEFINED,            // Posição inicial X
        SDL_WINDOWPOS_UNDEFINED,            // Posição inicial Y
        screenWidth,                        // Largura da janela em pixels
        screenHeight,                       // Altura da janela em pixels
        SDL_WINDOW_SHOWN                    // Flags
    );

    // Verificar se a janela foi criada corretamente
    if (!window) {
        SDL_Log("Criação da janela falhou! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Criar o objeto renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Criação do renderer falhou! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop - É dentro do main while loop que fazemos todas as interações com a janela
    bool isRunning = true; //variável de controle do loop
    SDL_Event event; //variável para checar os eventos da janela

    while (isRunning) {
        // É possível interagir com a janela, com clicks, apertando teclas ou clicando para fechar a janela
        // essas interações são chamadas de "eventos", abaixo, verificamos se o evento de fechar janela ocorre
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // É uma boa prática limpar o renderer antes de desenhar novos pixeis, no caso de existir "lixo" no renderer
        SDL_RenderClear(renderer);

        // Matriz de cores da imagem
        SDL_Color colors[nLin][nCol];

        // Dimensões dos retângulos da tela de mosquito
        double Dx = wJanela / nCol;
        double Dy = hJanela / nLin;

        // para cada retângulo...
        for (int l = 0; l < nLin; l++) {
            for (int c = 0; c < nCol; c++) {

                // Coordenadas do ponto central do retângulo
                double x = -wJanela / 2.0 + Dx / 2.0 + c * Dx;
                double y = hJanela / 2.0 - Dy / 2.0 - l * Dy;

                // coordenadas observador
                Vector3 ray_start(0, 0, 0);

                // vetor de direção da reta;
                Vector3 ray_direction(x, y, -dJanela);

                // Normalizar o vetor de direção do raio
                double magnitude = sqrt(ray_direction.x * ray_direction.x +
                    ray_direction.y * ray_direction.y +
                    ray_direction.z * ray_direction.z);

                Vector3 normalized_direction(ray_direction.x / magnitude,
                    ray_direction.y / magnitude,
                    ray_direction.z / magnitude);

                // testa intersecao
                bool intersecao = sphere_ray_intersect(ray_start, normalized_direction, sphere_center, rEsfera);

                // se tem intersecao, entao armazena a cor da esfera na matriz de cores
                if (intersecao == true) {
                    colors[l][c] = esfColor; // Pintar com a cor da esfera
                }
                else {
                    colors[l][c] = bgColor; // Pintar com a cor de background
                }
            }
        }

        // Razões entre resolução da tela e da matriz de cores
        // usado para saber a quantidade de pixels que cada retangulo vai ocupar na imagem final
        double Rx = screenWidth / nCol;
        double Ry = screenHeight / nLin;

        // Pintura da tela
        for (int l = 0; l < nLin; l++) {
            for (int c = 0; c < nCol; c++) {
                SDL_SetRenderDrawColor(renderer, colors[l][c].r, colors[l][c].g, colors[l][c].b, colors[l][c].a);
                for (int x = c * Rx; x < (c + 1) * Rx; x++) {
                    for (int y = l * Ry; y < (l + 1) * Ry; y++) {
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            }
        }

        // Por fim, atualizamos a janela com o renderer que acabamos de pintar e tudo deve funcionar corretamente
        SDL_RenderPresent(renderer);
    }

    // Destruir os objetos criados para limpar a memória
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}