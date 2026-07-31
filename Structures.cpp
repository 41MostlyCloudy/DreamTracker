#pragma once



#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <thread>
//#include <mutex>
#include <shared_mutex>



// The thread lock to make sure that the audio and main threads do not interfere.
std::shared_mutex mtx;
bool toIconifyScreen = false;



// Floating-point 2d position
struct Vector2 { float x = 0; float y = 0; };
struct Vector2i { int x = 0; int y = 0; };


// A square of the screen GUI
struct UIElement
{
	Vector2i sprite;
	int textCol = 0; int bgCol = 0; // Colors for text and background.
};


struct RGBColor
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
};





struct EnvelopePoint
{
	int position = 0;
	float amp = 1.0f;
};




struct InstrumentWave
{
	int operatorType = 0; // 0=Wave, 1=sample

	// Wave types: Sine, Square, Triangle, Saw, Noise, Wave A, Wave B
	int waveType = 0;

	int fuzzType = 0; // 0 = clip, 1 = fold, 2 = ring fold

	int loopStart = 0;
	int loopEnd = 0;

	

	std::vector <float> pcmFrames = { };

	// The duty cycle.
	float dutyCycle = 0.5f;
	float smoothness = 1.0f; // Smoothness
	int numOfSineWaves = 15;
	float offset = 0.5f; // Offsets the wave volume from center = 0.5f.

	
	float release = 0.0f;

	

	float fuzz = 0.0f;

	int octave = 3; // 0-15

	//int stereo = 0; // 0=Mix, 1=Left, 2=Right


	// Frequencies (1 = fundemental frequency)
	// (1/4) (1/3) (1/2) (1) (2) (3) (4) (5) (6) (7) (8)
	float frequencies[11] = { 0,0,0,0,0,0,0,0,0,0,0 };


	// Boolean flags
	bool loop = true;
	bool generateFromSines = false; // The square and saw waves can be generated from sine waves for a slightly different sound.
	bool reverseFrames = false;
	bool pitchToNote = true;
	bool continueNote = true;
	bool useArp = false;
	bool invertStereo = false;
	bool noSustain = false;


	// Envelope properties
	int envelopeLength = 1; // 1 - 80 (100 ms)
	float envelopeStartAmp = 1.0f;
	float envelopeScale = 1.0f;
	std::vector <EnvelopePoint> envelopePoints = { };



	void clampLoopPoints()
	{
		if (loopStart < 0) loopStart = 0;
		else if (loopStart > pcmFrames.size()) loopStart = pcmFrames.size();
		if (loopEnd < 0) loopEnd = 0;
		else if (loopEnd > pcmFrames.size()) loopEnd = pcmFrames.size();

		return;
	}
};



// An audio sample
struct Instrument
{
	std::string name = "NewSample";

	float volume = 1.0f;
	float glide = 0.0f;
	float scatter = 0.0f;

	bool enabled = false;

	InstrumentWave waveforms[4];
	int operatorMapping[4] = { 0, 1, 2, 3 }; // Which sample is mapped to each operator.

	int modulationTypes[4] = { 0,0,0,0 };
	float modScale[4] = { 0.0f,0.0f,0.0f,0.0f };

	float arpPitches[15] = { 7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f };
	float arpSpeed = 0.5f; // Arpeggiation speed in subdivisions of a beat.
	float arpLength = 0.0f;

	

	int algorithmType = 0;
};





struct GUITheme
{
	RGBColor uiColors[9];
};


struct ScrollBar
{
	float position = 0.0f;
	bool drag = false;
};


struct GUI
{
	bool clickingOnFloatingWind = false;

	bool drawScreen = false;
	// Tells the program to draw certain parts of the screen.
	bool drawUIThisFrame = true;
	bool drawFrameThisFrame = true;

	// 92 x 57 tile ui.
	UIElement activeUI[92][57]; // Active user interface elements on the screen.

