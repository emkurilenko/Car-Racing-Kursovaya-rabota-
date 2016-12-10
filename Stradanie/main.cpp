#include <iostream>
#include <fstream>
#include <cstdlib>
#include "include\GL\glut.h"
#include "include\GL\GLAux.h"
#include "include\GL\FreeImage.h"
#include "include\SDL\SDL.h"
#include "include\SDL\SDL_mixer.h"
#include <cmath>
#include <ctime>
#include <mmsystem.h>
#include <Windows.h>

#pragma comment(lib, "Glaux.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "FreeImage.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2test.lib")

GLuint textures[14];

using namespace std;

struct Player {
	int score;
	int distance;
};

int WinWidth = 900, WinHeight = 660;
bool Up = false, Down = false, Right = false, Left = false, collide = false,failsound = false,collidewithCarSound=false, chooseGame=false, addTableRecord = false;
bool homeMainResum = false, menuSelectColorbool = false, randomchikforznak=true;
bool firstStrip = true, secondtStrip = true, thirdStrip = true, forthtStrip = true;
bool bonus = false,checkrandombonus=true, downDistance=false; // все зависит от рандома, получишь бонус или нет?
int movddline = 0;
int randomchikEcheRaz = 0, randomchikDlyaBonusa = 0, randombonus=0;
int _50vs100 = 0;
int distanceCar = 0;
float mCarX = 234.0, mCarY = 10.0; 
float bCarX[4] = { 234,234,350,350 }, bCarY[4] = {710};
float bonusY = 700, bonusX = 350;
float speed = 2.5;
float decc = 0.3, turnSpeed = 0.08;
float xTriangle = 225.0;
int random = 0, score = 0;
int positionMenu = 2;
int selectColor = 1;
int bufDistanceCar = 0;
float znackX = 70;
Player *playArray;

char buffCar[20]="pics/whitecolor.png";
char buffRoad[20] = "pics/road.jpg";
char buffBonus[20] = "pics/50bonus.png";

Mix_Music *music = NULL;
Mix_Chunk *fail = NULL;
Mix_Chunk *chooisesound = NULL;
Mix_Chunk *chooiseEnter = NULL;
Mix_Chunk *collidewithCar = NULL;
Mix_Chunk *bonussound = NULL;

void MainDraw();
void loadfile(Player *array);
void savefile(Player *array);
void sortBub(Player *array);
void mainBackground();//fon
void drawCar();//draw machine
void moveCar();
void specialKeyboardUp(int key, int x, int y);
void specialKeyboard(int key, int x, int y);
void processNormalKeys(unsigned char key, int x, int y);
void LoadTexture(char *filename, int pos);
void Initialize();
void roadDraw();
void gameOverBackground(float x, float y);
void drawTwenty(float XznT, float YznT);
void drawTextNum(char ch[], int numtext, int xpos, int ypos);
void drawText(char ch[], int xpos, int ypos);
void otherPositionBarrier();
void randomBarrier();
void drawBarrier(float bCarX, float bCarY);
void gamePlay();
void restartgameplay();
void menuSelectColor();
void selectTriangle();
void drawBonusScore(float, float);
void bonusScore();
void topfivebackground(float, float);
void addtableRecord(int score, int dist, Player *arrayP);
void freeInitialise();


void Timer(int = 0) {
	MainDraw();
	glutTimerFunc(90, Timer, 0);
}

int main(int argc, char**argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WinWidth, WinHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Car Racing");
	Initialize();
	glutDisplayFunc(MainDraw);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardUp);
	glutKeyboardFunc(processNormalKeys);
	glutTimerFunc(100, Timer, 0);
	glutMainLoop();
	return 0;
}

