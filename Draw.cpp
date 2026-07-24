#pragma once



void DrawEverything();

void DrawTopUI();
void DrawKeyBoard();
void DrawEDOKeyboard();
void DrawLogo();

void DrawKey(int button, int key, Vector2i pos);
void DrawEDOKey(int key, Vector2i pos);

void DrawFrameBorder();

void DrawChannel(int channelNum, int offsetX);
void DrawChannelLine(int channelNum, int offsetX, int y);
void DrawChannelLineValues(int channelNum, int offsetX, int y);

void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol);
void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);
void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, bool continueDown);

void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol);
void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol);
void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawNote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);
void DrawEDONote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex);

void DrawFloatingWindow(FloatingWindow* wind);

void DrawWaveTypeButton(int type, int xVal, int yVal);

void DrawHorizontalSlider(int xStart, int xEnd, int yVal, float value);
void DrawVerticalSlider(int yStart, int yEnd, int xVal, float value);

void DrawHorizontalScrollBar(int xStart, int xEnd, int yVal, float value);
void DrawVerticalScrollBar(int yStart, int yEnd, int xVal, float value);

void DrawAlgorithm(Vector2i pos, int type, bool selected);



void DrawEverything()
{
	
	std::shared_lock<std::shared_mutex> lock(mtx);

	
	// Scroll vertically when playing the song.
	if (editor.playingSong)
	{
		gui.patternScroll = loadedSong.currentNote - 8;
		
		if (gui.patternScroll > int(loadedPattern.rows) - 40)
			gui.patternScroll = int(loadedPattern.rows) - 40;
		if (gui.patternScroll < 0)
			gui.patternScroll = 0;
	}

	gui.drawScreen = true;

	if (gui.drawFrameThisFrame)
	{
		int xStart = 4;
		for (int i = 0; i < 8; i++)
		{
			DrawChannel(i, xStart);

			xStart += 11;
		}
		
		DrawFrameBorder();
		
		DrawTopUI();
	}
	else if (gui.drawUIThisFrame)
	{
		DrawTopUI();
	}



	for (int wind = windowController.windows.size() - 1; wind > -1; wind--) // Draw windows.
		DrawFloatingWindow(&windowController.windows[wind]);
	
	return;
}



void  DrawTopUI()
{
	
	for (int y = 0; y < 11; y++)
	{
		for (int x = 0; x < 92; x++)
		{
			if (gui.lightMode)
			{
				gui.activeUI[x][y].bgCol = 4;
				gui.activeUI[x][y].sprite = { 18, 0 };
			}
			else
			{
				gui.activeUI[x][y].bgCol = 0;
				gui.activeUI[x][y].sprite = { 15, 0 };
			}
		}
	}


	// Song editor
	DrawGUIText("Song", 1, 5, 0, 3, 0);
	DrawGUIText("Num", 7, 10, 4, 3, 0);
	DrawGUIText("Add", 7, 10, 7, 3, 0);

	// Frame list
	gui.activeUI[0][1].sprite = { 0, 3 };
	gui.activeUI[1][1].sprite = { 2, 3 };
	gui.activeUI[2][1].sprite = { 2, 3 };
	gui.activeUI[3][1].sprite = { 3, 3 };
	gui.activeUI[4][1].sprite = { 2, 3 };
	gui.activeUI[5][1].sprite = { 2, 3 };
	gui.activeUI[6][1].sprite = { 1, 3 };



	for (int y = 2; y < 11; y++)
	{
		gui.activeUI[0][y].sprite = { 17, 0 };
		gui.activeUI[0][y].bgCol = 2;
		gui.activeUI[3][y].sprite = { 13, 12 };
	} 

	for (int y = 0; y < 9; y++)
	{
		if (y + gui.patternListScroll < loadedSong.patternSequence.size())
		{
			
			if (y + gui.patternListScroll == loadedSong.currentPattern)
			{
				DrawHex(loadedSong.patternSequence[y + gui.patternListScroll], 4, 2 + y, 5, 3, -1, -1);
				DrawHex(y + int(gui.patternListScroll), 1, 2 + y, 4, 3, -1, -1);
				gui.activeUI[3][y + 2].sprite = { 5, 4 };

				if (gui.lightMode)
				{
					gui.activeUI[4][y + 2].textCol = 5;
					gui.activeUI[5][y + 2].textCol = 5;
				}
			}
			else
			{
				DrawHex(loadedSong.patternSequence[y + gui.patternListScroll], 4, 2 + y, 5, 0, -1, -1);
				DrawHex(y + int(gui.patternListScroll), 1, 2 + y, 3, 2, -1, -1);
			}
		}
		else
		{
			if (gui.lightMode)
			{
				gui.activeUI[4][y + 2].sprite = { 18, 0 };
				gui.activeUI[4][y + 2].bgCol = 4;
				gui.activeUI[5][y + 2].sprite = { 18, 0 };
				gui.activeUI[5][y + 2].bgCol = 4;
			}
			else
			{
				gui.activeUI[4][y + 2].sprite = { 15, 0 };
				gui.activeUI[4][y + 2].bgCol = 0;
				gui.activeUI[5][y + 2].sprite = { 15, 0 };
				gui.activeUI[5][y + 2].bgCol = 0;
			}
			DrawHex(y + int(gui.patternListScroll), 1, 2 + y, 3, 2, -1, -1);
		}
	}


	DrawVerticalScrollBar(3, 10, 6, gui.patternListScrollBar.position);

	

	// Edit button box
	for (int x = 7; x < 10; x++)
	{
		gui.activeUI[x][3].sprite = { 2, 3 };
		gui.activeUI[x][9].sprite = { 2, 3 };
	}
	for (int y = 4; y < 9; y++)
	{
		gui.activeUI[10][y].sprite = { 2, 4 };
	}
	gui.activeUI[10][3].sprite = { 1, 3 };
	gui.activeUI[10][9].sprite = { 1, 4 };


	// Song/Frame settings box
	DrawGUIText("Settings", 11, 21, 0, 3, 0);
	gui.activeUI[10][1].sprite = { 0, 3 };
	gui.activeUI[18][1].sprite = { 1, 3 };

	
	for (int x = 11; x < 18; x++)
	{
		gui.activeUI[x][1].sprite = { 2, 3 };
	}
	for (int y = 2; y < 11; y++)
	{
		gui.activeUI[10][y].sprite = { 2, 4 };
		gui.activeUI[18][y].sprite = { 2, 4 };
		gui.activeUI[35][y].sprite = { 2, 4 };
		gui.activeUI[42][y].sprite = { 2, 4 };
	}

	gui.activeUI[10][3].sprite = { 4, 3 };
	gui.activeUI[10][9].sprite = { 4, 3 };

	gui.activeUI[35][1].sprite = { 0, 3 };
	for (int x = 36; x < 42; x++)
		gui.activeUI[x][1].sprite = { 2, 3 };
	gui.activeUI[42][1].sprite = { 1, 3 };

	// Buttons
	DrawGUIText("BPM", 11, 14, 2, 3, 0);
	DrawNum(loadedSong.bpm, 15, 18, 2, 4, 0, -1, 1);
	DrawGUIText("EDO", 11, 14, 4, 3, 0);
	DrawNum(loadedSong.edo, 15, 17, 4, 4, 0, -1, 2);
	DrawGUIText("OCT", 11, 14, 6, 3, 0);
	DrawNum(editor.selectedOctave, 15, 16, 6, 4, 0, -1, 3);
	DrawGUIText("ROWS", 11, 15, 8, 3, 0);
	DrawNum(loadedPattern.rows, 15, 18, 8, 4, 0, -1, 4);



	// Song name box
	if (gui.uiDisplayMenuOption != 2) // Don't draw with Effect GUI.
	{
		DrawGUIText("Song Name", 46, 21 + 45, 0, 3, 0);




		gui.activeUI[44][1].sprite = { 0, 3 };
		gui.activeUI[44][2].sprite = { 2, 4 };
		gui.activeUI[44][3].sprite = { 2, 4 };
		gui.activeUI[44][4].sprite = { 0, 4 };

		gui.activeUI[22 + 44][1].sprite = { 1, 3 };
		gui.activeUI[22 + 44][2].sprite = { 2, 4 };
		gui.activeUI[22 + 44][3].sprite = { 2, 4 };
		gui.activeUI[22 + 44][4].sprite = { 1, 4 };
		for (int x = 0; x < 21; x++)
		{
			gui.activeUI[45 + x][1].sprite = { 2, 3 };
			gui.activeUI[45 + x][4].sprite = { 2, 3 };
		}
		DrawGUIText(loadedSong.songName, 45, 22 + 44, 2, 5, 0, -1, 0);
		DrawGUIText("BY:", 45, 22 + 44, 3, 5, 0, -1, -1);
		DrawGUIText(loadedSong.artistName, 48, 22 + 44, 3, 5, 0, -1, 6);
	}


	// GUI Display Menu
	DrawGUIText("Song", 36, 42, 2, 3, 0);
	DrawGUIText("Keys", 36, 42, 3, 3, 0);
	DrawGUIText("Effect", 36, 42, 4, 3, 0);

	if (gui.lightMode)
	{
		for (int x = 36; x < 42; x++)
		{
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].textCol = 5;
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].bgCol = 1;
		}
	}
	else
	{
		for (int x = 36; x < 42; x++)
		{
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].textCol = 5;
			gui.activeUI[x][gui.uiDisplayMenuOption + 2].bgCol = 3;
		}
	}

	gui.activeUI[35][gui.uiDisplayMenuOption + 2].sprite = { 5, 4 };
	

	

	DrawGUIText("Instruments", 70, 81, 0, 3, 0);
	
	


	// Sample select box
	for (int y = 3; y < 11; y++)
	{
		gui.activeUI[68][y].sprite = { 2, 4 };
		gui.activeUI[71][y].sprite = { 2, 4 };
	}
	for (int x = 68; x < 91; x++)
	{
		gui.activeUI[x][1].sprite = { 2, 3 };
	}
	gui.activeUI[68][2].sprite = { 2, 4 };
	gui.activeUI[71][2].sprite = { 2, 4 };

	DrawVerticalScrollBar(3, 10, 91, gui.instrumentListScrollBar.position);

	
	for (int y = 2; y < 11; y++)
	{
		if (y - 2 + gui.instrumentListScroll < 256 && y - 2 + gui.instrumentListScroll >= 0) // Draw samples
		{
			if (!loadedInstruments[y - 2 + gui.instrumentListScroll].enabled)
			{
				if (y - 2 + gui.instrumentListScroll == editor.selectedInstrument)
				{
					DrawGUIText("", 72, 91, y, 5, 3);
					DrawHex(y - 2 + int(gui.instrumentListScroll), 69, y, 4, 3);
					gui.activeUI[71][y].sprite = { 5, 4 };
					gui.activeUI[90][y].sprite = { 26, 2 }; // Create sample button

					if (gui.lightMode)
					{
						for (int x = 69; x < 71; x++)
						{
							gui.activeUI[x][y].textCol = 5;
						}
					}
				}
				else
				{
					DrawGUIText("", 72, 91, y, 4, 0);
					DrawHex(y - 2 + int(gui.instrumentListScroll), 69, y, 3, 0);
				}
			}
			else
			{
				if (y - 2 + gui.instrumentListScroll == editor.selectedInstrument)
				{
					if (editor.selectedButton == 8)
					{
						if (gui.lightMode)
							DrawGUIText(loadedInstruments[y - 2 + gui.instrumentListScroll].name, 72, 91, y, 5, 2, -1, 8);
						else
							DrawGUIText(loadedInstruments[y - 2 + gui.instrumentListScroll].name, 72, 91, y, 5, 1, -1, 8);
					}
					else
						DrawGUIText(loadedInstruments[y - 2 + gui.instrumentListScroll].name, 72, 91, y, 5, 3);
					gui.activeUI[71][y].sprite = { 5, 4 };
					DrawHex(y - 2 + int(gui.instrumentListScroll), 69, y, 4, 3);
					gui.activeUI[90][y].sprite = { 31, 9 }; // Delete button

					if (gui.lightMode)
					{
						for (int x = 69; x < 91; x++)
						{
							gui.activeUI[x][y].textCol = 5;
						}
					}
				}
				else
				{
					DrawGUIText(loadedInstruments[y - 2 + gui.instrumentListScroll].name, 72, 91, y, 4, 0);
					DrawHex(y - 2 + int(gui.instrumentListScroll), 69, y, 3, 0);
				}
			}
		}
		else
		{
			DrawGUIText("", 72, 91, y, 4, 0);
			DrawHex(y - 2 + int(gui.instrumentListScroll), 69, y, 2, 0);
		}
	}


	

	gui.activeUI[86][0].sprite = { 29, 12 }; // Minimize program button
	gui.activeUI[87][0].sprite = { 30, 12 };

	if (screen.windowed)
	{
		gui.activeUI[88][0].sprite = { 29, 11 }; // Window program button
		gui.activeUI[89][0].sprite = { 30, 11 };
	}
	else
	{
		gui.activeUI[88][0].sprite = { 29, 10 }; // Window program button
		gui.activeUI[89][0].sprite = { 30, 10 };
	}

	gui.activeUI[90][0].sprite = { 29, 9 }; // Exit program button
	gui.activeUI[91][0].sprite = { 30, 9 };
	


	gui.activeUI[68][1].sprite = { 0, 3 };
	gui.activeUI[71][1].sprite = { 3, 3 };
	gui.activeUI[91][1].sprite = { 1, 3 };


	// Logo
	DrawLogo();
	

	// UI Buttons.
	DrawGUIText("FILE", 20, 34, 7, 4, -1);
	DrawGUIText("SETTINGS", 20, 34, 8, 4, -1);
	DrawGUIText("HELP", 20, 34, 9, 4, -1);
	DrawGUIText("INSTRUMENT", 20, 34, 10, 4, -1);


	gui.activeUI[19][7].sprite = { 5, 27 };
	gui.activeUI[34][7].sprite = { 6, 27 };

	for (int y = 8; y < 10; y++)
	{
		gui.activeUI[19][y].sprite = { 5, 28 };
		gui.activeUI[34][y].sprite = { 6, 28 };
	}

	gui.activeUI[19][10].sprite = { 5, 29 };
	gui.activeUI[34][10].sprite = { 6, 29 };
	


	// Draw the play and pause buttons.
	if (editor.playingSong)
	{
		gui.activeUI[0][13].sprite = { 14, 7 };
		gui.activeUI[0][14].sprite = { 14, 8 };
		gui.activeUI[1][13].sprite = { 15, 7 };
		gui.activeUI[1][14].sprite = { 15, 8 };

		gui.activeUI[2][13].sprite = { 16, 7 };
		gui.activeUI[2][14].sprite = { 16, 8 };
		gui.activeUI[3][13].sprite = { 17, 7 };
		gui.activeUI[3][14].sprite = { 17, 8 };
	}
	else
	{
		gui.activeUI[0][13].sprite = { 12, 7 };
		gui.activeUI[0][14].sprite = { 12, 8 };
		gui.activeUI[1][13].sprite = { 13, 7 };
		gui.activeUI[1][14].sprite = { 13, 8 };

		gui.activeUI[2][13].sprite = { 18, 7 };
		gui.activeUI[2][14].sprite = { 18, 8 };
		gui.activeUI[3][13].sprite = { 19, 7 };
		gui.activeUI[3][14].sprite = { 19, 8 };
	}



	// Buttons
	for (int y = 0; y < 4; y++) // Revert buttons to unpressed.
	{
		gui.activeUI[11][3 + 2 * y].sprite = { 8, 3 };
		gui.activeUI[12][3 + 2 * y].sprite = { 9, 3 };
		gui.activeUI[13][3 + 2 * y].sprite = { 10, 3 };
	}
	gui.activeUI[7][5].sprite = { 8, 3 };
	gui.activeUI[8][5].sprite = { 9, 3 };
	gui.activeUI[9][5].sprite = { 10, 3 };
	gui.activeUI[7][8].sprite = { 8, 3 };
	gui.activeUI[8][8].sprite = { 9, 3 };
	gui.activeUI[9][8].sprite = { 10, 3 };
	



	if (gui.uiDisplayMenuOption == 0) // Song GUI.
	{
		gui.songLength = calculateSongLength();
		int songSecs = (int)gui.songLength % 60;
		int songMins = (int)gui.songLength / 60;
		DrawGUIText("Song Length:", 44, 56, 6, 3, 0);
		DrawNum(songMins, 56, 58, 6, 4, 0);
		DrawGUIText(":", 58, 59, 6, 4, 0);
		DrawNum(songSecs, 59, 61, 6, 4, 0);


		gui.songPos = calculateSongPosition();
		songSecs = (int)gui.songPos % 60;
		songMins = (int)gui.songPos / 60;
		DrawGUIText("Current Position:", 44, 61, 8, 3, 0);
		DrawNum(songMins, 61, 63, 8, 4, 0);
		DrawGUIText(":", 63, 64, 8, 4, 0);
		DrawNum(songSecs, 64, 66, 8, 4, 0);

		// Song position bar.
		float posInSong = (gui.songPos / gui.songLength);

		DrawHorizontalSlider(44, 44 + 23, 7, posInSong);

		

		
		//////////////////////////////////////////////////
	}
	else if (gui.uiDisplayMenuOption == 1) // Key GUI.
	{
		// Draw keyboard
		if (loadedSong.edo != 12)
			DrawEDOKeyboard();
		else
			DrawKeyBoard();
	}
	else if (gui.uiDisplayMenuOption == 2) // Effect GUI.
	{
		for (int x = 0; x < 25; x++)
		{
			gui.activeUI[43 + x][0].sprite = { 2, 3 };
		}

		DrawGUIText("(1-4): Operator Amplitude", 43, 68, 3, 3, 0);
		DrawGUIText("(A): Increase Pitch", 43, 68, 4, 3, 0);
		DrawGUIText("(B): Decrease Pitch", 43, 68, 5, 3, 0);
		DrawGUIText("(C): Jump to Sample Point", 43, 68, 6, 3, 0);
		DrawGUIText("(D): Delay Note", 43, 68, 7, 3, 0);
		DrawGUIText("(E): Retrigger Note", 43, 68, 8, 3, 0);


		gui.activeUI[42][0].sprite = { 0, 3 };
		gui.activeUI[42][1].sprite = { 4, 3 };
		gui.activeUI[68][1].sprite = { 3, 4 };
		gui.activeUI[68][0].sprite = { 1, 3 };
	}

	return;
}