	Vector2 hoveredTile;
	Vector2 floatHoveredTile;
	Vector2 selectedTile;
	Vector2 selectedWindowTile;

	int patternListScroll = 0;
	ScrollBar patternListScrollBar;
	int instrumentListScroll = 0;
	ScrollBar instrumentListScrollBar;
	int patternScroll;
	ScrollBar patternVerticalScrollBar;

	int helpWindowScroll = 0;
	ScrollBar helpWindowScrollBar;

	bool draggingScrollBar = false;


	float songLength = 0; // Song length in seconds.
	float songPos = 0; // Song position in seconds.


	int uiColorTheme = 26;
	std::vector <GUITheme> themes = {};
	bool lightMode = false;
	int uiDisplayMenuOption = 0; // 0 = Piano, 1 = Effects

	unsigned int uiTexture;


	float uiColors[55] =
	{
		0.0f, 0.0f, 0.0f, // Blue/Gray\n"
		30.0f / 255.0f, 30.0f / 255.0f, 60.0f / 255.0f,
		50.0f / 255.0f, 50.0f / 255.0f, 100.0f / 255.0f,
		90.0f / 255.0f, 90.0f / 255.0f, 140.0f / 255.0f,
		150.0f / 255.0f, 150.0f / 255.0f, 200.0f / 255.0f,
		1.0f, 1.0f, 1.0f,

		63.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, // Green/Yellow
		127.0f / 255.0f, 190.0f / 255.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		100.0f / 255.0f, 0.0f, 60.0f / 255.0f, // Red
		160.0f / 255.0f, 0.0f, 40.0f / 255.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 120.0f / 255.0f, // Blue
		0.0f, 90.0f / 255.0f, 180.0f / 255.0f,
		0.0f, 180.0f / 255.0f, 1.0f,

		63.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, // Accent colors
		127.0f / 255.0f, 190.0f / 255.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

	};


	
};


struct Screen
{
	Vector2 screenSize;
	float windowRatio = 1.0f;

	Vector2 mouseCoords;
	bool mouseDown = false;
	bool rightMouseDown = false;
	float mouseHoldTime = 0.0f; // The amount of time that the mouse has been held down.
	bool keyDown = false;
	bool shiftDown = false;
	bool ctrDown = false;

	bool windowed = false;
};


struct Editor
{
	bool playingSong = false;
	bool recordingSong = false;
	bool focusOnNotes = false; // Whether pressing keys will create notes.

	bool toRecordSong = false;



	// Editing settings
	int selectedOctave = 3;
	// Selectable Buttons:
	// 0 = Song name, 1 = BPM, 2 = TPB, 3 = OCT, 4 = ROW, 5 = (None), 6 = Artist name, 7 = notes window, 8 = sample name, 9-16 = channel length, 17-24 = channel beat, 25-32 = None
	int selectedButton = -1;
	int selectedInstrument = 0;
	int selectedKey = -1;
	int selectedFile = 0;

	bool playSolo = false;

	Vector2 noteSelectionStart;
	Vector2 noteSelectionEnd;
	int transposeValue = 0; // The number of semitones to transpose by if the transpose window is used.

	std::string fileToLoad = "";