void Initialize()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WinWidth, 0, WinHeight, -320, 320);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_ALPHA_TEST);
	glGenTextures(14, textures);
	LoadTexture(buffCar, 0);
	LoadTexture("pics/sand.jpg", 1);
	LoadTexture("pics/greenCar.png", 2);
	LoadTexture(buffRoad, 3);
	LoadTexture("pics/mainbackground.png", 4);
	LoadTexture("pics/Newgame.png", 5);
	LoadTexture("pics/Exit.png", 6);
	LoadTexture("pics/Resume.png", 7);
	LoadTexture("pics/selectColor.png", 8);
	LoadTexture("pics/triangle.png", 9);
	LoadTexture("pics/zn20.png", 10);
	LoadTexture(buffBonus, 11);
	LoadTexture("pics/gameover.png", 12);
	LoadTexture("pics/topfive.png", 13);
	srand(time(NULL));
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);
	//music = Mix_LoadMUS("sound/t1.wav");
	fail = Mix_LoadWAV("sound/fail.wav");
	chooisesound = Mix_LoadWAV("sound/chooise.wav");
	collidewithCar = Mix_LoadWAV("sound/SOUND_REVERB_CAR_SCREECH_28000.wav");
	chooiseEnter = Mix_LoadWAV("sound/chooiseEnter.wav");
	bonussound = Mix_LoadWAV("sound/bonus.wav");
	playArray= new Player[5];
	loadfile(playArray);
	for (int i(0); i < 5; i++) {
		cout << "score[" << i << "]: " << playArray[i].score << endl;
		cout << "dist[" << i << "]: " << playArray[i].distance << endl;
		cout << endl;
	}
}

void sortBub(Player *array) {
	Player temp;
	for (int i(0); i < 5; i++) {
		for (int j(0); j < 4; j++) {
			if (array[j].score < array[j + 1].score) {
				temp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
		}
	}
}

void loadfile(Player *array) {
	ifstream fin;
	fin.open("result.txt");
	if (!fin.is_open()) {
		cout << "Error" << endl;
		return;
	}
	int temp;
	int i = 0, j = 1;
	while (!fin.eof()) {
		fin >> temp;
		if (j % 2 != 0) {
			array[i].score = temp;
		}
		if (j % 2 == 0) {
			array[i].distance = temp;
			i++;
		}
		j++;


	}
	fin.close();
}

void savefile(Player *array) {
	ofstream fout;
	fout.open("result.txt");
	if (!fout.is_open()) {
		cout << "Error" << endl;
		return;
	}
	for (int i(0); i < 5; i++) {
		fout << array[i].score << endl;
		fout << array[i].distance << endl;
	}
	fout.close();
}

void addtableRecord(int score, int dist, Player *arrayP) {
	if (score > arrayP[4].score) {
		arrayP[4].score = score;
		arrayP[4].distance = dist;
	}
	sortBub(arrayP);
	savefile(arrayP);
}

void LoadTexture(char *filename, int pos)
{
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filename, 0);//Automatocally detects the format(from over 20 formats!) 
	FIBITMAP* imagen = FreeImage_Load(formato, filename);
	FIBITMAP* temp = imagen;
	imagen = FreeImage_ConvertTo32Bits(imagen);
	FreeImage_Unload(temp);
	int w = FreeImage_GetWidth(imagen);
	int h = FreeImage_GetHeight(imagen);
	GLubyte* textura = new GLubyte[4 * w*h];
	char* pixeles = (char*)FreeImage_GetBits(imagen);
	for (int j = 0; j<w*h; j++) {
		textura[j * 4 + 0] = pixeles[j * 4 + 2];
		textura[j * 4 + 1] = pixeles[j * 4 + 1];
		textura[j * 4 + 2] = pixeles[j * 4 + 0];
		textura[j * 4 + 3] = pixeles[j * 4 + 3];
	}
	glBindTexture(GL_TEXTURE_2D, textures[pos]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)textura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	FreeImage_Unload(imagen);
}

void MainDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if (!chooseGame) {
		mainBackground();
		if (menuSelectColorbool) {
			menuSelectColor();
		}
	}
	
	else {
		gamePlay();
	}
	glColor3f(1, 1, 1);
	glFlush();
	glutSwapBuffers();
}