void DrawLogo()
{
	int logoX = 21;
	int logoY = 1;

	// D
	gui.activeUI[logoX][logoY].sprite = { 24, 7 }; gui.activeUI[logoX+1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY+1].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY+1].sprite = { 27, 7 };
	gui.activeUI[logoX][logoY+2].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY+2].sprite = { 23, 8 };
	logoX += 2;
	// R
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 7 };
	logoX += 2;
	// E
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 22, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// A
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 25, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 27, 7 };
	logoX += 2;
	// M
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 24, 7 }; gui.activeUI[logoX + 2][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 26, 7 }; gui.activeUI[logoX + 2][logoY + 1].sprite = { 27, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 2][logoY + 2].sprite = { 27, 7 };
	logoX = 20;
	logoY += 3;
	// T
	gui.activeUI[logoX][logoY].sprite = { 24, 8 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// R
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 7 };
	logoX += 2;
	// A
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 25, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 27, 7 };
	logoX += 2;
	// C
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 26, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 22, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// K
	gui.activeUI[logoX][logoY].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 7 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 27, 7 };
	logoX += 2;
	// E
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 22, 8 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 8 };
	logoX += 2;
	// R
	gui.activeUI[logoX][logoY].sprite = { 22, 7 }; gui.activeUI[logoX + 1][logoY].sprite = { 23, 7 };
	gui.activeUI[logoX][logoY + 1].sprite = { 24, 8 }; gui.activeUI[logoX + 1][logoY + 1].sprite = { 23, 8 };
	gui.activeUI[logoX][logoY + 2].sprite = { 26, 7 }; gui.activeUI[logoX + 1][logoY + 2].sprite = { 23, 7 };



	return;
}



void DrawKeyBoard()
{
	gui.activeUI[44][7].sprite = { 24, 9 };
	gui.activeUI[44][8].sprite = { 25, 9 };

	for (int x = 45; x < 60; x++)
	{
		gui.activeUI[x][8].sprite = { 26, 9 };
	}
	gui.activeUI[60][8].sprite = { 27, 9 };
	gui.activeUI[60][9].sprite = { 28, 9 };

	DrawKey(17, 0, { 44, 6 });
	DrawKey(18, 1, { 45, 5 });
	DrawKey(19, 2, { 46, 6 });
	DrawKey(20, 3, { 47, 5 });
	DrawKey(21, 4, { 48, 6 });
	DrawKey(22, 5, { 50, 6 });
	DrawKey(23, 6, { 51, 5 });
	DrawKey(24, 7, { 52, 6 });
	DrawKey(25, 8, { 53, 5 });
	DrawKey(26, 9, { 54, 6 });
	DrawKey(27, 10, { 55, 5 });
	DrawKey(28, 11, { 56, 6 });

	DrawKey(29, 0, { 58, 6 });
	DrawKey(30, 1, { 59, 5 });
	DrawKey(31, 2, { 60, 6 });
	DrawKey(32, 3, { 61, 5 });
	DrawKey(33, 4, { 62, 6 });
	DrawKey(34, 5, { 64, 6 });
	DrawKey(35, 6, { 65, 5 });
	DrawKey(36, 7, { 66, 6 });


	DrawKey(0, 0, { 46, 10 });
	DrawKey(1, 1, { 47, 9 });
	DrawKey(2, 2, { 48, 10 });
	DrawKey(3, 3, { 49, 9 });
	DrawKey(4, 4, { 50, 10 });
	DrawKey(5, 5, { 52, 10 });
	DrawKey(6, 6, { 53, 9 });
	DrawKey(7, 7, { 54, 10 });
	DrawKey(8, 8, { 55, 9 });
	DrawKey(9, 9, { 56, 10 });
	DrawKey(10, 10, { 57, 9 });
	DrawKey(11, 11, { 58, 10 });

	DrawKey(12, 0, { 60, 10 });
	DrawKey(13, 1, { 61, 9 });
	DrawKey(14, 2, { 62, 10 });
	DrawKey(15, 3, { 63, 9 });
	DrawKey(16, 4, { 64, 10 });

	return;
}



void DrawEDOKeyboard()
{
	DrawEDOKey(0, { 46, 10 });
	DrawEDOKey(1, { 48, 10 });
	DrawEDOKey(2, { 50, 10 });
	DrawEDOKey(3, { 52, 10 });
	DrawEDOKey(4, { 54, 10 });
	DrawEDOKey(5, { 56, 10 });
	DrawEDOKey(6, { 58, 10 });
	DrawEDOKey(7, { 60, 10 });
	DrawEDOKey(8, { 62, 10 });
	DrawEDOKey(9, { 64, 10 });

	DrawEDOKey(10, { 45, 9 });
	DrawEDOKey(11, { 47, 9 });
	DrawEDOKey(12, { 49, 9 });
	DrawEDOKey(13, { 51, 9 });
	DrawEDOKey(14, { 53, 9 });
	DrawEDOKey(15, { 55, 9 });
	DrawEDOKey(16, { 57, 9 });
	DrawEDOKey(17, { 59, 9 });
	DrawEDOKey(18, { 61, 9 });
	DrawEDOKey(19, { 63, 9 });
	DrawEDOKey(20, { 65, 9 });

	DrawEDOKey(21, { 44, 8 });
	DrawEDOKey(22, { 46, 8 });
	DrawEDOKey(23, { 48, 8 });
	DrawEDOKey(24, { 50, 8 });
	DrawEDOKey(25, { 52, 8 });
	DrawEDOKey(26, { 54, 8 });
	DrawEDOKey(27, { 56, 8 });
	DrawEDOKey(28, { 58, 8 });
	DrawEDOKey(29, { 60, 8 });
	DrawEDOKey(30, { 62, 8 });
	DrawEDOKey(31, { 64, 8 });
	DrawEDOKey(32, { 64, 8 });
	DrawEDOKey(33, { 66, 8 });

	DrawEDOKey(34, { 43, 7 });
	DrawEDOKey(35, { 45, 7 });
	DrawEDOKey(36, { 47, 7 });
	DrawEDOKey(37, { 49, 7 });
	DrawEDOKey(38, { 51, 7 });
	DrawEDOKey(39, { 53, 7 });
	DrawEDOKey(40, { 55, 7 });
	DrawEDOKey(41, { 57, 7 });
	DrawEDOKey(42, { 59, 7 });
	DrawEDOKey(43, { 61, 7 });
	DrawEDOKey(44, { 63, 7 });
	DrawEDOKey(45, { 65, 7 });
	


	return;
}




void DrawFrameBorder()
{
	


	for (int x = 0; x < 4; x++)
	{
		for (int y = 11; y < 16; y++)
		{
			gui.activeUI[x][y].bgCol = 2;
			gui.activeUI[x][y].textCol = 2;
		}

		gui.activeUI[x][56].sprite = { 17, 0 };
		gui.activeUI[x][56].bgCol = 2;
	}




	



	DrawVerticalScrollBar(17, 55, 91, gui.patternVerticalScrollBar.position);


	for (int y = 16; y < 56; y++)
	{
		gui.activeUI[0][y].sprite = { 17, 0 };
		gui.activeUI[0][y].bgCol = 2;

		// Beat numbering.
		DrawNum(y - 16 + gui.patternScroll, 1, 4, y, 3, 2);

		if (y - 16 + gui.patternScroll == loadedSong.currentNote)
		{

			for (int x = 0; x < 4; x++)
			{
				gui.activeUI[x][y].bgCol = 15;
				gui.activeUI[x][y].textCol = 5;
			}

			gui.activeUI[4][y].sprite = { 14, 12 };
		}
	}


	return;
}



void DrawChannel(int channelNum, int offsetX)
{
	// Channel top
	gui.activeUI[offsetX][15].sprite = { 13, 12 };
	gui.activeUI[offsetX][14].sprite = { 13, 12 };
	gui.activeUI[offsetX][13].sprite = { 15, 14 };
	gui.activeUI[offsetX][12].sprite = { 17, 0 };
	gui.activeUI[offsetX][12].bgCol = 2;
	gui.activeUI[offsetX][11].sprite = { 17, 14 };

	gui.activeUI[offsetX + 1][13].sprite = { 15 + channelNum, 13 };
	gui.activeUI[offsetX + 1][12].sprite = { 15 + channelNum, 12 };
	gui.activeUI[offsetX + 1][11].sprite = { 17, 14 };

	gui.activeUI[offsetX + 2][13].sprite = { 16, 14 };
	gui.activeUI[offsetX + 2][12].sprite = { 13, 12 };
	gui.activeUI[offsetX + 2][11].sprite = { 12, 13 };

	for (int x = 3; x < 10; x++)
		gui.activeUI[offsetX + x][11].sprite = { 13, 13 };

	gui.activeUI[offsetX + 10][15].sprite = { 12, 12 };
	gui.activeUI[offsetX + 10][14].sprite = { 12, 12 };
	gui.activeUI[offsetX + 10][13].sprite = { 12, 12 };
	gui.activeUI[offsetX + 10][12].sprite = { 12, 12 };
	gui.activeUI[offsetX + 10][11].sprite = { 14, 13 };


	


	gui.activeUI[offsetX + 5][12].sprite = { 15, 0 };
	gui.activeUI[offsetX + 1][14].sprite = { 15, 0 };

	int patternIndex = loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[channelNum];
	DrawHex(patternIndex, offsetX + 3, 12, 4, 0, -1, 25 + channelNum);



	DrawGUIText("ROW:", offsetX + 3, offsetX + 7, 13, 2, 0);
	DrawNum(loadedPattern.channels[channelNum].rows, offsetX + 7, offsetX + 10, 13, 4, 0, -1, 9 + channelNum);

	DrawGUIText("BEAT:", offsetX + 2, offsetX + 7, 14, 2, 0);
	DrawNum(loadedPattern.channels[channelNum].beatsPerMeasure, offsetX + 7, offsetX + 10, 14, 4, 0, -1, 17 + channelNum);



	// Stereo
	gui.activeUI[offsetX + 1][15].sprite = { 18, 14 };
	for (int x = 2; x < 9; x++)
		gui.activeUI[offsetX + x][15].sprite = { 19, 14 };
	gui.activeUI[offsetX + 5][15].sprite = { 20, 14 };
	gui.activeUI[offsetX + 9][15].sprite = { 21, 14 };



	// Buttons
	if (channels[channelNum].muted)
		gui.activeUI[offsetX + 6][12].sprite = { 23, 14 };
	else
		gui.activeUI[offsetX + 6][12].sprite = { 23, 13 };

	if (channels[channelNum].solo)
		gui.activeUI[offsetX + 7][12].sprite = { 24, 14 };
	else
		gui.activeUI[offsetX + 7][12].sprite = { 24, 13 };

	if (channels[channelNum].loopAtEnd)
	{
		gui.activeUI[offsetX + 8][12].sprite = { 25, 14 };
		gui.activeUI[offsetX + 9][12].sprite = { 26, 14 };
	}
	else
	{
		gui.activeUI[offsetX + 8][12].sprite = { 25, 13 };
		gui.activeUI[offsetX + 9][12].sprite = { 26, 13 };
	}



	int stereo = (channels[channelNum].stereo * 8.0f);
	gui.activeUI[offsetX + 1 + stereo][15].sprite = { 22, 14 };




	for (int y = 0; y < 40; y++) // Channel rows
	{
		DrawChannelLine(channelNum, offsetX, y);
		if (int(gui.patternScroll) + y < loadedPattern.rows)
			DrawChannelLineValues(channelNum, offsetX, y);
	}


	// Channel bottom
	if (int(gui.patternScroll) + 40 == loadedPattern.rows) // At pattern end
	{
		for (int x = 0; x < 11; x++)
		{
			gui.activeUI[offsetX + x][56].sprite = { 8, 14 };
			gui.activeUI[offsetX + x][56].bgCol = 2;
		}
	}
	else if (int(gui.patternScroll) + 40 > loadedPattern.rows) // Past pattern end
	{
		for (int x = 0; x < 11; x++)
		{
			gui.activeUI[offsetX + x][56].sprite = { 17, 0 };
			gui.activeUI[offsetX + x][56].bgCol = 2;
		}
	}
	else if (int(gui.patternScroll) + 40 > loadedPattern.channels[channelNum].rows) // Past channel end
	{
		for (int x = 0; x < 11; x++)
		{
			gui.activeUI[offsetX + x][56].sprite = { 17, 0 };
			gui.activeUI[offsetX + x][56].bgCol = 2;
		}
	}
	else
	{
		gui.activeUI[offsetX][56].sprite = { 12, 14 };
		for (int x = 1; x < 10; x++)
			gui.activeUI[offsetX + x][56].sprite = { 13, 14 };
		gui.activeUI[offsetX + 10][56].sprite = { 14, 14 };

		if (int(gui.patternScroll) + 40 < loadedPattern.channels[channelNum].rows) // Channel continues
		{
			gui.activeUI[offsetX + 4][56].sprite = { 9, 14 };
			gui.activeUI[offsetX + 5][56].sprite = { 10, 14 };
			gui.activeUI[offsetX + 6][56].sprite = { 11, 14 };
		}
	}

	

	return;
}



