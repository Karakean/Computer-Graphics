#define WF_NUM 5 //wall numbers + floor + triangle
#define TEX_NUM 4
#define VBO_NUM 2*WF_NUM+WF_NUM-1
#define GL3_PROTOTYPES 1
#include <iostream>
#include <GLEW.h>
#include <SDL.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define WIDTH 800
#define HEIGHT 600

float current_angle;

//------------------------------------------------------------------------------------------------------------------------------
//Globalne zmienne 
bool collision = false;
// kod zrodlowy shadera wierzcholkow
const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
"in vec3 color;"
"in vec2 texcoord;"
"out vec3 Color;"
"out vec2 Texcoord;"
"uniform mat4 transformMatrix;"
"void main()"
"{"
"    gl_Position =  transformMatrix * vec4(position, 1.0);"
"    Color = color;"
"	 Texcoord = texcoord;"
"}";

// kod zrodlowy shadera fragmentow
const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 Color;"
"in vec2 Texcoord;"
"out vec4 outColor;"
"uniform sampler2D tex;"
"void main()"
"{"
"    outColor = texture(tex, Texcoord) * vec4(Color, 1.0);"
"}";


//------------------------------------------------------------------------------------------------------------------------------

GLint posAttrib, colAttrib, texAttrib;					//wskazniki atrybutow wierzcholkow
GLuint vertexShader, fragmentShader, shaderProgram;		//shadery


GLuint vao[WF_NUM], vbo[VBO_NUM], ebo, tex[TEX_NUM];	// identyfikatory poszczegolnych obiektow (obiekty tablic wierzcholkow, buforow wierzcholkow, elementow, tekstury)

//------------------------------------------------------------------------------------------------------------------------------


GLfloat ver_floor[] = { //wspolrzedne wierzcholkow podlogi
	-5.0f,  0.0f, -5.0f,
	5.0f,  0.0f, -5.0f,
	5.0f,  0.0f,  5.0f,
	-5.0f,  0.0f,  5.0f,
};