void mainBackground() {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0.0, 0.0);
	glTexCoord2d(1, 0); glVertex2f(WinWidth, 0.0);
	glTexCoord2d(1, 1); glVertex2f(WinWidth, WinHeight);
	glTexCoord2d(0, 1); glVertex2f(0.0, WinHeight);
	glEnd();
	if (!menuSelectColorbool) {
		glBindTexture(GL_TEXTURE_2D, textures[5]);
		glAlphaFunc(GL_GREATER, 0.8f);
		glBegin(GL_QUADS);
		if (positionMenu == 2) {
			glColor3f(1, 0, 0);
		}
		else
			glColor3f(1, 1, 1);
		glTexCoord2d(0, 0); glVertex2f(581.0, 146.0);
		glTexCoord2d(1, 0); glVertex2f(835.0, 146.0);
		glTexCoord2d(1, 1); glVertex2f(835.0, 183.0);
		glTexCoord2d(0, 1); glVertex2f(581.0, 183.0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[6]);
		glAlphaFunc(GL_GREATER, 0.8f);
		glBegin(GL_QUADS);
		if (positionMenu == 1) {
			glColor3f(1, 0, 0);
		}
		else
			glColor3f(1, 1, 1);
		glTexCoord2d(0, 0); glVertex2f(733.0, 77.0);
		glTexCoord2d(1, 0); glVertex2f(835.0, 77.0);
		glTexCoord2d(1, 1); glVertex2f(835.0, 110.0);
		glTexCoord2d(0, 1); glVertex2f(733.0, 110.0);
		glEnd();

		if (homeMainResum) {
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			glAlphaFunc(GL_GREATER, 0.8f);
			glBegin(GL_QUADS);
			if (positionMenu == 3) {
				glColor3f(1, 0, 0);
			}
			else
				glColor3f(1, 1, 1);
			glTexCoord2d(0, 0); glVertex2f(648.0, 203.0);
			glTexCoord2d(1, 0); glVertex2f(835.0, 203.0);
			glTexCoord2d(1, 1); glVertex2f(835.0, 236.0);
			glTexCoord2d(0, 1); glVertex2f(648.0, 236.0);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void menuSelectColor() {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0.0, 0.0);
	glTexCoord2d(1, 0); glVertex2f(WinWidth, 0.0);
	glTexCoord2d(1, 1); glVertex2f(WinWidth, WinHeight);
	glTexCoord2d(0, 1); glVertex2f(0.0, WinHeight);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	selectTriangle();
}

void selectTriangle() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[9]);
	glTranslatef(xTriangle, 262.0, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0, 0);
	glTexCoord2d(1, 0); glVertex2f(23, 0);
	glTexCoord2d(1, 1); glVertex2f(23, 21);
	glTexCoord2d(0, 1); glVertex2f(0, 21);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void specialKeyboard(int key, int x, int y) {
		switch (key)
		{
		case GLUT_KEY_UP: {
			Up = true;
			downDistance = false;
			if (!chooseGame) {
				if (!menuSelectColorbool) {
					Mix_PlayChannel(-1, chooisesound, 0);
					if (positionMenu != 3) {
						positionMenu++;
					}
					else {
						positionMenu = 1;
					}
				}
			}

			if (!collide) {
				movddline -= speed*0.1;
				
			}
			break;
		}
		case GLUT_KEY_DOWN: {
			Down = true;
			if (!chooseGame) {
				if (!menuSelectColorbool) {
					Mix_PlayChannel(-1, chooisesound, 0);
					if (positionMenu != 1) {
						positionMenu--;
					}
					else {
						positionMenu = 1;
					}
				}
			}
			if(!collide){
			movddline += speed*0.1;
			}
			break;
		}
		case GLUT_KEY_RIGHT: {
			Right = true;
			if (menuSelectColorbool) {
				Mix_PlayChannel(-1, chooisesound, 0);
				if (selectColor != 6) {
					selectColor++;
					xTriangle += 85;
				}
				else {
					xTriangle = 225;
					selectColor = 1;
				}
			}
			break;
		}
		case GLUT_KEY_LEFT: {
			Left = true;
			if (menuSelectColorbool) {
				Mix_PlayChannel(-1, chooisesound, 0);
				if (selectColor != 1) {
					selectColor--;
					xTriangle -= 85;
					if (selectColor == 1) {
						xTriangle = 225;
					}
				}
				else {
					xTriangle = 658;
					selectColor = 6;
				}
			}
			break;
		}
	}
}