	//std::vector <float> crackleSample = { 0.0f }; // The sample used to create imperfections in waveforms.
	// Crackle sample
	std::vector <float> crackleSample = {
		1, 1, 1, 1, 1, 1, 1, 1, 0.968504, 0.905512, 1, 1.06299, 1.15748, 1.28346, 1.44094, 1.75591, 1.34646, 1.18898, 1.12598, 1.0315, 0.905512, 0.968504, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.84252, 0.811024, 0.779528, 0.653543, 0.496063, 0.307087, -0.102362, 0.181102, 0.433071, 0.622047, 0.716535, 0.779528, 0.811024, 0.84252, 0.84252, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.874016, 0.905512, 0.937008, 1.09449, 1.22047, 1.50394, 1.15748, 0.968504, 0.905512, 0.905512, 0.874016, 0.874016, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.905512, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.905512, 0.84252, 0.748031, 0.622047, 0.464567, 0.370079, 0.559055, 0.716535, 0.811024, 0.874016, 0.905512, 0.905512, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.968504, 1, 0.905512, 0.84252, 0.716535, 0.559055, 0.370079, -0.0393701, 0.244094, 0.496063, 0.685039, 0.779528, 0.84252, 0.874016, 0.905512, 0.968504, 1.0315, 0.968504, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.937008, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 0.968504, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.0315, 1.06299, 1.12598, 1.15748, 0.905512, 0.433071, 0.149606, 0.0551181, 0.149606, 0.464567, 0.748031, 1.0315, 1.09449, 1.12598, 1.06299, 1.0315, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.06299, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1, 0.937008, 1, 1.06299, 1.09449, 1.12598, 1.18898, 1.28346, 1.47244, 1.72441, 2.00787, 1.59843, 1.40945, 1.25197, 1.12598, 1.06299, 0.968504, 1, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.06299, 1.06299, 1.09449, 1.15748, 1.25197, 1.40945, 1.59843, 1.50394, 1.34646, 1.22047, 1.12598, 1.06299, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1.0315, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	}; // The sample used to create imperfections in waveforms.


	int findNotePlayed(int input)
	{
		int noteNum = -1;

		if (input == 122) noteNum += 1; // z (C 1)
		else if (input == 115) noteNum += 2; // s (C# 1)	
		else if (input == 120) noteNum += 3; // x (D 1)
		else if (input == 100) noteNum += 4; // d (D# 1)
		else if (input == 99) noteNum += 5; // c (E 1)
		else if (input == 118) noteNum += 6; // v (F 1)
		else if (input == 103) noteNum += 7; // g (F# 1)
		else if (input == 98) noteNum += 8; // b (G 1)
		else if (input == 104) noteNum += 9; // h (G# 1)
		else if (input == 110) noteNum += 10; // n (A 2)
		else if (input == 106) noteNum += 11; // j (A# 2)
		else if (input == 109) noteNum += 12; // m (B 2)
		else if (input == 44) noteNum += 13; // , (C 2)
		else if (input == 108) noteNum += 14; // l (C# 2)
		else if (input == 46) noteNum += 15; // . (D 2)
		else if (input == 59) noteNum += 16; // ; (D# 2)
		else if (input == 47) noteNum += 17; // / (E 2)

		selectedKey = noteNum;


		if (input == 113) noteNum += 13; // q (C 2)
		else if (input == 50) noteNum += 14; // 2 (C# 2)
		else if (input == 119) noteNum += 15; // w (D 2)
		else if (input == 51) noteNum += 16; // 3 (D# 2)
		else if (input == 101) noteNum += 17; // e (E 2)
		else if (input == 114) noteNum += 18; // r (F 2)
		else if (input == 53) noteNum += 19; // 5 (F# 2)
		else if (input == 116) noteNum += 20; // t (G 2)
		else if (input == 54) noteNum += 21; // 6 (G# 2)
		else if (input == 121) noteNum += 22; // y (A 3)
		else if (input == 55) noteNum += 23; // 7 (A# 3)
		else if (input == 117) noteNum += 24; // u (B 3)
		else if (input == 105) noteNum += 25; // i (C 3)
		else if (input == 57) noteNum += 26; // 9 (C# 3)
		else if (input == 111) noteNum += 27; // o (D 3)
		else if (input == 48) noteNum += 28; // 0 (D# 3)
		else if (input == 112) noteNum += 29; // p (E 3)
		else if (input == 91) noteNum += 30; // [ (F 3)
		else if (input == 61) noteNum += 31; // = (F# 3)
		else if (input == 93) noteNum += 32; // ] (A 3)

		if (selectedKey == -1)
		{
			if (noteNum == -1)
				selectedKey = -1;
			else
				selectedKey = noteNum + 5;
		}
		if (noteNum != -1)
		{
			noteNum += selectedOctave * 12;

			if (noteNum > 255)
				noteNum = 255;

		}

		return(noteNum);
	}