void DrawChannelLine(int channelNum, int offsetX, int y)
{


	int intScrollY = int(gui.patternScroll);


	if (int(y + intScrollY) == loadedPattern.rows)
	{
		for (int x = 0; x < 11; x++)
			gui.activeUI[offsetX + x][y + 16].sprite = { 8, 14 };


		if (int(y + intScrollY) < loadedPattern.channels[channelNum].rows)
		{
			gui.activeUI[offsetX + 4][y + 16].sprite = { 9, 14 };
			gui.activeUI[offsetX + 5][y + 16].sprite = { 10, 14 };
			gui.activeUI[offsetX + 6][y + 16].sprite = { 11, 14 };
		}
		
		return;
	}
	else if (int(y + intScrollY) == loadedPattern.channels[channelNum].rows)
	{
		gui.activeUI[offsetX][y + 16].sprite = { 12, 14 };
		for (int x = 1; x < 10; x++)
			gui.activeUI[offsetX + x][y + 16].sprite = { 13, 14 };
		gui.activeUI[offsetX + 10][y + 16].sprite = { 14, 14 };
		return;
	}
	else if (y + intScrollY > loadedPattern.channels[channelNum].rows || y + intScrollY > loadedPattern.rows)
	{
		for (int x = 0; x < 11; x++)
		{
			gui.activeUI[offsetX + x][y + 16].sprite = { 17, 0 };
			gui.activeUI[offsetX + x][y + 16].bgCol = 2;
		}
		return;
	}
	



	
	
	


	int xOffset = offsetX;
	int bgColor = 0;
	int textColor = 1;


	if (y + intScrollY == loadedSong.currentNote) // Note position marker.
	{
		bgColor = 2;
		textColor = 3;
	}
	else if (loadedPattern.channels[channelNum].beatsPerMeasure > 0 && y + intScrollY < loadedPattern.channels[channelNum].rows && (y + intScrollY) % loadedPattern.channels[channelNum].beatsPerMeasure == 0)
	{
		bgColor = 1;
		textColor = 2;
	}

	
	if (channels[channelNum].muted || (editor.playSolo && !channels[channelNum].solo) || intScrollY + y >= loadedPattern.channels[channelNum].rows)
	{
		bgColor--;
		textColor--;
		if (gui.lightMode)
		{
			bgColor -= 3;
			textColor -= 3;
		}
		if (bgColor < 0) bgColor = 0;
		if (textColor < 0) textColor = 0;
	}

	if (gui.lightMode)
	{
		if (bgColor < 5) bgColor++;
		if (textColor < 5) textColor++;
		if (bgColor < 6 && bgColor >= 0)
			bgColor = 5 - bgColor;
		if (textColor < 6 && textColor >= 0)
			textColor = 5 - textColor;

		if (y + intScrollY == loadedSong.currentNote) // Note position marker.
		{
			textColor = 17;
			bgColor = 15;
		}
	}

	gui.activeUI[xOffset][y + 16].sprite = { 13, 12 };
	xOffset++;
	
	// Note
	gui.activeUI[xOffset][y + 16].sprite = { 4, 17 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;
	gui.activeUI[xOffset][y + 16].sprite = { 5, 17 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;
	gui.activeUI[xOffset][y + 16].sprite = { 6, 17 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;
	// Instrument
	gui.activeUI[xOffset][y + 16].sprite = { 17, 0 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;
	// Instrument
	gui.activeUI[xOffset][y + 16].sprite = { 17, 0 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;

	// Volume
	if (xOffset > 4 && xOffset < 91)
	gui.activeUI[xOffset][y + 16].sprite = { 4, 17 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;
	// Volume
	gui.activeUI[xOffset][y + 16].sprite = { 6, 17 };
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	xOffset++;

	// Effects
	if (xOffset > 4 && xOffset < 91)
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	gui.activeUI[xOffset][y + 16].sprite = { 4, 17 };
	xOffset++;
	gui.activeUI[xOffset][y + 16].bgCol = bgColor;
	gui.activeUI[xOffset][y + 16].textCol = textColor;
	gui.activeUI[xOffset][y + 16].sprite = { 6, 17 };

	xOffset++;
	gui.activeUI[xOffset][y + 16].sprite = { 12, 12 };

	return;
}



void DrawChannelLineValues(int channelNum, int offsetX, int y)
{
	int intScrollY = int(gui.patternScroll);



	int rowNum = y + intScrollY;

	bool inLoop = false;

	if (channels[channelNum].loopAtEnd)
	{
		if (rowNum >= loadedPattern.channels[channelNum].rows)
		{
			rowNum = rowNum % loadedPattern.channels[channelNum].rows;
			inLoop = true;
		}
	}


	int xOffset = offsetX;
	int bgColor = 0;
	int textColor = 5;


	if (y + intScrollY == loadedSong.currentNote) // Note position marker.
	{
		bgColor = 2;
	}
	else if (loadedPattern.channels[channelNum].beatsPerMeasure > 0 && y + intScrollY < loadedPattern.channels[channelNum].data.size() && (y + intScrollY) % loadedPattern.channels[channelNum].beatsPerMeasure == 0)
	{
		bgColor = 1;
	}

	



	if (channels[channelNum].muted || (editor.playSolo && !channels[channelNum].solo))
	{
		bgColor--;
		textColor--;

		if (bgColor < 0) bgColor = 0;
		if (textColor < 0) textColor = 0;
	}

	if (gui.lightMode)
	{
		if (y + intScrollY == loadedSong.currentNote) // Note position marker.
		{
			bgColor = 15;
		}
	}


	if (inLoop)
	{
		bgColor = 2;
	}


	//if (y + intScrollY >= loadedFrame.rows.size())
	//	return;

	//if (intScrollY == 0)
	//	return;

	//if (loadedSong.currentNote < 0)
	//	std::cout << " -1 ";
	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	xOffset++;


	if (rowNum < loadedPattern.channels[channelNum].data.size())
	{

		if (loadedPattern.channels[channelNum].data[rowNum].note == 255) // Stop Note
		{
			for (int i = 0; i < 5; i++)
			{
				gui.activeUI[xOffset][y + 16].sprite = { 21 + i, 3 };
				xOffset++;
			}
		}
		else
		{
			if (loadedPattern.channels[channelNum].data[rowNum].note > -1) // Note
			{
				DrawEDONote(uint8_t(loadedPattern.channels[channelNum].data[rowNum].note), xOffset, y + 16, textColor, bgColor, -1, -1);
			}

			

			xOffset += 3;
			if (loadedPattern.channels[channelNum].data[rowNum].instrument > -1) // Instrument
			{
				DrawHex(uint8_t(loadedPattern.channels[channelNum].data[rowNum].instrument), xOffset, y + 16, textColor + 3, bgColor, -1, -1);
			}
			xOffset += 2;
		}
		

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (loadedPattern.channels[channelNum].data[rowNum].volume > -1) // Volume
		{
			DrawHex(int(loadedPattern.channels[channelNum].data[rowNum].volume), xOffset, y + 16, textColor + 6, bgColor);
		}
		xOffset += 2;



		if (loadedPattern.channels[channelNum].data[rowNum].effect > -1) // Effect
		{
			DrawHex(int(loadedPattern.channels[channelNum].data[rowNum].effect), xOffset, y + 16, textColor + 9, bgColor);
		}
		xOffset += 2;


		
	}

	

	if (y + intScrollY >= editor.noteSelectionStart.y && y + intScrollY <= editor.noteSelectionEnd.y) // Draw selected notes.
	{
		int xStart = editor.noteSelectionStart.x + 5;
		int xEnd = editor.noteSelectionEnd.x + 5;

		if (xStart < 5)
			xStart = 5;
		else if (xStart > 91)
			xStart = 91;
		if (xEnd < 5)
			xEnd = 5;
		else if (xEnd > 91)
			xEnd = 91;

		for (int x = xStart; x < xEnd; x++)
		{
			if (gui.lightMode)
			{
				gui.activeUI[x][y + 16].bgCol = 1;
				gui.activeUI[x][y + 16].textCol = 4;
			}
			else
			{
				gui.activeUI[x][y + 16].bgCol = 4;
				gui.activeUI[x][y + 16].textCol = 0;
			}
		}
	}

	return;
}



void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}




	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		selectedChar = gui.selectedTile.x - textStart;
	}

	std::string textS = std::to_string(int(text));
	for (int i = 0; i < textEnd - textStart; i++)
	{
		int textI = i - (textEnd - textStart) + textS.length();
		Vector2i textSprite = { 17, 0 };

		if (textI > -1 && textI < textS.length())
		{
			if (textS.at(textI) > 47 && textS.at(textI) < 58) // 0-9
				textSprite = { textS.at(textI) - 48, 0 };
		}
		else
			textSprite = { 0, 0 };

		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
		gui.activeUI[textStart + i][textY].bgCol = bgCol;

		if (i == selectedChar)
			gui.activeUI[textStart + i][textY].bgCol = 3;
	}

	return;
}



void DrawNum(int text, int textStart, int textEnd, int textY, int textCol, int bgCol)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}



	std::string textS = std::to_string(int(text));
	for (int i = 0; i < textEnd - textStart; i++)
	{
		int textI = i - (textEnd - textStart) + textS.length();
		Vector2i textSprite = { 17, 0 };

		if (textI > -1 && textI < textS.length())
		{
			if (textS.at(textI) > 47 && textS.at(textI) < 58) // 0-9
				textSprite = { textS.at(textI) - 48, 0 };
		}
		else
			textSprite = { 0, 0 };

		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
		gui.activeUI[textStart + i][textY].bgCol = bgCol;
	}

	return;
}



void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol)
{
	if (gui.lightMode)
	{
		if (bgCol >= 0)
		{
			if (bgCol < 5) bgCol++;
			if (bgCol < 6 && bgCol >= 0)
				bgCol = 5 - bgCol;
		}

		if (textCol < 5) textCol++;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	for (int i = 0; i < textEnd - textStart; i++)
	{
		if (i > textEnd)
			return;

		Vector2i textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { text.at(i) - 40 + 13, 0 };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { text.at(i) - 48, 0 };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { text.at(i) - 65, 1 };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { text.at(i) - 97, 2 };
			else if (text.at(i) == 44) // ,
				textSprite = { 23, 0 };
			else if (text.at(i) == 45) // -
				textSprite = { 11, 0 };
			else if (text.at(i) == 46) // .
				textSprite = { 22, 0 };
			else if (text.at(i) == 33) // !
				textSprite = { 27, 0 };
			else if (text.at(i) == 63) // ?
				textSprite = { 26, 0 };
			else if (text.at(i) == 58 || text.at(i) == 59) // :
				textSprite = { 30, 0 };
			else if (text.at(i) == 38) // &
				textSprite = { 28, 0 };
			else if (text.at(i) == 39 || text.at(i) == 34) // '
				textSprite = { 26, 1 };
		}

		// Draw text as button.
		if (bgCol == -1)
		{
			if (textSprite.y == 0)
			{
				textSprite.y = 31;
			}
			else
			{
				if (textSprite.y != 1)
					textSprite.x = 26;
				textSprite.y = 30;
			}
		}
		else
			gui.activeUI[textStart + i][textY].bgCol = bgCol;


		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
	}

	return;
}



void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}



	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		selectedChar = gui.selectedTile.x - textStart;
	}

	for (int i = 0; i < textEnd - textStart; i++)
	{
		if (i > textEnd)
			return;

		Vector2i textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { text.at(i) - 40 + 13, 0 };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { text.at(i) - 48, 0 };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { text.at(i) - 65, 1 };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { text.at(i) - 97, 2 };
			else if (text.at(i) == 44) // ,
				textSprite = { 23, 0 };
			else if (text.at(i) == 45) // -
				textSprite = { 11, 0 };
			else if (text.at(i) == 46) // .
				textSprite = { 22, 0 };
			else if (text.at(i) == 33) // !
				textSprite = { 27, 0 };
			else if (text.at(i) == 63) // ?
				textSprite = { 26, 0 };
			else if (text.at(i) == 58) // :
				textSprite = { 30, 0 };
			else if (text.at(i) == 38) // &
				textSprite = { 28, 0 };
		}

		gui.activeUI[textStart + i][textY].sprite = textSprite;
		gui.activeUI[textStart + i][textY].textCol = textCol;
		gui.activeUI[textStart + i][textY].bgCol = bgCol;

		if (i == selectedChar)
			gui.activeUI[textStart + i][textY].bgCol = 3;
	}

	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		if (loadedSong.songName.length() == 0)
			gui.activeUI[textStart][textY].bgCol = 3;
	}

	return;
}



void DrawGUIText(std::string text, int textStart, int textEnd, int textY, int textCol, int bgCol, bool continueDown)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	int yPos = textY;
	int xPos = textStart;

	for (int i = 0; i < text.length(); i++)
	{
		if (xPos > textEnd)
		{
			yPos++;
			xPos = textStart;

			if (yPos > 55)
				return;
		}

		Vector2i textSprite = { 17, 0 };

		if (i < text.length())
		{
			if (text.at(i) > 39 && text.at(i) < 42) // (-)
				textSprite = { text.at(i) - 40 + 13, 0 };
			else if (text.at(i) > 47 && text.at(i) < 58) // 0-9
				textSprite = { text.at(i) - 48, 0 };
			else if (text.at(i) > 64 && text.at(i) < 91) // A-Z
				textSprite = { text.at(i) - 65, 1 };
			else if (text.at(i) > 96 && text.at(i) < 123) // a-z
				textSprite = { text.at(i) - 97, 2 };
			else if (text.at(i) == 44) // ,
				textSprite = { 23, 0 };
			else if (text.at(i) == 45) // -
				textSprite = { 11, 0 };
			else if (text.at(i) == 46) // .
				textSprite = { 22, 0 };
			else if (text.at(i) == 33) // !
				textSprite = { 27, 0 };
			else if (text.at(i) == 63) // ?
				textSprite = { 26, 0 };
			else if (text.at(i) == 58) // :
				textSprite = { 30, 0 };
			else if (text.at(i) == 38) // &
				textSprite = { 28, 0 };
		}

		gui.activeUI[xPos][yPos].sprite = textSprite;
		gui.activeUI[xPos][yPos].textCol = textCol;
		gui.activeUI[xPos][yPos].bgCol = bgCol;

		xPos++;
	}

	return;
}