GLfloat col_floor[] = { //kolory wierzcholkow podlogi
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

GLfloat ver_wall1[] = {
	-4.0f, 0.0f,-5.0f,
	-4.0f, 2.0f,-5.0f,
	1.0f, 2.0f,-5.0f,
	1.0f, 0.0f,-5.0f,
};

GLfloat ver_wall2[] = {
	1.0f, 0.0f,-5.0f,
	1.0f, 2.0f,-5.0f,
	1.0f, 2.0f,0.0f,
	1.0f, 0.0f,0.0f,
};

GLfloat ver_wall3[] = {
	-4.0f, 0.0f,-5.0f,
	-4.0f, 2.0f,-5.0f,
	-4.0f, 2.0f,0.0f,
	-4.0f, 0.0f,0.0f,
};
// punkty do sprawdzania zderzenia 
GLfloat ver_walls_col[] = {
	-4.0f, 0.0f,-5.0f,
	1.0f, 0.0f,-5.0f,
	1.0f, 0.0f,-5.0f,
	1.0f, 0.0f,0.0f,
	-4.0f, 0.0f,-5.0f,
	-4.0f, 0.0f,0.0f,
};


GLfloat col_wall[] = {
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

GLfloat tex_floor[] = {	//wspolrzedne tekstury dla podlogi, 
						//jesli wykraczaja poza przedzial <0,1> nastapi nakladanie zalezne od parametrow ustawionych za pomoca funkcji glTexParameteri, np. GL_REPEAT
	0.0f, 0.0f,	// lewy dolny rog tekstury
	5.0f, 0.0f,
	5.0f, 5.0f,
	0.0f, 5.0f, // prawy gorny rog tekstury
};

GLfloat tex_wall1[] = {

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
};

GLfloat tex_wall2[] = {

	0.0f, 0.0f,
	1.5f, 0.0f,
	5.0f, 0.5f,
	0.0f, 1.5f,
};

GLfloat tex_wall3[] = {

	0.0f, 0.0f,
	3.25f, 0.0f,
	1.25f, 2.25f,
	0.0f, 3.25f,
};

GLuint elements[] = { // pogrupowanie wierzcholkow w trojkaty, wykorzystane zarowno dla sciany jaki dla podlogi
	0,1,2,
	2,3,0,
};

float pixels_floor[] = { //tekstura o wymiarach 2x2; dla kazdego punktu okreslone skladowe RGB koloru 
	1.0f, 1.0f, 1.0f,   0.1f, 0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,   1.0f, 1.0f, 1.0f,
};

float pixels_wall[] = {
	1.0f, 0.0f, 0.0f,   0.5f, 0.0f, 0.0f,   0.25f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,   0.0f, 0.5f, 0.0f,   0.0f, 0.25f, 0.0f,
	0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.5f,    0.0f, 0.0f, 0.25f,
	1.0f, 0.0f, 1.0f,  0.5f, 0.0f, 0.5f,    0.25f, 0.0f, 0.25f,
};

GLfloat ver_triangle[] = { //wspolrzedne wierzcholkow trojkata okreslajacego polozenie obserwatora (kamery)
	-0.2f,  0.0f, 0.0f,
	 0.2f,  0.0f, 0.0f,
	 0.0f,  0.0f, -1.0f,
};

GLfloat col_triangle[] = { //kolory wierzcholkow trojkata okreslajacego polozenie obserwatora (kamery)
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
};

bool between(float x, float x1, float x2) {
	float width = 1.0f;
	if (x2 > x1) {
		float temp = x2;
		x2 = x1;
		x1 = temp;
	}

	if (x >= x2 - width && x <= x1 + width)
		return true;
	return false;

}

bool will_colide(glm::vec3 newPosition) {

	for (int i = 0;i < 3; i++) {
		float x1, z1, x2, z2;
		x1 = ver_walls_col[i * 6];
		z1 = ver_walls_col[i * 6+2];

		x2 = ver_walls_col[i * 6+3];
		z2 = ver_walls_col[i * 6 + 5];
		if (between(newPosition.x, x1, x2) && between(newPosition.z, z1, z2)) {
			printf("kolizja!");
			return true;
		}
	}
	return false;

}
//------------------------------------------------------------------------------------------------------------------------------
void move_arrow(float x, float z, float angle) {
	//printf("(%f, %f)\n", x, z);
	float dx, dz;
	dx = x - ver_triangle[0];
	dz = z - ver_triangle[2];
	for (int i = 0; i < 3; i++) {
		ver_triangle[0 + i * 3] += dx; // x
		ver_triangle[2 + i * 3] += dz; // z
	}



	float newangle = angle - current_angle;
	current_angle = angle;
	// trojkat vao[1]

	// srodek trojkata 
	GLfloat cx = (ver_triangle[0] + ver_triangle[3] + ver_triangle[6]) / 3;
	GLfloat cz = (ver_triangle[2] + ver_triangle[5] + ver_triangle[8]) / 3;
	printf("(%f, %f)\n", cx, cz);
	float px, pz;
	for (int i = 0; i < 3; i++) {
		px = ver_triangle[0 + i * 3];
		pz = ver_triangle[2 + i * 3];

		//printf("%d : %f %f \n", i+1, dx, dz);
		ver_triangle[0 + i * 3] = cos(newangle) * (px - cx) - sin(newangle) * (pz - cz) + cx; // x
		ver_triangle[2 + i * 3] = sin(newangle) * (px - cx) + cos(newangle) * (pz - cz) + cz; // z
	}


	glBindVertexArray(vao[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);	// bufor wspolrzednych wierzcholkow trojkata
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_triangle), ver_triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);	// bufor kolorow wierzcholkow trojkata
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_triangle), col_triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//glBindVertexArray(vao[1]);
	//glDisable(GL_TEXTURE_2D);
	//glDrawArrays(GL_TRIANGLES, 0, 3);	//rysujemy trojkat przedstawiajacy polozenie kamery
}

int init_shaders()
{
	// tworzenie i kompilacja shadera wierzcholkow
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cout << "Kompilacja shadera wierzcholkow NIE powiodla sie!\n";
		return 0;
	}

	// tworzenie i kompilacja shadera fragmentow
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cout << "Kompilacja shadera fragmentow NIE powiodla sie!\n";
		return 0;
	}

	// dolaczenie programow przetwarzajacych wierzcholki i fragmenty do programu cieniujacego
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// wskazniki atrybutow wierzcholkow
	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);

	return 1;

}

//------------------------------------------------------------------------------------------------------------------------------

void create_objects()
{
	// generowanie obiektow
	glGenVertexArrays(WF_NUM, vao);  // obiekt tablicy wierzcholkow, dla kazdego obiektu (np. dla podlogi) mamy jedna tablice
	glGenBuffers(VBO_NUM, vbo);		// obiekty buforow wierzcholkow, dla kazdego typu atrubutow kazdego obiektu mamy jeden bufor (np. bufor dla kolorow podlogi, bufor dla wspolrzednych podlogi itd.)
	glGenBuffers(1, &ebo);		// obiekt bufora elementow (ten sam bufor mozna wykorzystac zarowno dla podlogi jak i sciany)

	// podloga vao[0]

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	// bufor wspolrzednych wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_floor), ver_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);	// bufor kolorow wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_floor), col_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);	// bufor wspolrzednych tekstury podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_floor), tex_floor, GL_STATIC_DRAW);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// trojkat vao[1]

	glBindVertexArray(vao[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);	// bufor wspolrzednych wierzcholkow trojkata
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_triangle), ver_triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);	// bufor kolorow wierzcholkow trojkata
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_triangle), col_triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);


	glBindVertexArray(vao[2]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);	// bufor wspolrzednych wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_wall1), ver_wall1, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);	// bufor kolorow wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_wall), col_wall, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);	// bufor wspolrzednych tekstury podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_wall1), tex_wall1, GL_STATIC_DRAW);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


	glBindVertexArray(vao[3]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);	// bufor wspolrzednych wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_wall2), ver_wall2, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);	// bufor kolorow wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_wall), col_wall, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);	// bufor wspolrzednych tekstury podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_wall2), tex_wall2, GL_STATIC_DRAW);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


	glBindVertexArray(vao[4]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);	// bufor wspolrzednych wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(ver_wall3), ver_wall3, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);	// bufor kolorow wierzcholkow podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(col_wall), col_wall, GL_STATIC_DRAW);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);	// bufor wspolrzednych tekstury podlogi
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_wall3), tex_wall3, GL_STATIC_DRAW);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texAttrib);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