	int findEDONotePlayed(int input, int edo)
	{
		int noteNum = -1;

		if (input == 122) noteNum = 1;
		else if (input == 120) noteNum = 2;
		else if (input == 99) noteNum = 3;
		else if (input == 118) noteNum = 4;
		else if (input == 98) noteNum = 5;
		else if (input == 110) noteNum = 6;
		else if (input == 109) noteNum = 7;
		else if (input == 44) noteNum = 8;
		else if (input == 46) noteNum = 9;
		else if (input == 47) noteNum = 10;

		else if (input == 97) noteNum = 11;
		else if (input == 115) noteNum = 12;
		else if (input == 100) noteNum = 13;
		else if (input == 102) noteNum = 14;
		else if (input == 103) noteNum = 15;
		else if (input == 104) noteNum = 16;
		else if (input == 106) noteNum = 17;
		else if (input == 107) noteNum = 18;
		else if (input == 108) noteNum = 19;
		else if (input == 59) noteNum = 20;
		else if (input == 39) noteNum = 21;

		else if (input == 113) noteNum = 22;
		else if (input == 119) noteNum = 23;
		else if (input == 101) noteNum = 24;
		else if (input == 114) noteNum = 25;
		else if (input == 116) noteNum = 26;
		else if (input == 121) noteNum = 27;
		else if (input == 117) noteNum = 28;
		else if (input == 105) noteNum = 29;
		else if (input == 111) noteNum = 30;
		else if (input == 112) noteNum = 31;
		else if (input == 91) noteNum = 32;
		else if (input == 93) noteNum = 33;

		else if (input == 49) noteNum = 34;
		else if (input == 50) noteNum = 35;
		else if (input == 51) noteNum = 36;
		else if (input == 52) noteNum = 37;
		else if (input == 53) noteNum = 38;
		else if (input == 54) noteNum = 49;
		else if (input == 55) noteNum = 40;
		else if (input == 56) noteNum = 41;
		else if (input == 57) noteNum = 42;
		else if (input == 48) noteNum = 43;
		else if (input == 45) noteNum = 44;
		else if (input == 61) noteNum = 45;

		selectedKey = noteNum - 1;

		if (noteNum != -1)
		{
			noteNum -= 1;

			noteNum += selectedOctave * edo;

			if (noteNum > 255)
				noteNum = 255;

		}

		return(noteNum);
	}
};







struct ChannelWaveform
{
	float sampleReadPos = 0.0f;


	float pitch = 0.0f;
	float volume = 1.0f;
	float volumeSlide = 0.0f;
	int note = 0;


	float jumpPoint = 0.0f;


	float envelopeSpeed = 1.0f;

	// Envelope
	float envelopePos = 0.0f;
	int currentEnvelopeIndex = 0;

	float currentEnvelopePos = 0.0f;
	float currentEnvelopeAmp = 0.0f;
	float nextEnvelopePos = 0.0f;
	float nextEnvelopeAmp = 0.0f;

	float glideDest = 0.0f;
	float glideVolume = 0.0f;


	double releaseTimer = 0.0f; // Restarts when note is stopped. Interpolates to 1. Stepped according to release amount.

	

	// Interpolation between notes to prevent clipping.
	float interpTimer = 0.0f; // Decrements from 0 to 1, then stops.
	float interpLastReadPos = 0.0f;


	float sampleRate = 1.0f;


	// Low-pass filter.
	float y1, y2, y3, y4;
	float oldx, oldy1, oldy2, oldy3;



	// HIgh-pass filter.
	float highPass = 0.0f;


	float RC = (1.0f / (2.0f * 3.14159265f * 1.0f));
	float alphaHigh = RC / (RC + (1.0f / 48000.0f));

	float prevHighPassSample = 0.0f; // Previous output sample (for filtering)
	float prevHighPassSampleI = 0.0f; // Previous output sample (for filtering)


	