void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}


	Vector2i textSprite = { 15, 0 };

	int value1 = num / 16;
	int value2 = num % 16;

	//std::cout << "  " << value1 << "  " << value2;

	if (value1 < 10)
		textSprite = { value1, 0 };
	else
		textSprite = { value1 - 10, 1 };

	gui.activeUI[textStart][textY].sprite = textSprite;
	gui.activeUI[textStart][textY].textCol = textCol;
	gui.activeUI[textStart][textY].bgCol = bgCol;

	if (value2 < 10)
		textSprite = { value2, 0 };
	else
		textSprite = { value2 - 10, 1 };

	gui.activeUI[textStart + 1][textY].sprite = textSprite;
	gui.activeUI[textStart + 1][textY].textCol = textCol;
	gui.activeUI[textStart + 1][textY].bgCol = bgCol;

	return;
}



void DrawHex(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}




	int selectedChar = -1; // Selected letter.
	if (buttonIndex > -1 && editor.selectedButton == buttonIndex)
	{
		selectedChar = gui.selectedTile.x - textStart;
	}

	Vector2i textSprite = { 15, 0 };

	int value1 = num / 16;
	int value2 = num % 16;

	//std::cout << "  " << value1 << "  " << value2;

	if (value1 < 10)
		textSprite = { value1, 0 };
	else
		textSprite = { value1 - 10, 1 };

	if (textStart < 0 || textStart > 91)
		return;

	gui.activeUI[textStart][textY].sprite = textSprite;
	gui.activeUI[textStart][textY].textCol = textCol;
	gui.activeUI[textStart][textY].bgCol = bgCol;

	if (textStart + 1 < 0 || textStart + 1 > 91)
		return;

	if (value2 < 10)
		textSprite = { value2, 0 };
	else
		textSprite = { value2 - 10, 1 };

	gui.activeUI[textStart + 1][textY].sprite = textSprite;
	gui.activeUI[textStart + 1][textY].textCol = textCol;
	gui.activeUI[textStart + 1][textY].bgCol = bgCol;


	if (selectedChar == 0)
		gui.activeUI[textStart][textY].bgCol = 3;
	else if (selectedChar == 1)
		gui.activeUI[textStart + 1][textY].bgCol = 3;

	return;
}