void specialKeyboardUp(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP: {
		Up = false;
		break;
	}
	case GLUT_KEY_DOWN: {
		Down = false;
		break;
	}
	case GLUT_KEY_RIGHT: {
		Right = false;
		break;
	}
	case GLUT_KEY_LEFT: {
		Left = false;
		break;
	}
	}
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch (key)
	{
	case 13: {
		Mix_PlayChannel(-1, chooiseEnter, 0);
		if (collide) {
			restartgameplay();	
		}
		if (positionMenu == 1) {
			freeInitialise();
			exit(1);
		}
		if (menuSelectColorbool) {
			if (!chooseGame) {
				Mix_PlayMusic(music, -1);
				restartgameplay();
				menuSelectColorbool = false;
				chooseGame = true;
			}
		}
		if (positionMenu == 2) {
			if(!chooseGame)
			menuSelectColorbool = true;
		}
		if (positionMenu ==3) {
			if (homeMainResum) {
				homeMainResum = false;
				Mix_PlayMusic(music, -1);
				chooseGame = true;
			}
		}
		
		break;
	}
	case 27: {
		if (chooseGame) {
			Mix_PauseMusic();
			positionMenu = 3;
			homeMainResum = true;
			chooseGame = false;
		}
		if (menuSelectColorbool) {
			chooseGame = false;
			menuSelectColorbool = false;
		}
		break;
	}
	case 32: {
		speed=speed*0.3;
		break;
	}
	}
}

void gamePlay() {
	glLoadIdentity();
	randomchikDlyaBonusa = rand() % 100+ 1;
	if (randomchikDlyaBonusa == 1) {
		_50vs100 = rand() % 2 + 1;
		if (_50vs100 == 1) {
			strcpy(buffBonus, "pics/50bonus.png");
			LoadTexture(buffBonus, 11);
		}
		if (_50vs100 == 2) {
			strcpy(buffBonus, "pics/100bonus.png");
			LoadTexture(buffBonus, 11);
		}
		bonus = true;
	}
	movddline -= speed*1.5;
	distanceCar += abs(speed) * 0.09; // s=V*t
	if (movddline < -3000)
	{
		movddline = 0;
		randomchikforznak = true;
	}
	if (movddline > 2000) {
		movddline = 0;
		randomchikforznak = true;
	}
	roadDraw();
	bonusScore();
	randomBarrier();
	drawCar();
	moveCar();
	char buffer[10];
	glColor3f(1, 1, 1);
	if (!collide) {
		drawText("Score: ", 740, 570);
		_itoa(score, buffer, 10);
		drawTextNum(buffer, 4, 800, 570);
		glColor3f(1, 1, 1);
		drawText("Speed: ", 30, 570);
		_itoa(speed, buffer, 10);
		drawTextNum(buffer, 3, 90, 570);
		glColor3f(1, 1, 1);
		drawText("Distance: ", 30, 540);
		_itoa(distanceCar, buffer, 10);
		drawTextNum(buffer, 5, 115, 540);
	}
	glColor3f(0, 1, 0);
	if (collide) {
		if (!collidewithCarSound) {
			Mix_PlayChannel(-1, collidewithCar, 0);
			collidewithCarSound = true;
		}
		glColor3f(1, 1, 1);
		gameOverBackground(270, 510);
		drawText("Your score: ", 380, 470);
		_itoa(score, buffer, 10);
		drawTextNum(buffer, 4, 480, 470);
		drawText("If you want to restart, press \"Enter\"  ", 310, 140);
		drawText("To return to the menu, press \"Esc\" ", 310, 110);
		speed = 0;
		topfivebackground(350, 375);
		if (!addTableRecord) {
			addtableRecord(score, distanceCar, playArray);
			addTableRecord = true;
		}
			drawText("Score ", 375, 380);
			drawText("Distance ", 485, 380);
			for (int i(0); i < 5; i++) {
				cout << "score[" << i << "]: " << playArray[i].score << endl;
				cout << "dist[" << i << "]: " << playArray[i].distance << endl;
				cout << endl;
				_itoa(i+1, buffer, 10);
				drawTextNum(buffer, 1, 355, 350 - 35 * i);
				drawText(". ", 365, 350 - 35 * i);
				_itoa(playArray[i].score, buffer, 10);
				drawTextNum(buffer, 4, 380, 350 - 35*i);
				_itoa(playArray[i].distance, buffer, 10);
				drawTextNum(buffer, 5, 495, 350-35*i);
				if (score == playArray[i].score) {
					drawText("You ", 310, 350 - 35 * i);
				}
			}
		Mix_PauseMusic();
		if (!failsound) {
			Mix_PlayChannel(-1, fail, 0);
			failsound = true;
		}
	}
}