//------------------------------------------------------------------------------------------------------------------------------

void configure_texture()
{
	glGenTextures(1, tex);		// obiekt tekstury
	glBindTexture(GL_TEXTURE_2D, tex[0]);		// powiazanie tekstury z obiektem (wybor tekstury)

	// ustawienia parametrow tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// sposob nakladania tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // sposob filtrowania tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels_floor); // ladowanie do tekstury tablicy pikseli

	for (int i = 1; i < TEX_NUM; i++) {
		glGenTextures(1, &tex[i]);		// obiekt tekstury
		glBindTexture(GL_TEXTURE_2D, tex[i]);		// powiazanie tekstury z obiektem (wybor tekstury)

		// ustawienia parametrow tekstury
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// sposob nakladania tekstury
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // sposob filtrowania tekstury
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 3, 3, 0, GL_RGB, GL_FLOAT, pixels_wall); // ladowanie do tekstury tablicy pikseli
	}
}

//------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_Event windowEvent;

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "ERROR" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // GL_ALWAYS)

	if (!init_shaders())
		return 0;

	create_objects();

	configure_texture();


	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 20.0f);		 //macierz rzutowania perspektywicznego
	glm::mat4 viewMatrix;  //macierz widoku
	glm::mat4 transformMatrix; //macierz wynikowa

	GLint transformMatrixUniformLocation = glGetUniformLocation(shaderProgram, "transformMatrix");

	int top_view = false; //zmienna okreslajaca czy patrzymy na scene z gory

	glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f); //poczatkowe polozenie kamery
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f); //poczatkowy kierunek, w ktorym kamera jest skierowana

	float angle = 4.71f;
	current_angle = angle;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_KEYUP &&
				windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
			if (windowEvent.type == SDL_QUIT) break;


			if (windowEvent.type == SDL_KEYDOWN)
			{
				switch (windowEvent.key.keysym.sym)
				{
				case SDLK_SPACE:
					top_view = !top_view;
					break;

				case SDLK_UP:
					if (!will_colide(position+direction * glm::vec3(0.1f, 0.1f, 0.1f)))
						position += direction * glm::vec3(0.1f, 0.1f, 0.1f);
					break;
				case SDLK_DOWN:
					if (!will_colide(position - direction * glm::vec3(0.1f, 0.1f, 0.1f)))
						position -= direction * glm::vec3(0.1f, 0.1f, 0.1f);
					break;
				case SDLK_LEFT:
					//direction += glm::vec3(-0.1f, 0.0f, 0.0f);
					angle -= 0.05f;
					direction = glm::vec3(cos(angle), 0.0f, sin(angle));
					break;
				case SDLK_RIGHT:
					//direction += glm::vec3(0.1f, 0.0f, 0.0f);
					angle += 0.05f;
					direction = glm::vec3(cos(angle), 0.0f, sin(angle));
					break;
				}

			}

		}

		if (top_view) {//patrzymy z gory
			viewMatrix = glm::lookAt(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			move_arrow(position.x + direction.x, position.z + direction.z, angle);
		}
		else //patrzymy z miejsca, w ktorym jest obserwator 
			viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));


		transformMatrix = projectionMatrix * viewMatrix;				// wynikowa macierz transformacji
		glUniformMatrix4fv(transformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));	// macierz jako wejściowa zmienna dla shadera wierzcholkow


		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);	// szare tlo
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao[0]);
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	//rysujemy podloge

		glBindVertexArray(vao[1]);
		glDisable(GL_TEXTURE_2D);
		glDrawArrays(GL_TRIANGLES, 0, 3);	//rysujemy trojkat przedstawiajacy polozenie kamery

		glBindVertexArray(vao[2]);
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(vao[3]);
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(vao[4]);
		glBindTexture(GL_TEXTURE_2D, tex[3]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(VBO_NUM, vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteTextures(TEX_NUM, tex);
	glDeleteVertexArrays(WF_NUM, vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}