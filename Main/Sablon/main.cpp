#define CRES 30 
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>

//Biblioteke OpenGL-a
#include <GL/glew.h>   //Omogucava upotrebu OpenGL naredbi
#include <GLFW/glfw3.h>//Olaksava pravljenje i otvaranje prozora (konteksta) sa OpenGL sadrzajem
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <random>
using namespace std;

unsigned int compileShader(GLenum type, const char* source); //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
unsigned int createShader(const char* vsSource, const char* fsSource); //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource i Fragment sejdera na putanji fsSource
static unsigned loadImageToTexture(const char* filePath);
int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Pokretanje GLFW biblioteke
    // Nju koristimo za stvaranje okvira prozora
    if (!glfwInit()) // !0 == 1  | glfwInit inicijalizuje GLFW i vrati 1 ako je inicijalizovana uspjesno, a 0 ako nije
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    //Odredjivanje OpenGL verzije i profila (3.3, programabilni pajplajn)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Stvaranje prozora
    GLFWwindow* window; //Mjesto u memoriji za prozor
    unsigned int wWidth = 1050;
    unsigned int wHeight = 900;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); // Napravi novi prozor
    // glfwCreateWindow( sirina, visina, naslov, monitor na koji ovaj prozor ide preko citavog ekrana (u tom slucaju umjesto NULL ide glfwGetPrimaryMonitor() ), i prozori sa kojima ce dijeliti resurse )
    if (window == NULL) //Ako prozor nije napravljen
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); //Gasi GLFW
        return 2; //Vrati kod za gresku
    }
    // Postavljanje novopecenog prozora kao aktivni (sa kojim cemo da radimo)
    glfwMakeContextCurrent(window);

    // Inicijalizacija GLEW biblioteke
    if (glewInit() != GLEW_OK) //Slicno kao glfwInit. GLEW_OK je predefinisani kod za uspjesnu inicijalizaciju sadrzan unutar biblioteke
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag"); // Napravi objedinjeni sejder program

    //Vertex Array Object - Kontejner za VBO i pokazivace na njihove atribute
    unsigned int VAO[4];
    glGenVertexArrays(4, VAO);
    unsigned int VBO[4];
    glGenBuffers(4, VBO);


    float vertices[] = //Tjemena trougla koja sadrze sve informacije o njemu
    {
        //Podaci su poredani za nasu citkivost - racunar ne vidi ni razmake ni redove.
        //Moramo mu naknadno reci kako da intepretira ove podatke

        //Pozicija    |    Boja
        //X    Y       R    G    B    A
        -0.9, 0.0,   1.0, 1.0, 0.0, 1.0, 
        -0.9, 0.5,   1.0, 1.0, 0.0, 1.0,
        -0.4, 0.0,   1.0, 1.0, 0.0, 1.0, 
        -0.4, 0.5,   1.0, 1.0, 0.0, 1.0,

        0.1, 0.85,    1.0, 0.0, 0.0, 1.0,  
        0.7, 0.85,    1.0, 0.0, 0.0, 1.0,
        0.1, 0.35,    1.0, 0.0, 0.0, 1.0, 
        0.7, 0.35,    1.0, 0.0, 0.0, 1.0,

        -0.3, -0.9,  1.0, 0.0, 1.0, 1.0, 
        -0.9, -0.9,  1.0, 0.0, 1.0, 1.0,
        -0.3, -0.5,  1.0, 0.0, 1.0, 1.0, 
        -0.9, -0.5,  1.0, 0.0, 1.0, 1.0,

        0.2, -0.8,   0.0, 0.0, 1.0, 1.0, 
        0.7, -0.8,   0.0, 0.0, 1.0, 1.0, 
        0.2, -0.3,   0.0, 0.0, 1.0, 1.0,
        0.7, -0.3,   0.0, 0.0, 1.0, 1.0,

        
    };
    unsigned int stride = (2 + 4) * sizeof(float); //Korak pri kretanju po podacima o tjemenima = Koliko mjesta u memoriji izmedju istih komponenti susjednih tjemena
    //U nasem slucaju XY (2) + RGBA (4) = 6
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int circleShader = createShader("basic.vert", "circle.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");
    unsigned int progressShader = createShader("progress.vert", "progress.frag");


    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); 
    glEnableVertexAttribArray(1);


    float circle[CRES * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    float poluprecnik = 0.05; //poluprecnik

    circle[0] = 0; //Centar X0
    circle[1] = 0; //Centar Y0
    int i;
    for (i = 0; i <= CRES; i++)
    {

        circle[2 + 2 * i] = poluprecnik * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi
        circle[2 + 2 * i + 1] = poluprecnik * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    
    }


    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    float textureVertices[] =
    {   //X    Y      S    T 
        -0.8, 0.1,   1.0, 0.0, 
        -0.8, 0.4,   1.0, 1.0, 
        -0.5, 0.1,   0.0, 0.0, 
        -0.5, 0.4,   0.0, 1.0,

        0.15, 0.40,    1.0, 0.0,
        0.15, 0.8,    1.0, 1.0,
        0.65, 0.40,    0.0, 0.0,
        0.65, 0.8,    0.0, 1.0,


        -0.85, -0.85,  1.0, 0.0, 
        -0.85, -0.55,  1.0, 1.0,
        -0.35, -0.85,  0.0, 0.0,
        -0.35, -0.55,  0.0, 1.0,

        0.25, -0.75,   1.0, 0.0,
        0.25, -0.35,   1.0, 1.0,
        0.65, -0.75,   0.0, 0.0,
        0.65, -0.35,   0.0, 1.0,

        -0.1, -0.3,   0.0, 0.0,
        -0.1, -0.1,   0.0, 1.0,
        0.1, -0.3,   1.0, 0.0,
        0.1, -0.1,   1.0, 1.0,

    };
    unsigned int textureStride = (2 + 2) * sizeof(float);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);

    // Assuming the order is X, Y, S, T
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);




    float verticesProgressBar[] =
    {
        -1.0, -1.0,
        -1.0, -0.95,
        1.0, -0.95,
        1.0, -0.95,
        1.0, -1.0,
        -1.0, -1.0
    };
    unsigned int progressStride = 2 * sizeof(float);
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesProgressBar), verticesProgressBar, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, progressStride, (void*)0);
    glEnableVertexAttribArray(0);



    glBindVertexArray(0);
    
    //Tekstura mona lise
    unsigned monalisaTexture = loadImageToTexture("monaLisa.jpg");
    glBindTexture(GL_TEXTURE_2D, monalisaTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Tekstura meduse
    unsigned medusaTexture = loadImageToTexture("splav.png");
    glBindTexture(GL_TEXTURE_2D, medusaTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Tekstura sloboda predvodi narod
    unsigned slobodaTexture = loadImageToTexture("sloboda.jpg");
    glBindTexture(GL_TEXTURE_2D, slobodaTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Tekstura napoleon
    unsigned napoleonTexture = loadImageToTexture("napoleon.jpg");
    glBindTexture(GL_TEXTURE_2D, napoleonTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Tekstura ime i prezime
    unsigned imeprezimeTexture = loadImageToTexture("imeprezime.png");
    glBindTexture(GL_TEXTURE_2D, imeprezimeTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned uTexLoc = glGetUniformLocation(textShader, "uTex");
    glUniform1i(uTexLoc, 0);







    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++
    unsigned int uPosLoc = glGetUniformLocation(unifiedShader, "uPos");
    unsigned int tPosLoc = glGetUniformLocation(textShader, "uPos");
    float r = 0.05;
    float r2 = 0.1;
    float r3 = 0.03;
    float r4 = 0.08;
    float rotationSpeed = 0.7;
    float rotationSpeed2 = 1.9;
    float rotationSpeed3 = 6.9;
    float rotationSpeed4 = 0.1;
    bool changeCircleColor = false;
    bool rotate = true;
    float pb = 0;

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) //Beskonacna petlja iz koje izlazimo tek kada prozor treba da se zatvori
    {
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        // [KOD ZA CRTANJE]
        glUseProgram(unifiedShader); 
        glUseProgram(basicShader);
        glBindVertexArray(VAO[0]);
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - startTime;

        //Proemni boju rama na svake 3 sekunde
        float timeVar = elapsedTime * 2.0;
        float timeColorRed = 0.5 + 0.5 * sin(timeVar);
        float timeColorGreen = 0.5 + 0.5 * sin(timeVar + 2.0);
        float timeColorBlue = 0.5 + 0.5 * sin(timeVar + 4.0);

        if (elapsedTime > 3) {
            glUniform1f(glGetUniformLocation(basicShader, "timeColorRed"), timeColorRed);
            glUniform1f(glGetUniformLocation(basicShader, "timeColorGreen"), timeColorGreen);
            glUniform1f(glGetUniformLocation(basicShader, "timeColorBlue"), timeColorBlue);
        }

        

        if (rotate) {
            glUniform1i(glGetUniformLocation(basicShader, "rotate"), 1);
            glUniform2f(uPosLoc, r * cos(glfwGetTime() * rotationSpeed), r * sin(glfwGetTime() * rotationSpeed));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
            glUniform2f(uPosLoc, r2 * cos(glfwGetTime() * rotationSpeed2), r2 * sin(glfwGetTime() * rotationSpeed2));
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4); 
            glUniform2f(uPosLoc, r3 * cos(glfwGetTime() * rotationSpeed3), r3 * sin(glfwGetTime() * rotationSpeed3));
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4); 
            glUniform2f(uPosLoc, r4 * cos(glfwGetTime() * rotationSpeed4), r4 * sin(glfwGetTime() * rotationSpeed4));
            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); 


            
        }
        else {
            glUniform1i(glGetUniformLocation(basicShader, "rotate"), 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4); 
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4); 
            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4); 
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        //Dugme
        glUseProgram(circleShader);
        glBindVertexArray(VAO[1]);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            changeCircleColor = true;
            rotate = false;
            
        }
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            changeCircleColor = false;
            rotate = true;
           
        }
        if (changeCircleColor) {
            glClearColor(0.3, 0.3, 0.3, 1.0);
            glUniform4f(glGetUniformLocation(circleShader, "circleColor"), 0.0, 0.0, 0.0, 1.0);
        }
        else {
            glClearColor(0.7, 0.7, 0.7, 1.0);
            glUniform4f(glGetUniformLocation(circleShader, "circleColor"), 1.0, 1.0, 0.0, 1.0);
        }
        glUniform2f(uPosLoc, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));




        //Teksture
        glUseProgram(textShader);
        glBindVertexArray(VAO[2]);


        glBindTexture(GL_TEXTURE_2D, imeprezimeTexture);


        glUniform2f(tPosLoc, 0, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE0);
        if (rotate) {
            glUniform1i(glGetUniformLocation(textShader, "rotation"), 1);


            glBindTexture(GL_TEXTURE_2D, monalisaTexture);


            glUniform2f(tPosLoc, r * cos(glfwGetTime() * rotationSpeed), r * sin(glfwGetTime() * rotationSpeed));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, medusaTexture);


            glUniform2f(tPosLoc, r2 * cos(glfwGetTime() * rotationSpeed2), r2 * sin(glfwGetTime() * rotationSpeed2));
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, slobodaTexture);


            glUniform2f(tPosLoc, r3 * cos(glfwGetTime() * rotationSpeed3), r3 * sin(glfwGetTime() * rotationSpeed3));
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
            glBindTexture(GL_TEXTURE_2D, 0);


            glBindTexture(GL_TEXTURE_2D, napoleonTexture);


            glUniform2f(tPosLoc, r4 * cos(glfwGetTime() * rotationSpeed4), r4 * sin(glfwGetTime() * rotationSpeed4));
            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            glUniform1i(glGetUniformLocation(textShader, "rotation"), 0);

            glBindTexture(GL_TEXTURE_2D, monalisaTexture);


            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, medusaTexture);


            
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, slobodaTexture);


           
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
            glBindTexture(GL_TEXTURE_2D, 0);


            glBindTexture(GL_TEXTURE_2D, napoleonTexture);


            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //Proggress bar
        glUseProgram(progressShader);
        glBindVertexArray(VAO[3]);
        glUniform1f(glGetUniformLocation(progressShader, "barColor"), pb);
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            if (pb < 1050) {
                pb += 1;
                rotationSpeed += 0.01;
                rotationSpeed2 += 0.01;
                rotationSpeed3 += 0.01;
                rotationSpeed4 += 0.01;
            }


        }
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            if (pb > 0) {
                pb -= 1;
                rotationSpeed -= 0.01;
                rotationSpeed2 -= 0.01;
                rotationSpeed3 -= 0.01;
                rotationSpeed4 -= 0.01;
            }

        }


        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++


    glDeleteBuffers(4, VBO);
    glDeleteVertexArrays(4, VAO);
    glDeleteProgram(unifiedShader);
    glfwTerminate();
    return 0;
}




unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) 
{
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