void restartgameplay()
{	
	 distanceCar = 0;
	 mCarX = 234.0, mCarY = 10.0;
	 score = 0;
	 speed = 0;
	 randomBarrier();
	 strcpy(buffCar, "pics/whitecolor.png");
	 LoadTexture(buffCar, 0);
	 for (int i = 0; i < 4; i++) {
		 bCarY[i] = 720;
	 }
	 Mix_PlayMusic(music, -1);
	 otherPositionBarrier();
	 failsound = false;
	 collidewithCarSound = false;
	 bonus = false;
	 addTableRecord = false;
	 collide = false;
}

void randomBarrier() {
	for (int i = 0; i <4; i++) {
		if (!collide) {
			if (Down) {
				if (!downDistance) {
					bufDistanceCar = distanceCar;
					downDistance = true;
				}
				if (speed > 5) {
					bCarY[i] = bCarY[i] - 10;
				}
				if (speed < 5) {
					bCarY[i] = bCarY[i] - 10 + abs(speed);
				}
				if (speed < -18 && (distanceCar - bufDistanceCar)>100) {
					bCarY[i] = bCarY[i] - 35;
				}
			}
			else {
				bCarY[i] = bCarY[i] - 15 - 0.9*speed;
			}

			random = rand() % 4 + 1;
			if (bCarY[i] < -120) {
				if (random == 1) {
					if (!secondtStrip) {
						bCarX[i] = 234;
						firstStrip = true;
					}
				}
			if (random == 2) {
				if (!firstStrip && !thirdStrip && !forthtStrip) {
					bCarX[i] = 350;
					secondtStrip = true;
				}
			}
			if (random == 3) {
				if (!secondtStrip && !firstStrip && !forthtStrip) {
					bCarX[i] = 466;
					thirdStrip = true;
				}
			}
			if (random == 4) {
				if (!firstStrip) {
					bCarX[i] = 582;
					forthtStrip = true;
				}
			}
		
				bCarY[i] = 680;

			}
			randomchikEcheRaz = rand() % 70 + 1;
			if (randomchikEcheRaz == 1 || randomchikEcheRaz == 51) {
				firstStrip = false;
			}
			if (randomchikEcheRaz == 2 || randomchikEcheRaz == 52) {
				secondtStrip = false;
			}
			if (randomchikEcheRaz == 3 || randomchikEcheRaz == 53) {
				thirdStrip = false;
			}
			if (randomchikEcheRaz == 4 || randomchikEcheRaz == 54) {
				forthtStrip = false;
			}
			random = 0;
			if (bCarY[i] > mCarY - 95 && bCarY[i] < mCarY + 100 && bCarX[i] == mCarX) {
				strcpy(buffCar, "pics/failCar1.png");
				LoadTexture(buffCar, 0);
				collide = true;
			}
			if (bCarY[i] > mCarY - 95 && bCarY[i] < mCarY + 100 && abs(bCarX[i] - mCarX) == 116) {
				if (speed>1 || speed<-1) {
					score++;
				}
				if (mCarX + 116 == bCarX[i]) {
					if (Right ) {
						mCarX += 40;
						strcpy(buffCar, "pics/failCar1.png");
						LoadTexture(buffCar, 0);
						collide = true;
					}
				}
				if (mCarX - 116 == bCarX[i]) {
					if (Left ) {
						mCarX -= 40;
						strcpy(buffCar, "pics/failCar1.png");
						LoadTexture(buffCar, 0);
						collide = true;
					}
				}

			}
		}
		drawBarrier(bCarX[i], bCarY[i]);
	}
}