	// Process a single sample (4-pole Moog-style ladder filter)
	float process(float input, float f, float fb, float q)
	{
		// Apply feedback to create resonance
		input -= y4 * fb;
		input *= 0.35013f * (f * f) * (f * f);

		// Four cascaded one-pole filters
		y1 = input + 0.3f * oldx + (1.0f - f) * y1;
		oldx = input;

		y2 = y1 + 0.3f * oldy1 + (1.0f - f) * y2;
		oldy1 = y1;

		y3 = y2 + 0.3f * oldy2 + (1.0f - f) * y3;
		oldy2 = y2;

		y4 = y3 + 0.3f * oldy3 + (1.0f - f) * y4;
		oldy3 = y3;


		if (y4 < -1.0f) y4 = -1.0f;
		else if (y4 > 1.0f) y4 = 1.0f;

		return y4;
	}


	void wrapReadPos(bool loop, float loopStart, float loopEnd)
	{
		if (loopStart == loopEnd)
		{
			sampleReadPos = 0.0f;
			return;
		}

		if (!loop) // No loop.
		{
			if (sampleReadPos >= loopEnd)
				sampleReadPos = loopEnd - 1;
		}
		else // Loop.
		{
			while (sampleReadPos >= loopEnd)
				sampleReadPos -= (loopEnd - loopStart);
		}

		return;
	}
};




// Audio channel
struct Channel
{
	bool playing = false;

	int patternOffset = 0; // If set more than 0, decrement.
	int offsetNote = 0;
	int offsetInstrument = 0;

	bool muted = false;
	bool solo = false;
	bool loopAtEnd = false;

	bool noteStopped = false; // When true, skip sustain and go to release.

	
	ChannelWaveform waveforms[4];


	float stereo = 0.5f; // (0 - 1) (L - R)


	float volume = 1.0f;

	float pitchSlide = 0.0f;

	float retrigger = 0.0f;
	int retriggerTimer = 0;


	// Channel arp pitches.
	float arpP[15] = { 7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f,7.75f };
	float arpTimer = 0.0f;
	int arpIndex = -1;
	float arpOriginalNote = 0.0f;

	


	int instrument = 0;

	int interpLastInstrument = 0;

	






	void resetChannelEffects(bool resetVolume)
	{

		for (int wave = 0; wave < 4; wave++)
		{
			waveforms[wave].volume = 1.0f;
			waveforms[wave].volumeSlide = 0.0f;
			waveforms[wave].envelopeSpeed = 1.0f;

			waveforms[wave].sampleRate = 1.0f;

			waveforms[wave].jumpPoint = 0.0f;

			// High-pass filter.
			waveforms[wave].highPass = 0.0f;

			// Low-pass filter
			if (resetVolume)
			{
				waveforms[wave].y1 = 0; waveforms[wave].y2 = 0; waveforms[wave].y3 = 0; waveforms[wave].y4 = 0;
				waveforms[wave].oldx = 0; waveforms[wave].oldy1 = 0; waveforms[wave].oldy2 = 0; waveforms[wave].oldy3 = 0;
			}
		}



		pitchSlide = 0.0f;

		if (resetVolume)
			volume = 1.0f;


		patternOffset = 0;

		retrigger = 0.0f;

		

		noteStopped = true;
	}
};



// 1 channel of a frame.
struct RolledChannel
{
	uint8_t rows = 32; // Length of the frame in beats.
	int beatsPerMeasure = 4;
	float stereo = 0.5f; // (0 - 1) (L - R)
	bool loopAtEnd = false;

	std::vector <int> notes;
	std::vector <int> volumes;
	std::vector <int> effects;
};



struct ChannelPatterns
{
	std::vector <RolledChannel> patterns;
};



// Song frame object
struct PatternIndexTable
{
	uint8_t rows = 32; // Length of the frame in beats.