void DrawNote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{
	if (gui.lightMode)
	{
		if (bgCol < 5) bgCol++;
		if (textCol < 5) textCol++;
		if (bgCol < 6 && bgCol >= 0)
			bgCol = 5 - bgCol;
		if (textCol < 6 && textCol >= 0)
			textCol = 5 - textCol;
	}



	// A, A#, B, C, C#, D, D#, E, F, F#, G, G#
	int letter = num % 12;
	int octave = int(float(num) / 12.0f);

	switch (letter)
	{
		case 0: // C
		{
			gui.activeUI[textStart][textY].sprite = { 2, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}
		case 1: // C#
		{
			gui.activeUI[textStart][textY].sprite = { 2, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 2: // D
		{
			gui.activeUI[textStart][textY].sprite = { 3, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 3: // D#
		{
			gui.activeUI[textStart][textY].sprite = { 3, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 4: // E
		{
			gui.activeUI[textStart][textY].sprite = { 4, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 5: // F
		{
			gui.activeUI[textStart][textY].sprite = { 5, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 6: // F#
		{
			gui.activeUI[textStart][textY].sprite = { 5, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 7: // G
		{
			gui.activeUI[textStart][textY].sprite = { 6, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 8: // G#
		{
			gui.activeUI[textStart][textY].sprite = { 6, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		case 9: // A
		{
			gui.activeUI[textStart][textY].sprite = { 0, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}

		case 10: // A#
		{
			gui.activeUI[textStart][textY].sprite = { 0, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 10, 0 };
			break;
		}

		default: // B
		{
			gui.activeUI[textStart][textY].sprite = { 1, 1 };
			gui.activeUI[textStart + 1][textY].sprite = { 11, 0 };
			break;
		}
	}


	gui.activeUI[textStart][textY].textCol = textCol;
	gui.activeUI[textStart][textY].bgCol = bgCol;

	gui.activeUI[textStart + 1][textY].textCol = textCol;
	gui.activeUI[textStart + 1][textY].bgCol = bgCol;
	DrawNum(octave, textStart + 2, textStart + 3, textY, textCol, bgCol);

	return;
}


void DrawEDONote(uint8_t num, int textStart, int textY, int textCol, int bgCol, int selectedLetter, int buttonIndex)
{



	// A, A#, B, C, C#, D, D#, E, F, F#, G, G#
	int scaleDeg = num % loadedSong.edo;
	int octave = num / loadedSong.edo;

	float floatLetter = (float(scaleDeg) / float(loadedSong.edo)) * 12.0f;
	int scaleLetter = floatLetter;
	float betweenNotes = floatLetter - scaleLetter;

	bool sharped = false;

	switch (scaleLetter)
	{
	case 0: // C
	{
		gui.activeUI[textStart][textY].sprite = { 2, 1 };
		break;
	}
	case 1: // C#
	{
		gui.activeUI[textStart][textY].sprite = { 2, 1 };
		sharped = true;
		break;
	}

	case 2: // D
	{
		gui.activeUI[textStart][textY].sprite = { 3, 1 };
		break;
	}

	case 3: // D#
	{
		gui.activeUI[textStart][textY].sprite = { 3, 1 };
		sharped = true;
		break;
	}

	case 4: // E
	{
		gui.activeUI[textStart][textY].sprite = { 4, 1 };
		break;
	}

	case 5: // F
	{
		gui.activeUI[textStart][textY].sprite = { 5, 1 };
		break;
	}

	case 6: // F#
	{
		gui.activeUI[textStart][textY].sprite = { 5, 1 };
		sharped = true;
		break;
	}

	case 7: // G
	{
		gui.activeUI[textStart][textY].sprite = { 6, 1 };
		break;
	}

	case 8: // G#
	{
		gui.activeUI[textStart][textY].sprite = { 6, 1 };
		sharped = true;
		break;
	}

	case 9: // A
	{
		gui.activeUI[textStart][textY].sprite = { 0, 1 };
		break;
	}

	case 10: // A#
	{
		gui.activeUI[textStart][textY].sprite = { 0, 1 };
		sharped = true;
		break;
	}

	default: // B
	{
		gui.activeUI[textStart][textY].sprite = { 1, 1 };
		break;
	}
	}

	int lightT = textCol;
	int lightB = bgCol;
	if (gui.lightMode)
	{
		if (lightB < 5) lightB++;
		if (lightT < 5) lightT++;
		if (lightB < 6 && lightB >= 0)
			lightB = 5 - lightB;
		if (lightT < 6 && lightT >= 0)
			lightT = 5 - lightT;
	}

	gui.activeUI[textStart][textY].textCol = lightT;
	gui.activeUI[textStart][textY].bgCol = lightB;

	gui.activeUI[textStart + 1][textY].textCol = lightT;
	gui.activeUI[textStart + 1][textY].bgCol = lightB;


	DrawNum(octave, textStart + 2, textStart + 3, textY, textCol, bgCol);


	////////////////////////////////////////////////////////////////////////////////////

	if (betweenNotes < 0.25f)
		gui.activeUI[textStart + 1][textY].sprite = { 0, 17 + sharped };
	else if (betweenNotes < 0.5f)
		gui.activeUI[textStart + 1][textY].sprite = { 1, 17 + sharped };
	else if (betweenNotes < 0.75f)
		gui.activeUI[textStart + 1][textY].sprite = { 2, 17 + sharped };
	else
		gui.activeUI[textStart + 1][textY].sprite = { 3, 17 + sharped };

	return;
}


void DrawKey(int button, int key, Vector2i pos)
{
	if (button == editor.selectedKey) // Key selected
	{
		gui.activeUI[pos.x][pos.y].textCol = 5;
		gui.activeUI[pos.x][pos.y].bgCol = 16;
		gui.activeUI[pos.x + 1][pos.y].textCol = 5;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 16;
	}
	else
	{
		gui.activeUI[pos.x][pos.y].textCol = 3;
		gui.activeUI[pos.x][pos.y].bgCol = 0;
		gui.activeUI[pos.x + 1][pos.y].textCol = 3;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 0;
	}

	gui.activeUI[pos.x][pos.y].sprite = { key * 2, 9 };
	gui.activeUI[pos.x + 1][pos.y].sprite = { key * 2 + 1, 9 };

	return;
}


void DrawEDOKey(int key, Vector2i pos)
{
	if (key == editor.selectedKey) // Key selected
	{
		gui.activeUI[pos.x][pos.y].bgCol = 3;
		gui.activeUI[pos.x][pos.y].textCol = 5;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 3;
		gui.activeUI[pos.x + 1][pos.y].textCol = 5;
	}
	else
	{
		gui.activeUI[pos.x][pos.y].bgCol = 1;
		gui.activeUI[pos.x][pos.y].textCol = 3;
		gui.activeUI[pos.x + 1][pos.y].bgCol = 1;
		gui.activeUI[pos.x + 1][pos.y].textCol = 3;
	}


	int scaleDeg = key % loadedSong.edo;

	float floatLetter = (float(scaleDeg) / float(loadedSong.edo)) * 12.0f;
	int scaleLetter = floatLetter;
	float betweenNotes = floatLetter - scaleLetter;


	

	bool sharped = false;

	if (scaleLetter < 2) // C/C#
	{
		if (scaleLetter == 1)
			sharped = true;
		else if (betweenNotes < 0.25f)
		{
			if (key == editor.selectedKey) // Key selected
				gui.activeUI[int(pos.x)][int(pos.y)].textCol = 8;
			else
				gui.activeUI[int(pos.x)][int(pos.y)].textCol = 7;
		}
		scaleLetter = 2;
	}
	else if (scaleLetter < 4) // D/D#
	{
		if (scaleLetter == 3)
			sharped = true;
		scaleLetter = 3;
	}
	else if (scaleLetter == 4) // E
	{
		scaleLetter = 4;
	}
	else if (scaleLetter < 7) // F/F#
	{
		if (scaleLetter == 6)
			sharped = true;
		scaleLetter = 5;
	}
	else if (scaleLetter < 9) // G/G#
	{
		if (scaleLetter == 8)
			sharped = true;
		scaleLetter = 6;
	}
	else if (scaleLetter < 11) // A/A#
	{
		if (scaleLetter == 10)
			sharped = true;
		scaleLetter = 0;
	}
	else // B
	{
		scaleLetter = 1;
	}

	//int keyLetter = (key )

	

	gui.activeUI[int(pos.x)][int(pos.y)].sprite = { scaleLetter, 16 };
	if (betweenNotes < 0.25f)
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 0, 17 + sharped };
	else if (betweenNotes < 0.5f)
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 1, 17 + sharped };
	else if (betweenNotes < 0.75f)
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 2, 17 + sharped };
	else
		gui.activeUI[int(pos.x + 1)][int(pos.y)].sprite = { 3, 17 + sharped };

	return;
}


void DrawFloatingWindow(FloatingWindow* wind)
{
	// Top
	gui.activeUI[int(wind->position.x)][int(wind->position.y)].sprite = { 20, 5 };
	for (int i = wind->position.x + 1; i < wind->position.x + wind->size.x; i++)
		gui.activeUI[i][int(wind->position.y)].sprite = { 21, 5 };

	DrawGUIText(wind->name, int(wind->position.x + 2), int(wind->position.x + 2 + wind->name.length()), int(wind->position.y), 5, 16);

	if (gui.lightMode)
	{
		for (int x = 2; x < wind->name.length() + 2; x++)
			gui.activeUI[int(wind->position.x + x)][int(wind->position.y)].textCol = 5;
	}


	gui.activeUI[int(wind->position.x + wind->size.x - 2)][int(wind->position.y)].sprite = { 29, 9 }; // Exit button
	gui.activeUI[int(wind->position.x + wind->size.x - 1)][int(wind->position.y)].sprite = { 30, 9 };
	gui.activeUI[int(wind->position.x + wind->size.x - 2)][int(wind->position.y)].bgCol = 16;
	gui.activeUI[int(wind->position.x + wind->size.x - 1)][int(wind->position.y)].bgCol = 16;


	gui.activeUI[int(wind->position.x + wind->size.x)][int(wind->position.y)].sprite = { 22, 5 };

	// Body
	for (int y = wind->position.y + 1; y < wind->position.y + wind->size.y - 1; y++)
	{
		gui.activeUI[int(wind->position.x)][y].sprite = {20, 6};
		for (int x = wind->position.x + 1; x < wind->position.x + wind->size.x; x++)
		{
			gui.activeUI[x][y].sprite = { 21, 6 };
		}
		gui.activeUI[int(wind->position.x + wind->size.x)][y].sprite = { 22, 6 };
	}

	// Bottom
	gui.activeUI[int(wind->position.x)][int(wind->position.y + wind->size.y - 1)].sprite = { 11, 4 };
	for (int i = wind->position.x + 1; i < wind->position.x + wind->size.x; i++)
		gui.activeUI[i][int(wind->position.y + wind->size.y - 1)].sprite = { 12, 4 };
	gui.activeUI[int(wind->position.x + wind->size.x)][int(wind->position.y + wind->size.y - 1)].sprite = { 13, 4 };


	// Draw contents
	if (wind->name == "Themes")
	{
		std::string lightModeText = "LIGHT MODE";

		if (gui.lightMode)
		{
			std::string themeText[32] = {
			"PURPLE", "LIGHT BLUE", "LIGHT RED", "LIGHT YELLOW",
			"ORANGE", "AQUATIC", "SUNNY", "BRIGHT",
			"GRASS", "RED BLUE", "RED", "GREY PINK",
			"SATURATED","LOSS", "CREEK", "PLASTIC",
			"LEAVES", "ANOTHER GREEN", "TERRACOTTA", "OFFICE BLUE",
			"OFFICE RED", "LICHEN", "VERY BLUE", "OAK",
			"BRIGHT YELLOW", "BRIGHT PURPLE", "BRIGHT GREEN", "MINT",
			"PAPER ONE", "PAPER TWO", "PAPER THREE", "STONE",
			};

			for (int i = 0; i < 32; i++)
			{
				DrawGUIText(themeText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 1 + i, 4, -1);
				if (i == gui.uiColorTheme)
				{
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1 + i)].sprite = { 5, 4 };
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1 + i)].sprite = { 5, 4 };
				}
				else
				{
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1 + i)].sprite = { 5, 28 };
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1 + i)].sprite = { 6, 28 };
				}
			}

			lightModeText = "DARK MODE";
		}
		else
		{
			std::string themeText[32] = {
			"RED", "BROWN PINK", "CLAY", "MAGNOLIA",
			"PALE", "BLUE", "CLOUDY", "RED SAND",
			"SAND", "AMBER", "BURNT", "WHITE",
			"NEUTRAL GREY","LIGHT BLUE", "CREEK", "TOXIC",
			"RUST", "FOREST", "EMERALD", "BLUE GOLD",
			"LOTIS", "ORANGE", "NAUTICAL", "SILVER",
			"LUNAR", "BLUE GLASS", "STANDARD", "PLASTIC",
			"DEEP SEA", "BRIGHT PINK", "PURPLE", "COSMIC",
			};

			for (int i = 0; i < 32; i++)
			{
				DrawGUIText(themeText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 1 + i, 4, -1);
				if (i == gui.uiColorTheme)
				{
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1 + i)].sprite = { 5, 4 };
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1 + i)].sprite = { 5, 4 };
				}
				else
				{
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1 + i)].sprite = { 5, 28 };
					gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1 + i)].sprite = { 6, 28 };
				}
			}
		}

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1)].sprite = { 5, 27 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 1)].sprite = { 6, 27 };

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 32)].sprite = { 5, 29 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 32)].sprite = { 6, 29 };

		DrawGUIText(lightModeText, wind->position.x + 2, wind->position.x + 15, wind->position.y + 34, 4, -1);
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 34)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 34)].sprite = { 6, 26 };
	}
	else if (wind->name == "Save and Exit")
	{
		DrawGUIText("Would you like to", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("save your song", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 3, 0);
		DrawGUIText("before exiting?", wind->position.x + 2, wind->position.x + 19, wind->position.y + 4, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 6)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 6, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 6)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 8)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 8, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 8)].sprite = { 6, 26 };
	}
	else if (wind->name == "Save and Load")
	{
		DrawGUIText("Would you like to", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("save your current", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 3, 0);
		DrawGUIText("song before", wind->position.x + 2, wind->position.x + 19, wind->position.y + 4, 3, 0);
		DrawGUIText("loading this?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Delete Frames")
	{
		DrawGUIText("This song contains", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 4, 0);
		DrawGUIText("unused frames.", wind->position.x + 2, wind->position.x + 16, wind->position.y + 4, 3, 0);
		DrawGUIText("Would you like ", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);
		DrawGUIText("to delete them?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 6, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Overwrite Song")
	{
		DrawGUIText("This song already", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("exists. Would", wind->position.x + 2, wind->position.x + 16, wind->position.y + 3, 3, 0);
		DrawGUIText("you like to", wind->position.x + 2, wind->position.x + 17, wind->position.y + 4, 3, 0);
		DrawGUIText("Replace it?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Save Song" || wind->name == "Save Instrument" || wind->name == "Save Sample" || wind->name == "Load File" || wind->name == "Export as .WAV")
	{
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 1)].sprite = { 25, 5 };
		DrawGUIText(fileNavigator.filePathName, wind->position.x + 2, wind->position.x + 40, wind->position.y + 1, 4, 2);

		for (int y = 2; y < 19; y++)
		{
			if (y - 2 + fileNavigator.fileListScroll < fileNavigator.fileNames.size())
			{
				if (fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll].at(0) == '0' || fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll].at(0) == '4')
					DrawGUIText(fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 2, 0);
				else if (y - 2 + fileNavigator.fileListScroll == editor.selectedFile)
					DrawGUIText(fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 5, 1);
				else
					DrawGUIText(fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll], wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

				if (fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll].at(0) == '0')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 26, 5 };
				else if (fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll].at(0) == '1')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 27, 5 };
				else if (fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll].at(0) == '2')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 28, 5 };
				else if (fileNavigator.fileNames[y - 2 + fileNavigator.fileListScroll].at(0) == '3')
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 30, 5 };
				else
					gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + y)].sprite = { 29, 5 };
			}
			else
				DrawGUIText("", wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);

			//gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + y)].sprite = { 31, 3 };
		}

		//gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 2)].sprite = { 31, 2 };
		//gui.activeUI[int(wind->position.x + 39)][int(wind->position.y + 18)].sprite = { 31, 4 };


		//gui.scrollBars[4].topLeft.x = wind->position.x + 39;
		//gui.scrollBars[4].topLeft.y = wind->position.y + 2;

		DrawVerticalScrollBar(wind->position.y + 2, wind->position.y + 19, wind->position.x, fileNavigator.fileScrollBar.position);


		if (wind->name == "Export as .WAV")
		{
			DrawGUIText("EXPORT", wind->position.x + 30, wind->position.x + 36, wind->position.y + 19, 4, -1);
			gui.activeUI[int(wind->position.x + 29)][int(wind->position.y + 19)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 36)][int(wind->position.y + 19)].sprite = { 6, 26 };
		}
		else
		{
			if (wind->name == "Load File")
				DrawGUIText("LOAD", wind->position.x + 30, wind->position.x + 34, wind->position.y + 19, 4, -1);
			else
				DrawGUIText("SAVE", wind->position.x + 30, wind->position.x + 34, wind->position.y + 19, 4, -1);

			gui.activeUI[int(wind->position.x + 29)][int(wind->position.y + 19)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 34)][int(wind->position.y + 19)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Settings")
	{
		std::string settingsText[4] = {
			"THEMES", "FISHTANK"
		};

		

		for (int i = 0; i < 2; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);

			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Selection")
	{
		std::string settingsText[5] = {
			"COPY", "PASTE", "DELETE", "TRANSPOSE", "SET SAMPLE"
		};
		for (int i = 0; i < 5; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);

			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Transpose")
	{
		DrawGUIText("Octaves", wind->position.x + 2, wind->position.x + 12, wind->position.y + 2, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 3)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 3)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 3)].sprite = { 10, 3 };
		DrawGUIText("Semitones", wind->position.x + 2, wind->position.x + 12, wind->position.y + 4, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 5)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 5)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 5)].sprite = { 10, 3 };


		DrawEDONote(48, wind->position.x + 2, wind->position.y + 7, 3, 0, -1, -1);
		DrawGUIText(" to ", wind->position.x + 5, wind->position.x + 12, wind->position.y + 7, 3, 0);
		DrawEDONote(editor.transposeValue + 48, wind->position.x + 9, wind->position.y + 7, 3, 0, -1, -1);

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 9)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Help")
	{
		DrawVerticalScrollBar(wind->position.y + 2, wind->position.y + 39, wind->position.x, gui.helpWindowScrollBar.position);

		int helpPageScroll = gui.helpWindowScroll;

		for (int y = 2; y < 39; y++)
		{
			if (y - 2 + helpPageScroll < helpPageText.size())
			{
				if (helpPageText[y - 2 + helpPageScroll][0] == '#')
				{
					for (int x = 1; x < 42; x++)
					{
						gui.activeUI[int(wind->position.x + x)][int(wind->position.y + y)].sprite = { 21, 0 };
					}
				}
				else
					DrawGUIText(helpPageText[y - 2 + helpPageScroll], wind->position.x + 1, wind->position.x + 42, wind->position.y + y, 3, 0);
			}
			else
				DrawGUIText("", wind->position.x + 1, wind->position.x + 39, wind->position.y + y, 3, 0);
		}
	}
	else if (wind->name == "Overwrite WAV")
	{
		DrawGUIText("This file already", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("exists. Would", wind->position.x + 2, wind->position.x + 16, wind->position.y + 3, 3, 0);
		DrawGUIText("you like to", wind->position.x + 2, wind->position.x + 17, wind->position.y + 4, 3, 0);
		DrawGUIText("Replace it?", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 7)].sprite = { 5, 26 };
		DrawGUIText("YES", wind->position.x + 8, wind->position.x + 12, wind->position.y + 7, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 7)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 7)][int(wind->position.y + 9)].sprite = { 5, 26 };
		DrawGUIText(" NO", wind->position.x + 8, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };
	}
	else if (wind->name == "Instrument Editor")
	{
		if (!loadedInstruments[editor.selectedInstrument].enabled)
		{
			DrawGUIText("(NO INSTRUMENT SELECTED)", wind->position.x + 5, wind->position.x + 30, wind->position.y + 5, 3, 0);
			DrawGUIText("Select an instrument in", wind->position.x + 5, wind->position.x + 30, wind->position.y + 8, 3, 0);
			DrawGUIText("the instrument menu.", wind->position.x + 5, wind->position.x + 30, wind->position.y + 9, 3, 0);
			return;
		}


		sampleDisplay.position.x = wind->position.x + 17.0f;
		sampleDisplay.position.y = wind->position.y + 31.0f;


		// Sample name
		DrawGUIText("INSTRUMENT: ", wind->position.x + 1, wind->position.x + 17, wind->position.y + 1, 3, 1);
		DrawGUIText(loadedInstruments[editor.selectedInstrument].name, wind->position.x + 12, wind->position.x + 34, wind->position.y + 1, 3, 1);

		// Volume/Arp speed
		if (sampleDisplay.displayArp)
		{
			DrawGUIText("Speed:", wind->position.x + 18, wind->position.x + 25, wind->position.y + 2, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 25), int(wind->position.x + 33), int(wind->position.y + 2), loadedInstruments[editor.selectedInstrument].arpSpeed);
		}

		// waveform or sample.
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 2)].sprite = { 5, 26 };
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType == 0)
			DrawGUIText("WAVEFORM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 2, 4, -1);
		else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
			DrawGUIText("SAMPLE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 2, 4, -1);
		else
			DrawGUIText("ADDITIVE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 2, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 2)].sprite = { 6, 26 };


		// Fuzz
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 3)].sprite = { 5, 26 };
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzzType == 0) // Clip
			DrawGUIText("CLIP", wind->position.x + 2, wind->position.x + 6, wind->position.y + 3, 4, -1);
		else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzzType == 1) // Fold
			DrawGUIText("FOLD", wind->position.x + 2, wind->position.x + 6, wind->position.y + 3, 4, -1);
		else // Ring fold
			DrawGUIText("ABS", wind->position.x + 2, wind->position.x + 6, wind->position.y + 3, 4, -1);

		gui.activeUI[int(wind->position.x + 6)][int(wind->position.y + 3)].sprite = { 6, 26 };

		//DrawGUIText("Fuzz:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 3, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 3), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzz);

		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
		{
			//DrawGUIText("SHAPE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 3, 3, 0
			DrawWaveTypeButton(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType, int(wind->position.x) + 3, int(wind->position.y) + 4);

			// Reverse wave toggle.
			if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].reverseFrames)
			{
				DrawGUIText("Reverse:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 11, 3, 0);
				gui.activeUI[int(wind->position.x + 9)][int(wind->position.y + 11)].sprite = { 24, 6 };
			}
			else
			{
				DrawGUIText("Reverse:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 11, 2, 0);
				gui.activeUI[int(wind->position.x + 9)][int(wind->position.y + 11)].sprite = { 23, 6 };
			}

			// Offset
			DrawGUIText("Offset:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 13, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 13), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].offset);
		}

		// Draw frequency/arp display.
		for (int x = 0; x < 15; x++)
		{
			if (sampleDisplay.displayArp)
			{
				if (x > loadedInstruments[editor.selectedInstrument].arpLength)
				{
					for (int y = 1; y < 14; y++)
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 10, 19 };
				}
				else
				{
					for (int y = 1; y < 14; y++)
					{
						int offsetSprite = 0;

						float arpVal = loadedInstruments[editor.selectedInstrument].arpPitches[x];

						bool onOct = (y == 13 || y == 10 || y == 7 || y == 4 || y == 1);
						bool overOct = (y - 1 == 13 || y - 1 == 10 || y - 1 == 7 || y - 1 == 4 || y - 1 == 1);

						if (y == int(arpVal))
						{
							if (arpVal - y == 0.0f)
							{
								offsetSprite++;
								if (overOct)
									offsetSprite += 5;
								if (y > 0)
									gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y + 1)].sprite = { 11 + offsetSprite, 15 };
							}
							else if (arpVal - y == 0.25f)
							{
								offsetSprite += 2;
								if (overOct)
									offsetSprite += 5;
								if (y > 0)
									gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y + 1)].sprite = { 11 + offsetSprite, 15 };
							}
							else if (arpVal - y == 0.5f)
							{
								offsetSprite += 3;
								if (overOct)
									offsetSprite += 5;
								if (y > 0)
									gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y + 1)].sprite = { 11 + offsetSprite, 15 };
							}
							else if (arpVal - y == 0.75f)
							{
								if (overOct)
									offsetSprite += 5;
								offsetSprite += 4;
							}

							if (overOct)
								offsetSprite -= 5;
						}

						if (onOct)
							offsetSprite += 5;

						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 11 + offsetSprite, 16 };
					}

					if (x == loadedInstruments[editor.selectedInstrument].arpLength)
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16)].sprite = { 9, 19 };
					else
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16)].sprite = { 8, 19 };
				}
			}
			else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1 && x > 3)
			{
				int offsetSprite = 0;
				if (x == 0 || x == 4 || x == 6 || x == 8 || x == 10 || x == 14)
					offsetSprite = 1;
				else if (x == 7)
					offsetSprite = 2;


				for (int y = 0; y < 14; y++)
				{
					if (y < loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4])
					{
						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4] - y == 0.25)
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 7, 20 + offsetSprite };
						else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4] - y == 0.5)
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 8, 20 + offsetSprite };
						else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[x - 4] - y == 0.75)
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 9, 20 + offsetSprite };
						else
							gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 10, 20 + offsetSprite };
					}
					else
					{
						gui.activeUI[int(wind->position.x + 18 + x)][int(wind->position.y + 16 - y)].sprite = { 6, 20 + offsetSprite };
					}
				}
			}
		}
		
		if (!sampleDisplay.displayArp)
		{
			for (int y = 0; y < 14; y++)
			{
				gui.activeUI[int(wind->position.x + 21)][int(wind->position.y + y + 3)].sprite = { 6, 23 };
				gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + y + 3)].sprite = { 7, 23 };
			}
		}
		//gui.activeUI[int(wind->position.x + 22)][int(wind->position.y + 16)].sprite.y += 3;
		//gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 16)].sprite.y += 2;

		// Draw the play and pause buttons.
		if (sampleDisplay.playingInstrument)
		{
			gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 38].sprite = { 14, 7 };
			gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 39].sprite = { 14, 8 };
			gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 38].sprite = { 15, 7 };
			gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 39].sprite = { 15, 8 };

			gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 38].sprite = { 16, 7 };
			gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 39].sprite = { 16, 8 };
			gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 38].sprite = { 17, 7 };
			gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 39].sprite = { 17, 8 };
		}
		else
		{
			gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 38].sprite = { 12, 7 };
			gui.activeUI[int(wind->position.x) + 1][int(wind->position.y) + 39].sprite = { 12, 8 };
			gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 38].sprite = { 13, 7 };
			gui.activeUI[int(wind->position.x) + 2][int(wind->position.y) + 39].sprite = { 13, 8 };

			gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 38].sprite = { 18, 7 };
			gui.activeUI[int(wind->position.x) + 3][int(wind->position.y) + 39].sprite = { 18, 8 };
			gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 38].sprite = { 19, 7 };
			gui.activeUI[int(wind->position.x) + 4][int(wind->position.y) + 39].sprite = { 19, 8 };
		}
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
		{
			if (sampleDisplay.displayType == 0)
			{
				if (sampleDisplay.drawing) // Draw the drawing button.
				{
					gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 38].sprite = { 19, 3 };
					gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 39].sprite = { 19, 4 };
					gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 38].sprite = { 20, 3 };
					gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 39].sprite = { 20, 4 };
				}
				else
				{
					gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 38].sprite = { 17, 3 };
					gui.activeUI[int(wind->position.x) + 26][int(wind->position.y) + 39].sprite = { 17, 4 };
					gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 38].sprite = { 18, 3 };
					gui.activeUI[int(wind->position.x) + 27][int(wind->position.y) + 39].sprite = { 18, 4 };
				}
				if (sampleDisplay.zoomed) // Draw the zoom button.
				{
					gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 38].sprite = { 30, 7 };
					gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 39].sprite = { 30, 8 };
					gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 38].sprite = { 31, 7 };
					gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 39].sprite = { 31, 8 };
				}
				else
				{
					gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 38].sprite = { 28, 7 };
					gui.activeUI[int(wind->position.x) + 28][int(wind->position.y) + 39].sprite = { 28, 8 };
					gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 38].sprite = { 29, 7 };
					gui.activeUI[int(wind->position.x) + 29][int(wind->position.y) + 39].sprite = { 29, 8 };
				}
			}
		}


		

		// Continue note
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].continueNote)
		{
			DrawGUIText("Continue Note:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 10, 3, 0);
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 10)].sprite = { 24, 6 };
		}
		else
		{
			DrawGUIText("Continue Note:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 10, 2, 0);
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 10)].sprite = { 23, 6 };
		}



		// Pitch to note
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pitchToNote)
		{
			DrawGUIText("Pitch to Note:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 12, 3, 0);
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 12)].sprite = { 24, 6 };
		}
		else
		{
			DrawGUIText("Pitch to Note:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 12, 2, 0);
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 12)].sprite = { 23, 6 };
		}


		// Loop button
		//DrawGUIText("Periods:", wind->position.x + 1, wind->position.x + 9, wind->position.y + 8, 3, 0);
		//DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.y + 8), float(loadedInstruments[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].periods) / 16.0f);

		// Arpeggio
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].useArp)
		{
			DrawGUIText("Arpeggio:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 8, 3, 0);
			gui.activeUI[int(wind->position.x + 10)][int(wind->position.y + 8)].sprite = { 24, 6 };
		}
		else
		{
			DrawGUIText("Arpeggio:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 8, 2, 0);
			gui.activeUI[int(wind->position.x + 10)][int(wind->position.y + 8)].sprite = { 23, 6 };
		}

		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 9)].sprite = { 5, 26 };
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopType == 0)
			DrawGUIText("NO LOOP", wind->position.x + 3, wind->position.x + 12, wind->position.y + 9, 4, -1);
		else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopType == 1)
			DrawGUIText("LOOP", wind->position.x + 3, wind->position.x + 12, wind->position.y + 9, 4, -1);
		else
			DrawGUIText("BOUNCE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 9, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 9)].sprite = { 6, 26 };





		// ASDR
		/*
		DrawGUIText("Attack:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 12, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 12), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].attack);
		DrawGUIText("Sustain:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 13, 3, 0);
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].sustainForever)
			gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 13)].sprite = { 5, 22 };
		else
		{
			gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 13)].sprite = { 4, 22 };
			DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 13), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].sustain);
		}
		DrawGUIText("Decay:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 14, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 14), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].decay);
		DrawGUIText("Release:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 15, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 15), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release);
		*/

		// Octave
		DrawGUIText("Octave:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 14, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 14), float(15 - loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].octave) / 16.0f);

		
		// Stereo
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].invertStereo)
		{
			DrawGUIText("Invert Stereo:", wind->position.x + 1, wind->position.x + 15, wind->position.y + 15, 3, 0);
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 15)].sprite = { 24, 6 };
		}
		else
		{
			DrawGUIText("Invert Stereo:", wind->position.x + 1, wind->position.x + 15, wind->position.y + 15, 2, 0);
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 15)].sprite = { 23, 6 };
		}

		// Release
		if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].noSustain)
		{
			DrawGUIText("Stop:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 16, 2, 0);
			gui.activeUI[int(wind->position.x + 6)][int(wind->position.y + 16)].sprite = { 23, 6 };
		}
		else
		{
			DrawGUIText("Stop:", wind->position.x + 1, wind->position.x + 16, wind->position.y + 16, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 16), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release);
			gui.activeUI[int(wind->position.x + 6)][int(wind->position.y + 16)].sprite = { 24, 6 };
		}
		


		// Instrument parameter division bar.
		for (int x = 1; x < 18; x++)
			gui.activeUI[int(wind->position.x + x)][int(wind->position.y + 17)].sprite = { 21, 0 };

		gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + 17)].sprite = { 21, 0 };

		// Volume
		DrawGUIText("Volume:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 18, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 18), loadedInstruments[editor.selectedInstrument].volume);

		// Glide
		DrawGUIText("Glide:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 19, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 19), loadedInstruments[editor.selectedInstrument].glide);

		// Scatter
		DrawGUIText("Scatter:", wind->position.x + 1, wind->position.x + 10, wind->position.y + 20, 3, 0);
		DrawHorizontalSlider(int(wind->position.x + 9), int(wind->position.x + 17), int(wind->position.y + 20), loadedInstruments[editor.selectedInstrument].scatter);


		

		//gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 20)].sprite = { 5, 26 };
		//DrawGUIText(std::to_string(12 - int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].lfo)), wind->position.x + 9, wind->position.x + 11, wind->position.y + 20, 4, -1);
		//gui.activeUI[int(wind->position.x + 11)][int(wind->position.y + 20)].sprite = { 6, 26 };
		// Continue note
		


		// Operator modulation type.
		for (int mod = 0; mod < 4; mod++)
		{
			gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 21 + mod)].sprite = { 30, 4 };
			
			if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 0)
				DrawGUIText("FM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 1)
				DrawGUIText("AM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 2)
				DrawGUIText("DELAY", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 3)
				DrawGUIText("LP-CUTOFF", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 4)
				DrawGUIText("LP-RESO.", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 5)
				DrawGUIText("LP-BOTH", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 6)
				DrawGUIText("FUZZ", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 7)
				DrawGUIText("PM", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 8)
				DrawGUIText("BIT DEPTH", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 9)
				DrawGUIText("ENVELOPE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 10)
				DrawGUIText("H-PASS", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);
			else if (loadedInstruments[editor.selectedInstrument].modulationTypes[mod] == 11)
				DrawGUIText("SAMPLE RATE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 21 + mod, 4, 0);

			DrawHorizontalSlider(int(wind->position.x + 13), int(wind->position.x + 21), wind->position.y + 21 + mod, loadedInstruments[editor.selectedInstrument].modScale[mod]);
			gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 21 + mod)].sprite = { 30, 3 };
		}

		



		// Frame/Envelop view toggle button.
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 25)].sprite = { 5, 26 };
		if (sampleDisplay.displayType == 0)
			DrawGUIText("FRAMES", wind->position.x + 3, wind->position.x + 12, wind->position.y + 25, 4, -1);
		else
			DrawGUIText("ENVELOPE", wind->position.x + 3, wind->position.x + 12, wind->position.y + 25, 4, -1);
		gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 25)].sprite = { 6, 26 };




		// Frequency/arp toggle button.
		gui.activeUI[int(wind->position.x + 18)][int(wind->position.y + 17)].sprite = { 5, 26 };
		if (sampleDisplay.displayArp)
			DrawGUIText("ARPEGGIO", wind->position.x + 19, wind->position.x + 32, wind->position.y + 17, 4, -1);
		else
			DrawGUIText("FREQUENCIES", wind->position.x + 19, wind->position.x + 32, wind->position.y + 17, 4, -1);
		gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 17)].sprite = { 6, 26 };



		DrawAlgorithm({ wind->position.x + 24, wind->position.y + 18 }, loadedInstruments[editor.selectedInstrument].algorithmType, true);

		gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 18)].sprite = { 8, 8 };
		gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 19)].sprite = { 9, 8 };
		gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 20)].sprite = { 10, 8 };
		gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 21)].sprite = { 11, 8 };

		for (int y = 0; y < 4; y++)
		{
			if (sampleDisplay.selectedOperator == y)
				gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 18 + y)].textCol = 5;
			else
				gui.activeUI[int(wind->position.x + 32)][int(wind->position.y + 18 + y)].textCol = 0;
		}
		

		gui.activeUI[int(wind->position.x + 22)][int(wind->position.y + 23)].sprite = { 5, 26 };
		DrawGUIText("INSTRUMENTS", wind->position.x + 23, wind->position.x + 33, wind->position.y + 23, 4, -1);
		gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + 23)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 22)][int(wind->position.y + 24)].sprite = { 5, 26 };
		DrawGUIText("ALGORITHMS", wind->position.x + 23, wind->position.x + 33, wind->position.y + 24, 4, -1);
		gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + 24)].sprite = { 6, 26 };

		gui.activeUI[int(wind->position.x + 22)][int(wind->position.y + 25)].sprite = { 5, 26 };
		DrawGUIText("PRESETS", wind->position.x + 23, wind->position.x + 33, wind->position.y + 25, 4, -1);
		gui.activeUI[int(wind->position.x + 33)][int(wind->position.y + 25)].sprite = { 6, 26 };


		//gui.scrollBars[5].topLeft.x = wind->position.x + 1;
		//gui.scrollBars[5].topLeft.y = wind->position.y + 27;

		

		//if (sampleDisplay.offset >= sampleDisplay.pcmData.size() * 0.5f - (960 / sampleDisplay.zoomFactor))
		//	sampleDisplay.offset = sampleDisplay.pcmData.size() * 0.5f - (960 / sampleDisplay.zoomFactor) - 1;
		//if (sampleDisplay.offset < 0)
		//	sampleDisplay.offset = 0;
		

		//DrawSampleDisplay();


		if (sampleDisplay.displayType == 0) // Sample display
		{
			// Snap button
			DrawGUIText("Snap:" + std::to_string(sampleDisplay.snapSubdivisions), wind->position.x + 6, wind->position.x + 14, wind->position.y + 38, 3, 0);

			if (sampleDisplay.enableSnap)
				gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + 38)].sprite = { 24, 6 };
			else
				gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + 38)].sprite = { 23, 6 };

			DrawHorizontalSlider(int(wind->position.x + 14), int(wind->position.x + 22), int(wind->position.y + 38), float(sampleDisplay.snapSubdivisions) / 64.0f);


			// Sample length display
			int frameCount = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

			gui.activeUI[int(wind->position.x + 6)][int(wind->position.y + 39)].sprite = { 5, 26 };
			if (sampleDisplay.measurementSystem == 0)
				DrawGUIText("FRAMES:" + std::to_string(frameCount), wind->position.x + 7, wind->position.x + 24, wind->position.y + 39, 4, -1);
			else if (sampleDisplay.measurementSystem == 1)
			{
				float milis = (frameCount / 48000.0f) * 1000.0f;
				DrawGUIText("MS:" + std::to_string(milis), wind->position.x + 7, wind->position.x + 24, wind->position.y + 39, 4, -1);
			}
			else
			{
				float seconds = (frameCount) / 48000.0f;
				DrawGUIText("SECONDS:" + std::to_string(seconds), wind->position.x + 7, wind->position.x + 24, wind->position.y + 39, 4, -1);
			}
			gui.activeUI[int(wind->position.x + 24)][int(wind->position.y + 39)].sprite = { 6, 26 };
		}
		else if (sampleDisplay.displayType == 1) // Envelope display
		{
			DrawGUIText("Size:" + std::to_string(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength), wind->position.x + 5, wind->position.x + 14, wind->position.y + 38, 3, 0);
			DrawHorizontalSlider(int(wind->position.x + 12), int(wind->position.x + 32), int(wind->position.y + 38), float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength - 1.0f) / 80.0f);

			gui.activeUI[int(wind->position.x + 5)][int(wind->position.y + 39)].sprite = { 5, 26 };
			if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale == 0.25f)
				DrawGUIText("SCALE:4", wind->position.x + 6, wind->position.x + 15, wind->position.y + 39, 4, -1);
			else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale == 0.5f)
				DrawGUIText("SCALE:2", wind->position.x + 6, wind->position.x + 15, wind->position.y + 39, 4, -1);
			else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale == 1.0f)
				DrawGUIText("SCALE:1", wind->position.x + 6, wind->position.x + 15, wind->position.y + 39, 4, -1);
			else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale == 2.0f)
				DrawGUIText("SCALE:.5", wind->position.x + 6, wind->position.x + 15, wind->position.y + 39, 4, -1);
			else
				DrawGUIText("SCALE:.25", wind->position.x + 6, wind->position.x + 15, wind->position.y + 39, 4, -1);
			
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 39)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Sample Selection")
	{
		std::string settingsText[8] = {
			"CUT", "RESAMPLE", "VOLUME", "SET LOOP", "FADE OUT", "FADE IN", "COPY", "PASTE"
		};
		for (int i = 0; i < 8; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Resample Sample")
	{
		DrawGUIText("Fine-tune", wind->position.x + 2, wind->position.x + 12, wind->position.y + 2, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 3)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 3)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 3)].sprite = { 10, 3 };
		DrawGUIText(std::to_string(sampleDisplay.resampleFineTune), wind->position.x + 6, wind->position.x + 16, wind->position.y + 3, 3, 0);
		DrawGUIText("Semitones", wind->position.x + 2, wind->position.x + 12, wind->position.y + 4, 3, 0);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 5)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 5)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 5)].sprite = { 10, 3 };
		DrawGUIText("C-4 to ", wind->position.x + 6, wind->position.x + 13, wind->position.y + 5, 3, 0);
		DrawEDONote(sampleDisplay.resampleNote, wind->position.x + 13, wind->position.y + 5, 3, 0, -1, -1);
		
		float selectedFrameSize = abs(sampleDisplay.sampleSelectionEnd - sampleDisplay.sampleStartPos);
		DrawGUIText(std::to_string(int(selectedFrameSize)) + " frames", wind->position.x + 2, wind->position.x + 20, wind->position.y + 7, 3, 0);
		DrawGUIText("to ", wind->position.x + 2, wind->position.x + 20, wind->position.y + 8, 3, 0);

		
		selectedFrameSize *= sampleDisplay.resampleMultiplier;

		DrawNum(selectedFrameSize, wind->position.x + 2, wind->position.x + 20, wind->position.y + 9, 4, 0, -1, 28);

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 11, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 11)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 11)].sprite = { 6, 26 };
	}
	else if (wind->name == "Sample Volume")
	{
		DrawGUIText("Volume", wind->position.x + 2, wind->position.x + 12, wind->position.y + 2, 4, 1);
		gui.activeUI[int(wind->position.x + 2)][int(wind->position.y + 3)].sprite = { 8, 3 };
		gui.activeUI[int(wind->position.x + 3)][int(wind->position.y + 3)].sprite = { 9, 3 };
		gui.activeUI[int(wind->position.x + 4)][int(wind->position.y + 3)].sprite = { 10, 3 };
		DrawGUIText(std::to_string(sampleDisplay.volumeMultiplier), wind->position.x + 6, wind->position.x + 16, wind->position.y + 3, 4, 1);

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 5, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 5)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 5)].sprite = { 6, 26 };
	}
	else if (wind->name == "Algorithms")
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (sampleDisplay.operatorMenuSelectedOperator == x + 4 * y)
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, true);
				else
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, false);
			}
		}

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 32, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 32)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 32)].sprite = { 6, 26 };
	}
	else if (wind->name == "Presets")
	{
		for (int i = 0; i < 6; i++)
		{
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 * i + 1)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 12)][int(wind->position.y + 2 * i + 1)].sprite = { 6, 26 };
		}
		DrawGUIText("LEADS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 1, 4, -1);
		DrawGUIText("PADS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 3, 4, -1);
		DrawGUIText("BASS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 5, 4, -1);
		DrawGUIText("KEYS", wind->position.x + 2, wind->position.x + 12, wind->position.y + 7, 4, -1);
		DrawGUIText("PERCUSSION", wind->position.x + 2, wind->position.x + 12, wind->position.y + 9, 4, -1);
		DrawGUIText("SFX", wind->position.x + 2, wind->position.x + 12, wind->position.y + 11, 4, -1);

		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 * presetMenu.instrumentType + 1)].sprite = { 5, 4 };

		for (int i = 0; i < 16; i++)
		{
			gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + i + 1)].sprite = { 2, 4 };
			gui.activeUI[int(wind->position.x + 30)][int(wind->position.y + i + 1)].sprite = { 2, 4 };

			//if (i == presetMenu.selectedSample)
			//	DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i], wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 5, 2);
			//else
			//	DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i], wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 3, 0);
			//if (i + 16 == presetMenu.selectedSample)
			//	DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i + 16], wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 5, 2);
			//else
			//	DrawGUIText(presetMenu.presetNames[presetMenu.instrumentType * 32 + i + 16], wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 3, 0);
		
			///////////////////////////////////////
			if (i < presetMenu.fileNames.size())
			{
				if (i == presetMenu.selectedSample)
					DrawGUIText(presetMenu.fileNames[i], wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 5, 2);
				else
					DrawGUIText(presetMenu.fileNames[i], wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 3, 0);
			}
			else
			{
				if (i == presetMenu.selectedSample)
					DrawGUIText("", wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 5, 2);
				else
					DrawGUIText("", wind->position.x + 14, wind->position.x + 30, wind->position.y + i + 1, 3, 0);
			}

			if (i + 16 < presetMenu.fileNames.size())
			{
				if (i + 16 == presetMenu.selectedSample)
					DrawGUIText(presetMenu.fileNames[i + 16], wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 5, 2);
				else
					DrawGUIText(presetMenu.fileNames[i + 16], wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 3, 0);
			}
			else
			{
				if (i + 16 == presetMenu.selectedSample)
					DrawGUIText("", wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 5, 2);
				else
					DrawGUIText("", wind->position.x + 31, wind->position.x + 48, wind->position.y + i + 1, 3, 0);
			}
			///////////////////////////////////////
		
		}

		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 17)].sprite = { 5, 26 };
		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 17, 4, -1);
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 17)].sprite = { 6, 26 };
	}
	else if (wind->name == "Algorithms")
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (sampleDisplay.operatorMenuSelectedOperator == x + 4 * y)
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, true);
				else
					DrawAlgorithm({ wind->position.x + x * 8 + 2, wind->position.y + y * 6 + 2 }, x + 4 * y, false);
			}
		}

		DrawGUIText("APPLY", wind->position.x + 9, wind->position.x + 14, wind->position.y + 32, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 32)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 14)][int(wind->position.y + 32)].sprite = { 6, 26 };
	}
	else if (wind->name == "File")
	{
		std::string settingsText[6] = {
			"NEW", "SAVE SONG", "SAVE INSTR.", "SAVE SAMPLE", "LOAD", "EXPORT"
		};
		for (int i = 0; i < 6; i++)
		{
			DrawGUIText(settingsText[i], wind->position.x + 2, wind->position.x + 15, wind->position.y + 2 + i * 2, 4, -1);
			gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 2 + i * 2)].sprite = { 5, 26 };
			gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 2 + i * 2)].sprite = { 6, 26 };
		}
	}
	else if (wind->name == "Newer Format")
	{
		DrawGUIText("This song uses a", wind->position.x + 2, wind->position.x + 19, wind->position.y + 2, 3, 0);
		DrawGUIText("newer format. It", wind->position.x + 2, wind->position.x + 19, wind->position.y + 3, 3, 0);
		DrawGUIText("is incompatible", wind->position.x + 2, wind->position.x + 19, wind->position.y + 4, 3, 0);
		DrawGUIText("with this", wind->position.x + 2, wind->position.x + 17, wind->position.y + 5, 3, 0);
		DrawGUIText("program version.", wind->position.x + 2, wind->position.x + 17, wind->position.y + 6, 3, 0);
	}
	else if (wind->name == "Fishtank")
	{
		gui.activeUI[int(wind->position.x)][int(wind->position.y + 1)].sprite = { 11, 3 };
		for (int x = 1; x < 16; x++)
			gui.activeUI[int(wind->position.x) + x][int(wind->position.y + 1)].sprite = { 12, 3 };
		gui.activeUI[int(wind->position.x + 16)][int(wind->position.y + 1)].sprite = { 13, 3 };


		gui.activeUI[int(wind->position.x)][int(wind->position.y + 8)].sprite = { 28, 24 };
		gui.activeUI[int(wind->position.x)][int(wind->position.y + 9)].sprite = { 28, 25 };
		gui.activeUI[int(wind->position.x)][int(wind->position.y + 10)].sprite = { 28, 25 };
		gui.activeUI[int(wind->position.x)][int(wind->position.y + 11)].sprite = { 28, 26 };

		for (int y = 2; y < 9; y++)
			gui.activeUI[int(wind->position.x) + 1][int(wind->position.y + y)].sprite = { 20, 6 };
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 9)].sprite = { 28, 24 };
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 10)].sprite = { 28, 25 };
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 11)].sprite = { 28, 26 };
		gui.activeUI[int(wind->position.x + 1)][int(wind->position.y + 12)].sprite = { 11, 4 };

		for (int y = 2; y < 9; y++)
			gui.activeUI[int(wind->position.x) + 15][int(wind->position.y + y)].sprite = { 22, 6 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 9)].sprite = { 29, 24 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 10)].sprite = { 29, 25 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 11)].sprite = { 29, 26 };
		gui.activeUI[int(wind->position.x + 15)][int(wind->position.y + 12)].sprite = { 13, 4 };

		gui.activeUI[int(wind->position.x) + 16][int(wind->position.y + 8)].sprite = { 29, 24 };
		gui.activeUI[int(wind->position.x) + 16][int(wind->position.y + 9)].sprite = { 29, 25 };
		gui.activeUI[int(wind->position.x) + 16][int(wind->position.y + 10)].sprite = { 29, 25 };
		gui.activeUI[int(wind->position.x) + 16][int(wind->position.y + 11)].sprite = { 29, 26 };


		// Sand
		for (int x = 2; x < 15; x++)
		{
			gui.activeUI[int(wind->position.x) + x][int(wind->position.y + 11)].sprite = { 31, 23 };
			gui.activeUI[int(wind->position.x) + x][int(wind->position.y + 10)].sprite = { 28, 27 };
		}

		gui.activeUI[int(wind->position.x) + 2][int(wind->position.y + 9)].sprite = { 28, 23 };

		gui.activeUI[int(wind->position.x) + 3][int(wind->position.y + 9)].sprite = { 29, 23 };
		gui.activeUI[int(wind->position.x) + 4][int(wind->position.y + 9)].sprite = { 29, 23 };
		gui.activeUI[int(wind->position.x) + 5][int(wind->position.y + 9)].sprite = { 29, 23 };

		gui.activeUI[int(wind->position.x) + 6][int(wind->position.y + 9)].sprite = { 30, 23 };
		gui.activeUI[int(wind->position.x) + 7][int(wind->position.y + 10)].sprite = { 30, 23 };
		gui.activeUI[int(wind->position.x) + 8][int(wind->position.y + 10)].sprite = { 28, 23 };
		gui.activeUI[int(wind->position.x) + 9][int(wind->position.y + 10)].sprite = { 29, 23 };
		gui.activeUI[int(wind->position.x) + 10][int(wind->position.y + 9)].sprite = { 28, 23 };

		gui.activeUI[int(wind->position.x) + 11][int(wind->position.y + 9)].sprite = { 29, 23 };
		gui.activeUI[int(wind->position.x) + 12][int(wind->position.y + 9)].sprite = { 30, 23 };

		gui.activeUI[int(wind->position.x) + 13][int(wind->position.y + 10)].sprite = { 29, 23 };
		gui.activeUI[int(wind->position.x) + 14][int(wind->position.y + 10)].sprite = { 29, 23 };

		// Plants
		for (int y = 4; y < 9; y++)
			gui.activeUI[int(wind->position.x) + 3][int(wind->position.y + y)].sprite = { 30, 24 };
		for (int y = 6; y < 8; y++)
			gui.activeUI[int(wind->position.x) + 4][int(wind->position.y + y)].sprite = { 30, 24 };
		for (int y = 6; y < 10; y++)
			gui.activeUI[int(wind->position.x) + 13][int(wind->position.y + y)].sprite = { 30, 24 };


		// Rocks
		gui.activeUI[int(wind->position.x) + 4][int(wind->position.y + 8)].sprite = { 31, 24 };
		gui.activeUI[int(wind->position.x) + 5][int(wind->position.y + 8)].sprite = { 31, 24 };
		gui.activeUI[int(wind->position.x) + 9][int(wind->position.y + 9)].sprite = { 31, 24 };


		// Fish
		gui.activeUI[int(wind->position.x) + 7][int(wind->position.y + 3)].sprite = { 30, 25 };
		gui.activeUI[int(wind->position.x) + 8][int(wind->position.y + 3)].sprite = { 31, 25 };
		gui.activeUI[int(wind->position.x) + 10][int(wind->position.y + 5)].sprite = { 30, 26 };
		gui.activeUI[int(wind->position.x) + 11][int(wind->position.y + 5)].sprite = { 31, 26 };
	}
	else if (wind->name == "Copy Instrument")
	{
		DrawVerticalScrollBar(int(wind->position.y + 2), int(wind->position.y + 11), int(wind->position.x), instrumentFloatingWindow.instrumentListScrollBar.position);

		
		for (int y = 0; y < 10; y++)
		{
			int instNum = y + instrumentFloatingWindow.instrumentListScroll;
			int instX = int(wind->position.x + 1);
			int instY = int(wind->position.y + y + 1);

			gui.activeUI[instX + 2][instY].sprite = { 2, 4 };

			


			if (instNum < 256 && instNum >= 0) // Draw samples
			{
				if (!loadedInstruments[instNum].enabled)
				{
					if (instNum == instrumentFloatingWindow.selectedInstrument) // 72 - 91
					{
						DrawGUIText("", instX + 3, instX + 22, instY, 5, 3);
						DrawHex(instNum, instX, instY, 4, 3);
						gui.activeUI[instX + 2][instY].sprite = { 5, 4 };

						if (gui.lightMode)
						{
							for (int x = instX; x < instX + 22; x++)
							{
								gui.activeUI[x][instY].textCol = 5;
							}
						}
					}
					else
					{
						DrawGUIText("", instX + 3, instX + 22, instY, 4, 0);
						DrawHex(instNum, instX, instY, 3, 0);
					}
				}
				else
				{
					if (instNum == instrumentFloatingWindow.selectedInstrument)
					{
						if (editor.selectedButton == 8)
						{
							if (gui.lightMode)
								DrawGUIText(loadedInstruments[instNum].name, instX + 3, instX + 22, instY, 5, 2, -1, 8);
							else
								DrawGUIText(loadedInstruments[instNum].name, instX + 3, instX + 22, instY, 5, 1, -1, 8);
						}
						else
							DrawGUIText(loadedInstruments[instNum].name, instX + 3, instX + 22, instY, 5, 3);
						gui.activeUI[instX + 2][instY].sprite = { 5, 4 };
						DrawHex(instNum, instX, instY, 4, 3);

						if (gui.lightMode)
						{
							for (int x = instX; x < instX + 22; x++)
							{
								gui.activeUI[x][instY].textCol = 5;
							}
						}
					}
					else
					{
						DrawGUIText(loadedInstruments[instNum].name, instX + 3, instX + 22, instY, 4, 0);
						DrawHex(instNum, instX, instY, 3, 0);
					}
				}
			}
			else
			{
				DrawGUIText("", instX + 3, instX + 22, instY, 4, 0);
				DrawHex(instNum, instX, instY, 2, 0);
			}
		}

		DrawGUIText("COPY", wind->position.x + 9, wind->position.x + 13, wind->position.y + 11, 4, -1);
		gui.activeUI[int(wind->position.x + 8)][int(wind->position.y + 11)].sprite = { 5, 26 };
		gui.activeUI[int(wind->position.x + 13)][int(wind->position.y + 11)].sprite = { 6, 26 };
	}

	



	return;
}