void drawCar() {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTranslatef(mCarX, mCarY, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
	if (!collide) {
		if (selectColor == 1)
			glColor3f(0.9989, 0.9743, 0.5797);
		if (selectColor == 2)
			glColor3f(0.8913,0.5481,0.000);
		if (selectColor == 3)
			glColor3f(0.000,0.289,0.0478);
		if (selectColor == 4)
			glColor3f(0.4879,0.7156,0.2159);
		if (selectColor == 5)
			glColor3f(0.6941,0.0152,0.0078);
		if (selectColor == 6)
			glColor3f(0.000,0.2048,0.5481);
	}
	glTexCoord2d(0, 0);	glVertex3f(0, 10, 0);
	glTexCoord2d(1, 0); glVertex3f(85, 10, 0);
	glTexCoord2d(1, 1); glVertex3f(85, 120, 0);
	glTexCoord2d(0, 1); glVertex3f(0, 120, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
}

void drawBarrier(float bCarX, float bCarY) {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTranslatef(bCarX, bCarY, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
			/*
	if (selectColor == 1)
		glColor3f(0.9989, 0.9743, 0.5797);
	if (selectColor == 2)
		glColor3f(0.8913, 0.5481, 0.000);
	if (selectColor == 3)
		glColor3f(0.000, 0.289, 0.0478);
	if (selectColor == 4)
		glColor3f(0.4879, 0.7156, 0.2159);
	if (selectColor == 5)
		glColor3f(0.6941, 0.0152, 0.0078);
	if (selectColor == 6)

		glColor3f(0.000, 0.2048, 0.5481);
		*/
	glTexCoord2d(0, 0);	glVertex3f(0, 10, 0);
	glTexCoord2d(1, 0); glVertex3f(85, 10, 0);
	glTexCoord2d(1, 1); glVertex3f(85, 120, 0);
	glTexCoord2d(0, 1); glVertex3f(0, 120, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
}

void moveCar() {
	if (!collide) {
		if (Up) {
			if (speed < 22) {
				speed += decc;
				if (speed > 0) {
					if (mCarY < 125)
						mCarY += 12 * speed*0.01;
				}
			}
		}
		if (Down) {
			if (speed > -20) {
				if (speed < 10 && speed >= 1) {
					speed -= 0.7;
				}
				if (speed > -5 && speed < 1) {
					speed -= 0.5;
				}
				else
					speed -= decc;
				if (speed > 0) {
				if (mCarY >= 15)
					mCarY -= 12 * speed*0.02;
				}
			}
		}
		if (!Up || !Down) {
			speed = speed - speed*0.01;
		}
		if (Right) {
			if (mCarX < 490)
				mCarX += 116;
			Right = false;
		}
		if (Left) {
			if (mCarX > 240)
				mCarX -= 116;
			Left = false;
		}
	}
}

void otherPositionBarrier() {
	glClearColor(0, 0, 1, 0);
	for (int i = 0; i <4; i++) {
		random = rand() % 4 +1;
		if(random==1){
		if (!secondtStrip) {
			bCarX[i] = 234;
			firstStrip = true;
		}
	}
	if (random == 2) {
		if (!firstStrip && !thirdStrip && !forthtStrip) {
			bCarX[i] = 350;
			secondtStrip = true;
		}
	}
	if (random == 3) {
		if (!secondtStrip && !firstStrip && !forthtStrip) {
			bCarX[i] = 466;
			thirdStrip = true;
		}
	}
	if (random == 4) {
		if (!firstStrip) {
			bCarX[i] = 582;
			forthtStrip = true;
		}
	}
	random = 0;
	bCarY[i] = 1240 - i * 160;
		}
}

void roadDraw() {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTranslatef(0, movddline, 0);
	for (int i = -20; i < 30; i++) {
		glBegin(GL_QUADS);
		if (i % 2 == 0){
		glTexCoord2d(0, 0); glVertex2f(0, 312*i/2);
		glTexCoord2d(1, 0); glVertex2f(312,312*(i/2));
		glTexCoord2d(1, 1); glVertex2f(312,312+312*i/2);
		glTexCoord2d(0, 1); glVertex2f(0,312+312*i/2);
		}
		else {
		glTexCoord2d(0, 0); glVertex2f(588, 312 * (i-1) / 2);
		glTexCoord2d(1, 0); glVertex2f(900, 312 * (i-1) / 2);
		glTexCoord2d(1, 1); glVertex2f(900, 312 + 312 * (i-1) / 2);
		glTexCoord2d(0, 1); glVertex2f(588, 312 + 312 * (i-1) / 2);
		}
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	
	glAlphaFunc(GL_GREATER, 0.8f);
	int k = 0;

	for (int i = -12; i < 15; i++) {
		glBegin(GL_QUADS);
		if (i == 0) {
			k = -100;
		}
		glTexCoord2d(0, 0);	 glVertex3f(200.0, k + i * 300 - 50 * i, 0.0);
		glTexCoord2d(0, 1);	 glVertex3f(700.0, k + i * 300 - 50 * i, 0.0);
		glTexCoord2d(1, 1);	 glVertex3f(700.0, k + i * 300 + 300 - 50 * i, 0.0);
		glTexCoord2d(1, 0); ; glVertex3f(200.0, k + i * 300 + 300 - 50 * i, 0.0);
		glEnd();
	}

	
	glDisable(GL_TEXTURE_2D);
	float znackY=650+ movddline;
	if (randomchikforznak) {
		int randomchik = rand() % 3 + 1;
		if (randomchik == 1) {
			znackX = 70;
			znackY = 650;
		}
		if (randomchik == 2) {
			znackX = 730;
			znackY = 650;
		}
		randomchikforznak = false;
	}
	drawTwenty(znackX, znackY);
	glLoadIdentity();
}

void gameOverBackground(float x, float y) {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[12]);
	glTranslatef(x, y, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2d(0, 0); glVertex2f(0, 0);
	glTexCoord2d(1, 0); glVertex2f(357, 0);
	glTexCoord2d(1, 1); glVertex2f(357, 79);
	glTexCoord2d(0, 1); glVertex2f(0, 79);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
  
void bonusScore() {
		if (bonus) {
		bonusY = bonusY - speed;	
			if (checkrandombonus) {
				randombonus = rand() % 4 + 1;
			}
			if (bonusY < -100) {
				checkrandombonus = true;
				bonus = false;
				bonusY = 700;
			}
			if (randombonus == 1) {
					bonusX = 234; bonusY = 700;
					checkrandombonus = false;
			}
			if (randombonus == 2) {
					bonusX = 350; bonusY = 700;
					checkrandombonus = false;
			}
			if (randombonus == 3) {
					bonusX = 466; bonusY = 700;
					checkrandombonus = false;
			}
			if (randombonus == 4) {
					bonusX = 582; bonusY = 700;
					checkrandombonus = false;
			}
			if (bonusY > mCarY - 115 && bonusY < mCarY + 120 && bonusX == mCarX) {
				if (_50vs100==1)
					score += 50;
				if(_50vs100==2)
					score += 100;
				checkrandombonus = true;
				Mix_PlayChannel(-1, bonussound, 0);
				bonus = false;
			}
			drawBonusScore(bonusX, bonusY);
			randombonus = 0;
	}
}

void topfivebackground(float x, float y)
{
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[13]);
	glTranslatef(x, y, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2d(0, 0); glVertex2f(0, 0);
	glTexCoord2d(1, 0); glVertex2f(200, 0);
	glTexCoord2d(1, 1); glVertex2f(200, 100);
	glTexCoord2d(0, 1); glVertex2f(0, 100);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void freeInitialise()
{
	Mix_FreeChunk(fail);
	Mix_FreeChunk(chooisesound);
	Mix_FreeChunk(chooiseEnter);
	Mix_FreeChunk(collidewithCar);
	Mix_FreeChunk(bonussound);
	Mix_FreeMusic(music);

	Mix_CloseAudio();
	glDeleteTextures(13, textures);
}

void drawBonusScore(float bonusX, float bonusY) {
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[11]);
	glTranslatef(bonusX, bonusY, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0); glVertex2f(90, 0);
	glTexCoord2f(1, 1); glVertex2f(90, 90);
	glTexCoord2f(0, 1); glVertex2f(0, 90);
	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void drawTwenty(float XznT, float YznT)
{
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[10]);
	glTranslatef(XznT, YznT, 0);
	glAlphaFunc(GL_GREATER, 0.8f);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0); glVertex2f(100, 0);
	glTexCoord2f(1, 1); glVertex2f(100, 100);
	glTexCoord2f(0, 1); glVertex2f(0, 100);
	glEnd();
}

void drawText(char ch[], int xpos, int ypos) {
	int numofochar = strlen(ch);
	glLoadIdentity();
	glRasterPos2f(xpos, ypos);
	for (int i = 0; i<numofochar - 1; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[i]);
	}
}

void drawTextNum(char ch[], int numtext, int xpos, int ypos) {
	int len = numtext - strlen(ch), k = 0;
	glLoadIdentity();
	glRasterPos2f(xpos, ypos);
	for (int i = 0; i <= numtext - 1; i++) {
		if (i<len) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0');
		}
		else
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[k]);
			k++;
		}
	}
}