	int channelPatterns[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
};



struct UnrolledRow
{
	int note = -1;
	int instrument = -1;
	int volume = -1;
	int effect = -1;
};



// 1 row of an unrolled frame.
struct UnrolledChannel
{
	uint8_t  rows = 32;
	int beatsPerMeasure = 4;

	std::vector <UnrolledRow> data;
};



struct UnrolledPattern
{
	uint8_t  rows = 32;

	UnrolledChannel channels[8];
};



struct Song
{
	std::string songName = "NewSong1";
	std::string artistName = "Me";


	int edo = 12; // Equal divisions of an octave. (Number of tones)
	float startingBPM = 120;
	float bpm = 120;

	std::vector <int> patternSequence;

	std::vector <PatternIndexTable> patterns;

	ChannelPatterns channelPatterns[8];


	int currentPattern = 0; // Current frame in frameSequence.
	int currentNote = 0;
	float timeInNote = 0.0f;
	float timeInSong = 0.0f;



	float toNextChannelNote[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	float toNextChannelVolume[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	float toNextChannelEffect[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	float noteChannelIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	float volumeChannelIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	float effectChannelIndex[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };


	bool unsavedChanges = false;
	bool overWriteOldSong = false;
	bool unusedFrames = false;
};



struct SampleDisplay
{
	bool visible = false;


	bool displayArp = false;

	int displayType = 0; // 0=frames, 1=envelope

	Vector2i position;
	RGBColor pixelData[528 * 192]; // Screen data (528 x 160) pixels.

	//float zoomFactor = 1.0f; // Increase to zoom in on the sample.

	bool playingInstrument = false;

	bool drawing = false;
	Vector2 drawWavePos; // { pos in frames, volume }

	int sampleStartPos = 0;



	bool dragLoopStart = false;
	bool dragLoopEnd = false;




	bool enableSnap = true;
	int snapSubdivisions = 16;

	int measurementSystem = 0; // 0 = frames, 1 = ms, 2 = seconds



	int selectedOperator = 0;
	int operatorMenuSelectedOperator = 0; // The operator selected in the operator menu.
	int swapMenuOperators[4] = { 1, 2, 3, 4 };


	std::vector <float> copyFrames = {};


	Instrument SwapOperators(Instrument sample)
	{
		drawing = false; // Stop sample drawing.
		InstrumentWave swapWaves[4];

		for (int wave = 0; wave < 4; wave++)
			swapWaves[wave] = sample.waveforms[swapMenuOperators[wave]];

		for (int wave = 0; wave < 4; wave++)
			sample.waveforms[wave] = swapWaves[wave];

		return sample;
	}
};



struct FloatingWindow
{
	std::string name = "Floating Window";
	Vector2i position;
	Vector2i size;
	bool dragWindow = false;
	Vector2i dragPoint;
};



struct WindowController
{
	std::vector <FloatingWindow> windows;


	

	bool HasWindow(std::string windName)
	{
		bool exists = false;

		for (int i = 0; i < windows.size(); i++)
		{
			if (windows[i].name == windName)
				exists = true;
		}

		return exists;
	};


	void InitializeWindow(std::string windowName, Vector2i windPos, Vector2i windSize)
	{
		if (HasWindow(windowName)) // Don't create copies of existing windows.
			return;

		FloatingWindow newWindow;

		for (int i = 0; i < windows.size(); i++) // Move the window so that it doesn't fall on the exact same spot as old ones.
		{
			if (windows[i].position.x == windPos.x)
				windPos.x++;
			if (windows[i].position.y == windPos.y)
				windPos.y++;
		}

		if (windPos.x < 0) windPos.x = 0;
		if (windPos.x + windSize.x > 91) windPos.x = 91 - windSize.x;
		if (windPos.y < 0) windPos.y = 0;
		if (windPos.y + windSize.y > 56)windPos.y = 56 - windSize.y;

		if (windSize.x < windowName.length() + 6) windSize.x = (float)windowName.length() + 6;
		if (windSize.y < 3)  windSize.y = 3;
		if (windSize.y > 56)  windSize.y = 56;


		newWindow.name = windowName;
		newWindow.position = windPos;
		newWindow.size = windSize;


		windows.emplace(windows.begin(), newWindow);

		return;
	}
};



struct FileNavigator
{
	std::filesystem::path currentFilePath;

	std::string filePathName = "";

	std::vector <std::string> fileNames = {};

	int fileListScroll = 0;
	ScrollBar fileScrollBar;



	std::string getRelativePath()
	{

		std::string currentPath = std::filesystem::current_path().string();

		for (int i = 0; i < currentPath.length(); i++)
		{
			if (currentPath.at(i) == '\\')
				currentPath.at(i) = '/';
		}

		return currentPath;
	}


	std::string getFilePath()
	{

		std::string currentPath = currentFilePath.string();

		for (int i = 0; i < currentPath.length(); i++)
		{
			if (currentPath.at(i) == '\\')
				currentPath.at(i) = '/';
		}

		return currentPath;
	}


	
	void NavigateToFile()
	{
		std::string pathName = currentFilePath.std::filesystem::path::string();

		filePathName = " " + currentFilePath.std::filesystem::path::filename().generic_string();

		fileNames.clear();
		for (auto const& dir_entry : std::filesystem::directory_iterator(pathName))
		{
			std::string sampleName;
			if (!dir_entry.path().std::filesystem::path::has_extension())
				sampleName = "0" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else if (dir_entry.path().std::filesystem::path::extension() == ".song")
				sampleName = "1" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else if (dir_entry.path().std::filesystem::path::extension() == ".wav" || dir_entry.path().std::filesystem::path::extension() == ".ogg")
				sampleName = "2" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else if (dir_entry.path().std::filesystem::path::extension() == ".inst")
				sampleName = "3" + dir_entry.path().std::filesystem::path::filename().generic_string();
			else
				sampleName = "4" + dir_entry.path().std::filesystem::path::filename().generic_string();

			fileNames.emplace_back(sampleName);
		}

		fileListScroll = 0;

		return;
	}

	void ExitFile()
	{
		currentFilePath = currentFilePath.std::filesystem::path::parent_path();
		
		NavigateToFile();

		return;
	}

	void EnterFile(std::string fileName)
	{
		currentFilePath = currentFilePath.std::filesystem::path::append(fileName);

		NavigateToFile();

		return;
	}

	void NavigateToSamplesFile()
	{
		currentFilePath = std::filesystem::current_path();

		NavigateToFile();

		return;
	}

};



struct PresetMenu
{
	std::filesystem::path currentFilePath;

	std::vector <std::string> fileNames = {};


	// 32 * 6 = 192
	int instrumentType = 0;
	std::string categories[6] = { "Leads", "Pads", "Bass", "Keys", "Percussion", "SFX" };

	int selectedSample = 0;


	void NavigateToFile()
	{
		std::string pathName = currentFilePath.std::filesystem::path::string();

		fileNames.clear();
		for (auto const& dir_entry : std::filesystem::directory_iterator(pathName))
		{
			std::string sampleName;
			if (dir_entry.path().std::filesystem::path::extension() == ".inst")
			{
				sampleName = dir_entry.path().std::filesystem::path::filename().generic_string();
				sampleName.erase(sampleName.length() - 5, 6);
				fileNames.emplace_back(sampleName);
			}
		}

		return;
	}

	void NavigateToInstrumentType(std::string category)
	{
		currentFilePath = std::filesystem::current_path();
		currentFilePath = currentFilePath.std::filesystem::path::append("Presets");

		

		//currentFilePath = std::filesystem::relative("Presets", "C:/");
		currentFilePath = currentFilePath.std::filesystem::path::append(category);
		NavigateToFile();
		return;
	}




};


struct InstrumentFloatingWindow
{
	int selectedInstrument = 0;
	int instrumentListScroll = 0;
	ScrollBar instrumentListScrollBar;
};