void DrawWaveTypeButton(int type, int xVal, int yVal)
{
	gui.activeUI[int(xVal - 1)][int(yVal)].sprite = { 5, 26 };
	switch (type)
	{
		case -1:
		{
			DrawGUIText("---------", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 1:
		{
			DrawGUIText("SQUARE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 2:
		{
			DrawGUIText("TRIANGLE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 3:
		{
			DrawGUIText("SAW", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 4:
		{
			DrawGUIText("NOISE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
		case 5:
		{
			DrawGUIText("WAVE A", xVal, xVal + 9, yVal, 4, -1);
			break;
		}

		case 6:
		{
			DrawGUIText("WAVE B", xVal, xVal + 9, yVal, 4, -1);
			break;
		}

		case 7:
		{
			DrawGUIText("WAVE C", xVal, xVal + 9, yVal, 4, -1);
			break;
		}

		default:
		{
			DrawGUIText("SINE", xVal, xVal + 9, yVal, 4, -1);
			break;
		}
	}
	gui.activeUI[int(xVal + 9)][int(yVal)].sprite = { 6, 26 };


	
	if (loadedInstruments[editor.selectedInstrument].enabled)
	{


		DrawGUIText("Duty:" + std::to_string(int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle * 100)), xVal - 2, xVal + 8, yVal + 1, 3, 0);
		DrawHorizontalSlider(int(xVal + 6), int(xVal + 14), int(yVal + 1), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle);

		if (type > 3 && type != 7) // Bell
		{
			DrawGUIText("Waves:" + std::to_string(int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves)), xVal - 2, xVal + 8, yVal + 2, 3, 0);
			DrawHorizontalSlider(int(xVal + 6), int(xVal + 14), int(yVal + 2), float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves) / 16.0f);
		}
		else if (type == 1 || type == 3)
		{
			if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].generateFromSines)
			{
				DrawGUIText("Waves:" + std::to_string(int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves)), xVal - 2, xVal + 8, yVal + 2, 3, 0);
				DrawGUIText("FROM SINE", xVal, xVal + 9, yVal + 3, 4, -1);

				DrawHorizontalSlider(int(xVal + 6), int(xVal + 14), int(yVal + 2), float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves) / 16.0f);
			}
			else
			{
				DrawGUIText("Soft:" + std::to_string(int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness * 100)), xVal - 2, xVal + 8, yVal + 2, 3, 0);
				DrawGUIText("EXACT", xVal, xVal + 9, yVal + 3, 4, -1);

				DrawHorizontalSlider(int(xVal + 6), int(xVal + 14), int(yVal + 2), loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness);
			}

			gui.activeUI[int(xVal - 1)][int(yVal + 3)].sprite = { 5, 26 };
			gui.activeUI[int(xVal + 9)][int(yVal + 3)].sprite = { 6, 26 };
		}
	}


	return;
}



void DrawHorizontalSlider(int xStart, int xEnd, int yVal, float value)
{
	value *= (xEnd - xStart);

	if (value > 0.5f)
		gui.activeUI[xStart][yVal].sprite = { 15, 22 };
	else if (value > 0.0f)
		gui.activeUI[xStart][yVal].sprite = { 14, 22 };
	else
		gui.activeUI[xStart][yVal].sprite = { 13, 22 };

	for (int i = 1; i < (xEnd - xStart) - 1; i++)
	{
		if (i < value)
		{
			if (value - float(i) <= 0.5f)
				gui.activeUI[xStart + i][yVal].sprite = { 12, 21 };
			else
				gui.activeUI[xStart + i][yVal].sprite = { 13, 21 };
		}
		else
		{
			gui.activeUI[xStart + i][yVal].sprite = { 11, 21 };
		}
	}

	if (value > float(xEnd - xStart) - 1.0f)
		gui.activeUI[xEnd - 1][yVal].sprite = { 15, 21 };
	else
		gui.activeUI[xEnd - 1][yVal].sprite = { 14, 21 };

	return;
}


void DrawVerticalSlider(int yStart, int yEnd, int xVal, float value)
{
	value = 1.0f - value;

	value *= (yEnd - yStart);

	if (value > 0.5f)
		gui.activeUI[xVal][yStart].sprite = { 15, 24 };
	else if (value > 0.0f)
		gui.activeUI[xVal][yStart].sprite = { 14, 24 };
	else
		gui.activeUI[xVal][yStart].sprite = { 13, 24 };

	for (int i = 1; i < (yEnd - yStart) - 1; i++)
	{
		if (i < value)
		{
			if (value - float(i) <= 0.5f)
				gui.activeUI[xVal][yStart + i].sprite = { 12, 23 };
			else
				gui.activeUI[xVal][yStart + i].sprite = { 13, 23 };
		}
		else
		{
			gui.activeUI[xVal][yStart + i].sprite = { 11, 23 };
		}
	}

	if (value > float(yEnd - yStart) - 1.0f)
		gui.activeUI[xVal][yEnd - 1].sprite = { 15, 23 };
	else
		gui.activeUI[xVal][yEnd - 1].sprite = { 14, 23 };
}


void DrawHorizontalScrollBar(int xStart, int xEnd, int yVal, float value)
{
	float scrollX = value * float(xEnd - xStart) + xStart + 1.0f;

	if (scrollX < xStart + 1.0f)
		scrollX = xStart + 1.0f;
	else if (scrollX > xEnd)
		scrollX = xEnd;

	for (int x = xStart; x < xEnd; x++)
	{
		gui.activeUI[x][yVal].sprite = { 21, 16 };
	}


	if (scrollX - int(scrollX) < 0.25f)
	{
		gui.activeUI[int(scrollX) - 1][yVal].sprite = { 21, 15 };
	}
	else if (scrollX - int(scrollX) < 0.5f)
	{
		gui.activeUI[int(scrollX) - 1][yVal].sprite = { 24, 16 };
		gui.activeUI[int(scrollX)][yVal].sprite = { 24, 15 };
	}
	else if (scrollX - int(scrollX) < 0.75f)
	{
		gui.activeUI[int(scrollX) - 1][yVal].sprite = { 23, 16 };
		gui.activeUI[int(scrollX)][yVal].sprite = { 23, 15 };
	}
	else
	{
		gui.activeUI[int(scrollX) - 1][yVal].sprite = { 22, 16 };
		gui.activeUI[int(scrollX)][yVal].sprite = { 22, 15 };
	}

	// Scroll arrows
	gui.activeUI[xStart - 1][yVal].sprite = { 30, 4 };
	gui.activeUI[xEnd][yVal].sprite = { 30, 3 };

	return;
}


void DrawVerticalScrollBar(int yStart, int yEnd, int xVal, float value)
{
	float scrollY = value * float(yEnd - yStart) + yStart + 1.0f;

	if (scrollY < yStart + 1.0f)
		scrollY = yStart + 1.0f;
	else if (scrollY > yEnd)
		scrollY = yEnd;

	for (int y = yStart; y < yEnd; y++)
	{
		gui.activeUI[xVal][y].sprite = { 11, 16 };
	}


	if (scrollY - int(scrollY) < 0.25f)
	{
		gui.activeUI[xVal][int(scrollY) - 1].sprite = { 11, 15 };
	}
	else if (scrollY - int(scrollY) < 0.5f)
	{
		gui.activeUI[xVal][int(scrollY) - 1].sprite = { 14, 16 };
		gui.activeUI[xVal][int(scrollY)].sprite = { 14, 15 };
	}
	else if (scrollY - int(scrollY) < 0.75f)
	{
		gui.activeUI[xVal][int(scrollY) - 1].sprite = { 13, 16 };
		gui.activeUI[xVal][int(scrollY)].sprite = { 13, 15 };
	}
	else
	{
		gui.activeUI[xVal][int(scrollY) - 1].sprite = { 12, 16 };
		gui.activeUI[xVal][int(scrollY)].sprite = { 12, 15 };
	}

	// Scroll arrows
	gui.activeUI[xVal][yStart - 1].sprite = { 6, 3 };
	gui.activeUI[xVal][yEnd].sprite = { 6, 4 };

	return;
}



void DrawAlgorithm(Vector2i pos, int type, bool selected)
{
	int operatorWaves[4];
	for (int i = 0; i < 4; i++)
		operatorWaves[i] = loadedInstruments[editor.selectedInstrument].operatorMapping[i];

	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			gui.activeUI[pos.x + x][pos.y + y].sprite = { 4, 7 };
		}
	}

	if (selected)
	{
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (gui.lightMode)
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 4;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 5;
				}
				else
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 1;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 2;
				}
			}
		}
	}
	else
	{
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (gui.lightMode)
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 3;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 4;
				}
				else
				{
					gui.activeUI[pos.x + x][pos.y + y].bgCol = 0;
					gui.activeUI[pos.x + x][pos.y + y].textCol = 1;
				}
			}
		}
	}

	switch (type)
	{
	case 0:
	{
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };
		break;
	}

	case 1:
	{
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 8, 7 };
		break;
	}

	case 2:
	{
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 1, 0 }; // Connectors
		break;
	}

	case 3:
	{
		gui.activeUI[pos.x + 1][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 1][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 9, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 9, 7 };
		gui.activeUI[pos.x + 5][pos.y + 3].sprite = { operatorWaves[2], 7};
		gui.activeUI[pos.x + 5][pos.y + 4].sprite = { 8, 7 };
		break;
	}

	case 4:
	{
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 8, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		break;
	}

	case 5:
	{
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { 8, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 5][pos.y + 2].sprite = { 2, 0 }; // Connectors
		break;
	}

	case 6:
	{
		gui.activeUI[pos.x + 3][pos.y].sprite = { 6, 8 };
		gui.activeUI[pos.x + 4][pos.y].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 2][pos.y].sprite = { 2, 0 }; // Connectors
		break;
	}

	case 7:
	{
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 8 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 9, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { 7, 8 };
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { operatorWaves[1], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 4].sprite = { 8, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 5][pos.y + 2].sprite = { 2, 0 }; // Connectors
		break;
	}

	case 8:
	{
		gui.activeUI[pos.x + 4][pos.y].sprite = { 6, 8 };
		gui.activeUI[pos.x + 5][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { 6, 8 };
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 8 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 2][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 }; // Connectors
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { 2, 0 };
		gui.activeUI[pos.x + 3][pos.y].sprite = { 3, 0 };
		break;
	}

	case 9:
	{
		gui.activeUI[pos.x + 2][pos.y].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { 4, 8 };
		gui.activeUI[pos.x + 4][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { 5, 8 };
		gui.activeUI[pos.x + 3][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 5, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y + 1].sprite = { 2, 0 }; // Connectors
		gui.activeUI[pos.x + 5][pos.y + 1].sprite = { 3, 0 };
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 1, 0 };
		break;
	}

	case 10:
	{
		gui.activeUI[pos.x + 2][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { 5, 7 };
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 2][pos.y + 3].sprite = { 4, 8 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 4][pos.y + 3].sprite = { 5, 8 };

		gui.activeUI[pos.x + 1][pos.y + 1].sprite = { 3, 0 }; // Connectors
		gui.activeUI[pos.x + 1][pos.y + 3].sprite = { 1, 0 };
		gui.activeUI[pos.x + 5][pos.y + 3].sprite = { 2, 0 };

		break;
	}

	case 11:
	{
		gui.activeUI[pos.x + 2][pos.y].sprite = { 6, 8 };
		gui.activeUI[pos.x + 3][pos.y].sprite = { operatorWaves[3], 8};
		gui.activeUI[pos.x + 4][pos.y].sprite = { 7, 8 };
		gui.activeUI[pos.x + 2][pos.y + 1].sprite = { operatorWaves[1], 8};
		gui.activeUI[pos.x + 4][pos.y + 1].sprite = { operatorWaves[2], 8};
		gui.activeUI[pos.x + 2][pos.y + 2].sprite = { 6, 7 };
		gui.activeUI[pos.x + 3][pos.y + 2].sprite = { 7, 7 };
		gui.activeUI[pos.x + 4][pos.y + 2].sprite = { 8, 7 };
		gui.activeUI[pos.x + 3][pos.y + 3].sprite = { operatorWaves[0], 7};
		gui.activeUI[pos.x + 3][pos.y + 4].sprite = { 5, 7 };

		gui.activeUI[pos.x + 1][pos.y].sprite = { 3, 0 }; // Connectors
		gui.activeUI[pos.x + 5][pos.y].sprite = { 4, 0 };
		gui.activeUI[pos.x + 1][pos.y + 2].sprite = { 1, 0 };
		gui.activeUI[pos.x + 5][pos.y + 2].sprite = { 2, 0 };
		break;
	}

	case 12:
	{
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 6, 8 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 7, 8 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { 3, 0 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 2, 0 };
		break;
	}

	case 13:
	{
		gui.activeUI[int(pos.x + 2)][int(pos.y)].sprite = { 6, 8 };
		gui.activeUI[int(pos.x + 3)][int(pos.y)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 1)][int(pos.y + 0)].sprite = { 2, 0 };
		break;
	}

	case 14:
	{
		gui.activeUI[int(pos.x + 1)][int(pos.y + 1)].sprite = { operatorWaves[1], 8};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 8, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 5, 7 };

		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { 2, 0 }; // Connectors
		gui.activeUI[int(pos.x)][int(pos.y + 2)].sprite = { 1, 0 };
		gui.activeUI[int(pos.x + 6)][int(pos.y + 2)].sprite = { 3, 0 };
		break;
	}

	case 15:
	{
		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };


		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 2, 0 };
		break;
	}


	case 16:
	{
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 6, 8 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 2)].sprite = { 7, 8 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 3)].sprite = { operatorWaves[2], 7};
		gui.activeUI[int(pos.x + 5)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 2)][int(pos.y + 1)].sprite = { 2, 0 }; // Connectors
		gui.activeUI[int(pos.x)][int(pos.y + 2)].sprite = { 1, 0 };
		gui.activeUI[int(pos.x + 6)][int(pos.y + 2)].sprite = { 3, 0 };
		break;
	}

	case 17:
	{
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 5, 7 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 3)].sprite = { operatorWaves[2], 7};
		gui.activeUI[int(pos.x + 5)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		break;
	}

	case 18:
	{
		gui.activeUI[int(pos.x + 1)][int(pos.y + 1)].sprite = { operatorWaves[2], 8};
		gui.activeUI[int(pos.x + 3)][int(pos.y + 1)].sprite = { operatorWaves[3], 8};
		gui.activeUI[int(pos.x + 1)][int(pos.y + 2)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 2)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 2)].sprite = { 8, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 8, 7 };

		gui.activeUI[int(pos.x)][int(pos.y + 2)].sprite = { 1, 0 }; // Connectors
		gui.activeUI[int(pos.x + 4)][int(pos.y + 2)].sprite = { 2, 0 };
		break;
	}

	case 19:
	{
		gui.activeUI[int(pos.x + 0)][int(pos.y + 3)].sprite = { operatorWaves[0], 7};
		gui.activeUI[int(pos.x + 0)][int(pos.y + 4)].sprite = { 6, 7 };
		gui.activeUI[int(pos.x + 1)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 2)][int(pos.y + 3)].sprite = { operatorWaves[1], 7};
		gui.activeUI[int(pos.x + 2)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 3)][int(pos.y + 4)].sprite = { 7, 7 };
		gui.activeUI[int(pos.x + 4)][int(pos.y + 3)].sprite = { operatorWaves[2], 7};
		gui.activeUI[int(pos.x + 4)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 5)][int(pos.y + 4)].sprite = { 9, 7 };
		gui.activeUI[int(pos.x + 6)][int(pos.y + 3)].sprite = { operatorWaves[3], 7};
		gui.activeUI[int(pos.x + 6)][int(pos.y + 4)].sprite = { 8, 7 };
		break;
	}


	default:
		break;
	}

	if (gui.lightMode)
	{
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (gui.activeUI[pos.x + x][pos.y + y].sprite.y == 0)
					gui.activeUI[pos.x + x][pos.y + y].textCol = 2;
			}
		}
	}
	else
	{
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (gui.activeUI[pos.x + x][pos.y + y].sprite.y == 0)
					gui.activeUI[pos.x + x][pos.y + y].textCol = 4;
			}
		}
	}

	return;
}