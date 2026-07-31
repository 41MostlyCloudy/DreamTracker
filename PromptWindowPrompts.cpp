#pragma once



#include "GlobalVariables.h"
#include "FileHandling.cpp"

 

//void AnswerQuestion(int question, int answer, GLFWwindow* wind);

void ChangeTheme(int theme);

void ClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void RightClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void HoldClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void RightHoldFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain);

void selectAlgorithmOperator(Vector2 pos);






void ClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{

	clickPos.x = int(clickPos.x);
	clickPos.y = int(clickPos.y);




	if (clickPos.y == 0)
	{
		if (clickPos.x == wind->size.x - 2 || clickPos.x == wind->size.x - 1) // Exit window.
		{
			if (wind->name == "Instrument Editor")
				sampleDisplay.visible = false;
			windowController.windows.erase(windowController.windows.begin() + windowIndex);
			windowController.windows.shrink_to_fit();
			gui.drawUIThisFrame = true;
			gui.drawFrameThisFrame = true;
			screen.mouseDown = false;
			return;
		}
		else // Drag window.
		{
			wind->dragWindow = true;
			wind->dragPoint.x = clickPos.x;
			wind->dragPoint.y = clickPos.y;
		}
		// Exit
	}
	
	{

		if (wind->name == "Themes")
		{
			if (clickPos.y > 0 && clickPos.y < 33)
			{
				ChangeTheme((int(clickPos.y) - 1));
				SaveSettings();
			}
			else if (clickPos.y == 34)
			{
				gui.lightMode = !gui.lightMode;
				LoadGUIThemes();
				ChangeTheme(gui.uiColorTheme);
				SaveSettings();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Save and Exit")
		{
			if (clickPos.y == 6)
			{
				SaveSong();
				glfwSetWindowShouldClose(windMain, true);
			}
			else if (clickPos.y == 8)
				glfwSetWindowShouldClose(windMain, true);
		}
		else if (wind->name == "Save and Load")
		{
			if (clickPos.y == 7)
			{
				SaveSong();
				LoadSong(editor.fileToLoad);
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)

			{
				loadedSong.unsavedChanges = false;
				LoadSong(editor.fileToLoad);
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Delete Frames")
		{
			if (clickPos.y == 7)
			{
				// Check for unused frames.
				std::vector <bool> frameUsed;
				for (int i = 0; i < loadedSong.patterns.size(); i++)
					frameUsed.emplace_back(false);

				for (int i = 0; i < loadedSong.patternSequence.size(); i++)
				{
					frameUsed[loadedSong.patternSequence[i]] = true;
				}

				std::vector <PatternIndexTable> newFrames;
				for (int i = 0; i < frameUsed.size(); i++)
				{
					if (!frameUsed[i])
					{
						PatternIndexTable newPattern;
						loadedSong.patterns[i] = newPattern;
					}
				}

				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();


				SaveSong();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)
			{
				SaveSong();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Overwrite Song")
		{
			if (clickPos.y == 7)
			{
				loadedSong.overWriteOldSong = true;
				SaveSong();
				loadedSong.overWriteOldSong = false;
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Load File")
		{

			if (clickPos.y == 1 && clickPos.x == 1)
			{
				fileNavigator.ExitFile();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 19 && clickPos.x > 28 && clickPos.x < 35) // Load.
			{
				if (editor.selectedFile < 0 || editor.selectedFile >= fileNavigator.fileNames.size())
					return;

				if (fileNavigator.fileNames[editor.selectedFile].at(0) == '1') // Load Song
				{
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					std::string fileN = fileNavigator.fileNames[editor.selectedFile];
					fileN.erase(0, 1);
					LoadSong(fileN);
				}
				else if (fileNavigator.fileNames[editor.selectedFile].at(0) == '2') // Load Sample
				{
					std::string fileN = fileNavigator.fileNames[editor.selectedFile];
					fileN.erase(0, 1);
					std::string filePath = fileNavigator.currentFilePath.std::filesystem::path::string() + "/" + fileN;
					fileN.erase(fileN.size() - 4, 4);
					loadedInstruments[editor.selectedInstrument].name = fileN;
					loadedInstruments[editor.selectedInstrument].enabled = true;



					///////////////////////////////////////////////////////////// Load in the sample frames.
					// Reset all of the frame reading positions since the wave may have changed sizes.
					for (int ch = 0; ch < 8; ch++)
					{
						for (int op = 0; op < 4; op++)
							channels[ch].waveforms[op].sampleReadPos = 0.0f;
					}

					ma_decoder loadingDecoder;
					const char* fPath = &filePath[0];
					ma_decoder_init_file(fPath, &decoderConfig, &loadingDecoder);

					bool reading = true;

					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.clear();
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.shrink_to_fit();
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType = 1;

					while (reading)
					{
						float frames[2048]; // For stereo f32 output, this would be 2048 samples
						ma_uint64 framesRead;

						ma_result result;

						result = ma_decoder_read_pcm_frames(&loadingDecoder, frames, 1024, &framesRead);



						for (int i = 0; i < framesRead * 2; i += 2) // Mix the frames together into 1 channel.
						{
							float mixedFrame = (frames[i] + frames[i + 1]);

							// Scale the frame to the bit depth of the modules.
							int scaledFrame = int(mixedFrame * 128.0f);
							mixedFrame = float(scaledFrame) / 128.0f;

							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.emplace_back(mixedFrame);
						}

						if (framesRead < 1024)
							reading = false;
					}

					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart = 0;
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();
					// Usually, samples are not continuous waves. They are retriggered every time a note is played.
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].continueNote = false;
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loop = false;

					ma_decoder_uninit(&loadingDecoder);

					///////////////////////////////////////////////////////////// End.


					if (sampleDisplay.visible)
					{
						DrawSampleDisplay();
					}
					loadedSong.unsavedChanges = true;
				}
				else if (fileNavigator.fileNames[editor.selectedFile].at(0) == '3') // Load Instrument
				{
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
					std::string fileN = fileNavigator.fileNames[editor.selectedFile];
					fileN.erase(0, 1);
					LoadCurrentInstrument(fileN);
				}
			}
			else if (clickPos.y > 0 && clickPos.x > 0 && clickPos.x < 39)
			{
				if (clickPos.y - 2 + fileNavigator.fileListScroll < fileNavigator.fileNames.size())
				{
					if (clickPos.x == 1)
					{
						if (fileNavigator.fileNames[clickPos.y - 2 + fileNavigator.fileListScroll].at(0) == '0')
						{
							std::string fileN = fileNavigator.fileNames[clickPos.y - 2 + fileNavigator.fileListScroll];
							fileN.erase(0, 1);
							fileNavigator.EnterFile(fileN);
						}
					}
					else
					{
						editor.selectedFile = clickPos.y - 2 + fileNavigator.fileListScroll;
					}
				}
			}
		}
		else if (wind->name == "Save Song" || wind->name == "Save Instrument" || wind->name == "Save Sample")
		{
			if (clickPos.y == 1 && clickPos.x == 1)
			{
				fileNavigator.ExitFile();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 19 && clickPos.x > 28 && clickPos.x < 35) // Save.
			{
				if (wind->name == "Save Song")
					SaveSong();
				else if (wind->name == "Save Instrument")
					SaveCurrentInstrument();
				else
					SaveCurrentSample();

				// Refresh the preset menu and file menu.
				presetMenu.NavigateToInstrumentType(presetMenu.categories[presetMenu.instrumentType]);
				fileNavigator.NavigateToFile();
			}
			else if (clickPos.y > 0 && clickPos.x > 0 && clickPos.x < 39)
			{
				if (clickPos.y - 2 + fileNavigator.fileListScroll < fileNavigator.fileNames.size())
				{
					if (clickPos.x == 1)
					{
						if (fileNavigator.fileNames[clickPos.y - 2 + fileNavigator.fileListScroll].at(0) == '0')
						{
							std::string fileN = fileNavigator.fileNames[clickPos.y - 2 + fileNavigator.fileListScroll];
							fileN.erase(0, 1);
							fileNavigator.EnterFile(fileN);
						}
					}
					else
					{
						editor.selectedFile = clickPos.y - 2 + fileNavigator.fileListScroll;
					}
				}
			}
		}
		else if (wind->name == "Settings")
		{
			if (clickPos.y == 2)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				windowController.InitializeWindow("Themes", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 36 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			if (clickPos.y == 4)
			{
				windowController.InitializeWindow("Fishtank", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 13 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Selection")
		{
			if (clickPos.y == 2)
			{
				copyNotes();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 4)
			{
				pasteNotes();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 6)
			{
				deleteNotes();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 8)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				editor.transposeValue = 0;
				windowController.InitializeWindow("Transpose", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 16, 10 });
			}
			else if (clickPos.y == 10)
			{
				setNoteSamples();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Transpose")
		{
			if (clickPos.y == 3)
			{
				if (clickPos.x == 2)
					editor.transposeValue += loadedSong.edo;
				if (clickPos.x == 4)
					editor.transposeValue -= loadedSong.edo;
			}
			else if (clickPos.y == 5)
			{
				if (clickPos.x == 2)
					editor.transposeValue++;
				if (clickPos.x == 4)
					editor.transposeValue--;
			}
			else if (clickPos.y == 9)
			{
				if (clickPos.x > 7 && clickPos.x < 15)
				{
					transposeNotes();
					gui.drawUIThisFrame = true;
					gui.drawFrameThisFrame = true;
				}
			}
		}
		else if (wind->name == "Export as .WAV")
		{
			if (clickPos.y == 1 && clickPos.x == 1)
			{
				fileNavigator.ExitFile();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 19 && clickPos.x > 28 && clickPos.x < 37) // Save.
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				for (int name = 0; name < fileNavigator.fileNames.size(); name++)
				{
					std::string indexName = fileNavigator.fileNames[name];
					indexName.erase(0, 1);
					if (indexName == loadedSong.songName + ".wav")
					{
						windowController.InitializeWindow("Overwrite WAV", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 20, 16 });
						return;
					}
				}

				windowController.InitializeWindow("Exporting...", { 45, 20 }, { 10, 3 });

				editor.toRecordSong = true;
			}
			else if (clickPos.y > 0 && clickPos.x > 0 && clickPos.x < 39)
			{
				if (clickPos.y - 2 + fileNavigator.fileListScroll < fileNavigator.fileNames.size())
				{
					if (clickPos.x == 1)
					{
						if (fileNavigator.fileNames[clickPos.y - 2 + fileNavigator.fileListScroll].at(0) == '0')
						{
							std::string fileN = fileNavigator.fileNames[clickPos.y - 2 + fileNavigator.fileListScroll];
							fileN.erase(0, 1);
							fileNavigator.EnterFile(fileN);
						}
					}
					else
					{
						editor.selectedFile = clickPos.y - 2 + fileNavigator.fileListScroll;
					}
				}
			}
		}
		else if (wind->name == "Overwrite WAV")
		{
			if (clickPos.y == 7)
			{
				RecordSong();
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
			else if (clickPos.y == 9)
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
				return;
			}
		}
		else if (wind->name == "Instrument Editor")
		{
			if (!loadedInstruments[editor.selectedInstrument].enabled)
				return;


			
			// Synth UI.
			if (clickPos.x > 1 && clickPos.x < 13)
			{
				if (clickPos.y == 2) // Toggle waveform, sample or additive synth.
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType++;
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType > 1)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType = 0;
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
					{
						GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
						DrawSampleDisplay();
					}

					// Update additive operator frequencies.
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType == 2)
					{
						for (int wave = 0; wave < 4; wave++)
						{
							if (wave != sampleDisplay.selectedOperator && loadedInstruments[editor.selectedInstrument].waveforms[wave].operatorType == 2)
							{
								for (int freq = 0; freq < 11; freq++)
									loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[freq] = loadedInstruments[editor.selectedInstrument].waveforms[wave].frequencies[freq];
							}
						}
					}

					sampleDisplay.drawing = false; // Stop sample drawing.
					loadedSong.unsavedChanges = true;
					return;
				}
				if (clickPos.y == 3) // Change fuzz type.
				{
					if (clickPos.x > 0 && clickPos.x < 7)
					{
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzzType++;
						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzzType > 2)
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzzType = 0;
					}
				}
				if (clickPos.y == 4) // Change wave shape type
				{
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
					{
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType++;
						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType > 7)
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType = 0;
						GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
						DrawSampleDisplay();
						loadedSong.unsavedChanges = true;
					}
					return;
				}
				if (clickPos.y == 7 && (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType == 1 || loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType == 3)) // Toggle generate from sine waves.
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].generateFromSines = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].generateFromSines;
					GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
					return;
				}
				else if (clickPos.y > 20 && clickPos.y < 25) // Toggle modulator input type
				{
					if (clickPos.x == 2)
					{
						loadedInstruments[editor.selectedInstrument].modulationTypes[int(clickPos.y - 21)]--;
						if (loadedInstruments[editor.selectedInstrument].modulationTypes[int(clickPos.y - 21)] < 0)
							loadedInstruments[editor.selectedInstrument].modulationTypes[int(clickPos.y - 21)] = 11;
					}
					else if (clickPos.x == 12)
					{
						loadedInstruments[editor.selectedInstrument].modulationTypes[int(clickPos.y - 21)]++;
						if (loadedInstruments[editor.selectedInstrument].modulationTypes[int(clickPos.y - 21)] > 11)
							loadedInstruments[editor.selectedInstrument].modulationTypes[int(clickPos.y - 21)] = 0;
					}
					loadedSong.unsavedChanges = true;
					return;
				}
				else if (clickPos.y == 25) // Toggle sample display type
				{
					if (sampleDisplay.displayType == 0)
						sampleDisplay.displayType = 1;
					else
						sampleDisplay.displayType = 0;
					DrawSampleDisplay();
					return;
				}
			}


			if (int(clickPos.y) == 8) // Toggle using arps.
			{
				if (clickPos.x == 10)
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].useArp = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].useArp;

				DrawSampleDisplay();
			}
			else if (int(clickPos.y) == 9) // Toggle loop.
			{
				if (clickPos.x == 6)
				{
					for (int ch = 0; ch < 8; ch++)
					{
						for (int wave = 0;wave < 4; wave++)
						{
							channels[ch].waveforms[wave].sampleReadPos = 0.0f;
						}
					}

					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loop = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loop;

					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
			}
			else if (int(clickPos.y) == 10) // Toggle continue note.
			{
				if (clickPos.x == 15)
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].continueNote = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].continueNote;
					loadedSong.unsavedChanges = true;
				}

				DrawSampleDisplay();
			}
			else if (clickPos.y == 11) // Toggle reverse sample
			{
				if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
				{
					if (clickPos.x == 9)
					{
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].reverseFrames = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].reverseFrames;
						GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
						DrawSampleDisplay();
					}

					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;

					return;
				}
			}
			else if (clickPos.y == 15) // Toggle invert stereo
			{
				if (clickPos.x == 15)
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].invertStereo = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].invertStereo;

				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 12) // Toggle pitch to note
			{
				if (clickPos.x == 15)
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pitchToNote = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pitchToNote;

				DrawSampleDisplay();
				loadedSong.unsavedChanges = true;
			}
			else if (clickPos.y == 38) // Grid snap settings
			{
				if (clickPos.x == 13)
					sampleDisplay.enableSnap = !sampleDisplay.enableSnap;

				DrawSampleDisplay();
			}
			else if (clickPos.y == 15) // Toggle stereo type
			{
				if (clickPos.x == 15)
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].invertStereo = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].invertStereo;
			}
			else if (clickPos.y == 16) // Toggle sustain forever
			{
				if (clickPos.x == 6)
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].noSustain = !loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].noSustain;
				}
			}
			

			if (clickPos.x > 21 && clickPos.x < 34)
			{
				if (clickPos.y == 23) // Open replace with instrument menu.
				{
					windowController.InitializeWindow("Copy Instrument", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 24, 12 });
					return;
				}
				else if (clickPos.y == 24) // Open algorithm menu.
				{
					sampleDisplay.operatorMenuSelectedOperator = loadedInstruments[editor.selectedInstrument].algorithmType;
					windowController.InitializeWindow("Algorithms", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 34, 33 });
					return;
				}
				else if (clickPos.y == 25) // Open preset menu.
				{
					presetMenu.NavigateToInstrumentType(presetMenu.categories[0]);
					windowController.InitializeWindow("Presets", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 48, 18 });
					return;
				}
			}


			if (clickPos.y > 17 && clickPos.y < 22)
			{
				if (clickPos.x == 32) // Select operator.
				{
					sampleDisplay.drawing = false; // Stop sample drawing.
					sampleDisplay.selectedOperator = clickPos.y - 18;
					DrawSampleDisplay();
				}
			}

			if (clickPos.x > 17 && clickPos.x < 33) // Toggle displaying frequencies and arpeggios.
			{
				if (clickPos.y == 17)
				{
					sampleDisplay.displayArp = !sampleDisplay.displayArp;
				}
			}

			if (clickPos.y > 17 && clickPos.y < 23 && clickPos.x > 21 && clickPos.x < 31)
			{
				selectAlgorithmOperator({ clickPos.x - 24, clickPos.y - 18 });
				return;
			}

			if (clickPos.y > 25 && clickPos.y < 38 && editor.selectedInstrument > -1) // Sample display
			{
				if (sampleDisplay.displayType == 0) // Sample display
				{
					if (clickPos.y > 35) // Move loop points.
					{
						float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x) * 16.0f - 8.0f;
						float lStartPos = float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart) * (528.0f / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()));
						if (abs(lStartPos - posX) < 16)
						{
							sampleDisplay.dragLoopStart = true;
							return;
						}
						else
						{
							float lEndPos = float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd) * (528.0f / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()));
							if (abs(lEndPos - posX) < 16)
							{
								sampleDisplay.dragLoopEnd = true;
								return;
							}
						}
						loadedSong.unsavedChanges = true;
					}
					else
					{
						float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 0.5f) / 33.0f;

						if (posX > 1.0f)
							posX = 1.0f;
						else if (posX < 0)
							posX = 0;


						if (sampleDisplay.enableSnap)
						{
							posX *= sampleDisplay.snapSubdivisions;
							posX = int(posX);
							posX /= sampleDisplay.snapSubdivisions;
						}

						int frameIndex = (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * posX;

						sampleDisplay.sampleStartPos = frameIndex;
					}
					DrawSampleDisplay();
				}
				else if (sampleDisplay.displayType == 1) // Envelope display
				{
					if (clickPos.y > 24) // Create sample points.
					{
						float amp = 1.0f - (clickPos.y - 27.5f) / (33.0f - 25.0f);
						if (amp < 0.0f) amp = 0.0f;
						else if (amp > 1.0f) amp = 1.0f;
						
						int pos = int((gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 1.0f) * (float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength) + 0.5f) / 33.0f);
						if (pos > loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength)
							pos = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength;

						if (pos == 0)
						{
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeStartAmp = amp;
							DrawSampleDisplay();
							return;
						}
						
						for (int i = 0; i < loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.size(); i++)
						{
							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[i].position == pos)
							{
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[i].amp = amp;
								DrawSampleDisplay();
								return;
							}
							else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[i].position > pos)
							{
								EnvelopePoint newP;
								newP.position = pos;
								newP.amp = amp;

								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.emplace(
									loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.begin() + i, newP);
								DrawSampleDisplay();
								return;
							}
						}

						EnvelopePoint newP;
						newP.position = pos;
						newP.amp = amp;

						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.emplace_back(newP);
						DrawSampleDisplay();

						return;
					}
				}
			}
			else if (clickPos.y > 37)
			{
				if (clickPos.x == 1 || clickPos.x == 2) // Play sample
				{
					if (!editor.playingSong) // Play the note sound.
					{
						channels[0].resetChannelEffects(true);
						StartNote(0, editor.selectedInstrument, 48);
						
						sampleDisplay.playingInstrument = true;
						for (int wave = 0; wave < 4; wave++)
							channels[0].waveforms[wave].sampleReadPos = sampleDisplay.sampleStartPos;
					}
				}
				else if (clickPos.x == 3 || clickPos.x == 4) // Pause sample
				{
					channels[0].playing = false;
					sampleDisplay.playingInstrument = false;
					DrawSampleDisplay();
				}

				if (sampleDisplay.displayType == 0)
				{
					if (clickPos.x == 26 || clickPos.x == 27) // Toggle draw sample
					{
						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType == 1)
						{
							sampleDisplay.drawing = !sampleDisplay.drawing;
							DrawSampleDisplay();
							loadedSong.unsavedChanges = true;
						}
					}
					else if (clickPos.y == 39)
					{
						if (clickPos.x > 5 && clickPos.x < 25) // Change sample length display units
						{
							sampleDisplay.measurementSystem++;
							if (sampleDisplay.measurementSystem > 2)
								sampleDisplay.measurementSystem = 0;
						}

						DrawSampleDisplay();
					}
				}
				else if (sampleDisplay.displayType == 1)
				{
					if (clickPos.y == 39) // Set envelope scale
					{
						if (clickPos.x > 4 && clickPos.x < 16)
						{
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale /= 2.0f;
							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale < 0.25f)
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeScale = 4.0f;


							DrawSampleDisplay();
							loadedSong.unsavedChanges = true;
							return;
						}
					}
				}
			}
			
		}
		else if (wind->name == "Algorithms")
		{
			if (clickPos.x > 0 && clickPos.y > 0 && clickPos.x < 34 && clickPos.y < 32)
			{
				int algoX = int(clickPos.x - 2) / 8;
				int algoY = int(clickPos.y - 2) / 6;
				int algo = algoX + algoY * 4;
				sampleDisplay.operatorMenuSelectedOperator = algo;
				return;
			}
			else if (clickPos.y == 32 && clickPos.x > 7 && clickPos.x < 15)
			{
				loadedInstruments[editor.selectedInstrument].algorithmType = sampleDisplay.operatorMenuSelectedOperator;
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();
				loadedSong.unsavedChanges = true;
				return;
			}

		}
		else if (wind->name == "Presets")
		{
			if (clickPos.x > 0 && clickPos.y > 0 && clickPos.x < 13 && clickPos.y < 13)
			{
				if (int(clickPos.y) % 2 == 1)
				{
					presetMenu.instrumentType = (clickPos.y - 1) / 2;
					presetMenu.NavigateToInstrumentType(presetMenu.categories[presetMenu.instrumentType]);
				}

				return;
			}
			if (clickPos.x > 14 && clickPos.y > 0 && clickPos.y < 17)
			{
				if (clickPos.x < 30)
				{
					presetMenu.selectedSample = clickPos.y - 1;
					return;
				}
				else if (clickPos.x < 48)
				{
					presetMenu.selectedSample = clickPos.y - 1 + 16;
					return;
				}
			}
			else if (clickPos.y == 17 && clickPos.x > 7 && clickPos.x < 15)
			{
				if (presetMenu.selectedSample < presetMenu.fileNames.size())
				{
					Instrument newInstrument;

					std::ifstream instrumentFile("Presets/" + presetMenu.categories[presetMenu.instrumentType] + "/" + presetMenu.fileNames[presetMenu.selectedSample] + ".inst", std::ios::binary | std::ios::in);


					if (instrumentFile.is_open())
					{
						newInstrument = ReadInstrument(&instrumentFile);
					}

					loadedInstruments[editor.selectedInstrument] = newInstrument;

					loadedInstruments[editor.selectedInstrument].name = presetMenu.fileNames[presetMenu.selectedSample];

					DrawSampleDisplay();

					instrumentFile.close();

					loadedSong.unsavedChanges = true;
				}
				
				return;
			}
		}
		else if (wind->name == "File")
		{
			if (clickPos.y == 2) // New
			{
				// New song
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				std::string newSongName = "NewSong";
				int songNameNumber = 1;

				for (int i = 0; i < fileNavigator.fileNames.size(); i++)
				{
					std::string songName = fileNavigator.fileNames[i];
					if (songName.at(0) == '1')
					{
						songName.erase(0, 1);
						if (songName == "NewSong" + std::to_string(songNameNumber) + ".song") // Only show .wav files.
							songNameNumber++;
					}
				}

				newSongName.append(std::to_string(songNameNumber));
				newSongName.append(".song");

				LoadSong(newSongName);

				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 4) // Save song
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Save Song", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 6) // Save instrument
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Save Instrument", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 8) // Save sample
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Save Sample", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 10) // Load
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Load File", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
			else if (clickPos.y == 12) // Export
			{
				windowController.windows.erase(windowController.windows.begin() + windowIndex);
				windowController.windows.shrink_to_fit();

				fileNavigator.NavigateToFile();
				windowController.InitializeWindow("Export as .WAV", { int(gui.hoveredTile.x), int(gui.hoveredTile.y) }, { 40, 20 });
				gui.drawUIThisFrame = true;
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Copy Instrument")
		{
			if (clickPos.y >= 1.0f && clickPos.y < 10.0f)
			{
				if (clickPos.x >= 1.0f)
				{
					instrumentFloatingWindow.selectedInstrument = clickPos.y - 1 + instrumentFloatingWindow.instrumentListScroll;
					sampleDisplay.drawing = false; // Stop sample drawing.
					sampleDisplay.selectedOperator = 0; // Select the first sample operator.
				}
			}
		}
		if (clickPos.y == 11 && clickPos.x > 7 && clickPos.x < 14) // Copy instrument.
		{
			loadedInstruments[editor.selectedInstrument] = loadedInstruments[instrumentFloatingWindow.selectedInstrument];

			return;
		}
	}





	return;
}



void RightClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	clickPos.x = int(clickPos.x);
	clickPos.y = int(clickPos.y);

	//std::lock_guard<std::shared_mutex> lock(mtx);



	if (clickPos.y > 0)
	{

		if (wind->name == "Instrument Editor")
		{
			if (loadedInstruments[editor.selectedInstrument].enabled)
			{
				if (sampleDisplay.displayType == 1) // Envelope display
				{
					if (clickPos.y > 24)
					{
						int pos = int((gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 1.0f) * (float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength) + 0.5f) / 33.0f);
						if (pos > loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength)
							pos = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength;
						
						if (pos == 0)
						{
							return;
						}

						for (int i = 0; i < loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.size(); i++)
						{
							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[i].position == pos)
							{
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.erase(
									loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.begin() + i);
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.shrink_to_fit();
								DrawSampleDisplay();
								return;
							}
							else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[i].position > pos)
							{
								return;
							}
						}
					}


				}
			}
		}
	}


	if (windowIndex > 0) // Move the dragged window to front.
	{
		FloatingWindow moveWind = windowController.windows[windowIndex];
		windowController.windows.erase(windowController.windows.begin() + windowIndex);
		windowController.windows.emplace(windowController.windows.begin(), moveWind);
		windowController.windows.shrink_to_fit();
		windowIndex = 0;
	}

	return;
}




void HoldClickFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);

	


	if (clickPos.y > 0)
	{
		if (wind->name == "Save Song" || wind->name == "Save Instrument" || wind->name == "Load File" || wind->name == "Export as .WAV")
		{
			if (clickPos.x < 1.0f && clickPos.y > 1.0f && clickPos.y < 20.0f)
			{
				fileNavigator.fileScrollBar.position = (clickPos.y - 2.5f) / 17.0f;
				if (fileNavigator.fileScrollBar.position < 0.0f) fileNavigator.fileScrollBar.position = 0.0f;
				if (fileNavigator.fileScrollBar.position > 1.0f) fileNavigator.fileScrollBar.position = 1.0f;
				fileNavigator.fileListScroll = fileNavigator.fileScrollBar.position * fileNavigator.fileNames.size();
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Help")
		{
			if (clickPos.x < 1.0f && clickPos.y > 1.0f && clickPos.y < 39.0f)
			{
				gui.helpWindowScrollBar.position = (clickPos.y - 2.5f) / 37.0f;
				if (gui.helpWindowScrollBar.position < 0.0f) gui.helpWindowScrollBar.position = 0.0f;
				if (gui.helpWindowScrollBar.position > 1.0f) gui.helpWindowScrollBar.position = 1.0f;
				gui.helpWindowScroll = gui.helpWindowScrollBar.position * helpPageText.size();
				gui.drawFrameThisFrame = true;
			}
		}
		else if (wind->name == "Instrument Editor")
		{
			if (!loadedInstruments[editor.selectedInstrument].enabled)
				return;

			if (int(clickPos.y) == 2)
			{
				if (clickPos.x > 24 && clickPos.x < 34) // Volume/Arp speed.
				{
					if (sampleDisplay.displayArp)
					{
						loadedInstruments[editor.selectedInstrument].arpSpeed = (float(int((clickPos.x - 25) * 2.0f)) / 16.0f);

						if (loadedInstruments[editor.selectedInstrument].arpSpeed < 0.0f)
							loadedInstruments[editor.selectedInstrument].arpSpeed = 0.0f;
						else if (loadedInstruments[editor.selectedInstrument].arpSpeed > 0.9375f)
							loadedInstruments[editor.selectedInstrument].arpSpeed = 0.9375f;

						DrawSampleDisplay();
						loadedSong.unsavedChanges = true;
						return;
					}
				}
			}
			if (clickPos.x > 9 && clickPos.x < 18)
			{
				if (int(clickPos.y) == 3) // Set sample fuzz.
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzz = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);;

					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzz < 0)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzz = 0;
					else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzz > 0.9375)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].fuzz = 0.9375;

					loadedSong.unsavedChanges = true;
					return;
				}
				else if (int(clickPos.y) == 5) // Edit duty cycle.
				{
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
					{
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle < 0.0f)
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle = 0.0f;
						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle > 1.0f)
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].dutyCycle = 1.0f;

						GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
						DrawSampleDisplay();
						loadedSong.unsavedChanges = true;
					}
					return;
				}
				else if (int(clickPos.y) == 6) // Edit smoothness / # of sine waves to generate.
				{
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
					{
						if ((loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].generateFromSines // Waves
							&& (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType == 1
							|| loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType == 3))
							|| (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType > 3
								&& loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType != 7))
						{
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f) * 16;

							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves < 1)
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves = 1;
							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves > 15)
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].numOfSineWaves = 15;
						}
						else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType == 1 // Smoothness
							|| loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].waveType == 3)
						{
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness < 0.0f)
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness = 0.0f;
							if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness > 1.0f)
								loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].smoothness = 1.0f;
						}
						loadedSong.unsavedChanges = true;
						GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
						DrawSampleDisplay();
					}
					return;

				}
				else if (int(clickPos.y) == 13) // Set wave offset.
				{
					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1)
					{
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].offset = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].offset < 0)
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].offset = 0;
						else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].offset > 0.9375)
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].offset = 0.9375;

						loadedSong.unsavedChanges = true;
						GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
						DrawSampleDisplay();
					}
					return;
				}
				else if (int(clickPos.y) == 14) // Set sample octave.
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].octave = 15 - int(float(int((clickPos.x - 9) * 2.0f)));

					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].octave < 0)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].octave = 0;
					else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].octave > 15)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].octave = 15;

					loadedSong.unsavedChanges = true;
					DrawSampleDisplay();
					return;
				}
				else if (int(clickPos.y) == 16) // Set sample release.
				{

					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

					if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release < 0.0f)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release = 0.0f;
					else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release > 1.0f)
						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].release = 1.0f;

					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
					return;
				}
				else if (int(clickPos.y) == 18) // Set instrument volume.
				{

					loadedInstruments[editor.selectedInstrument].volume = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

					if (loadedInstruments[editor.selectedInstrument].volume < 0.0f)
						loadedInstruments[editor.selectedInstrument].volume = 0.0f;
					else if (loadedInstruments[editor.selectedInstrument].volume > 1.0f)
						loadedInstruments[editor.selectedInstrument].volume = 1.0f;
					
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
					return;
				}
				else if (int(clickPos.y) == 19) // Set instrument glide.
				{
					loadedInstruments[editor.selectedInstrument].glide = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

					if (loadedInstruments[editor.selectedInstrument].glide < 0)
						loadedInstruments[editor.selectedInstrument].glide = 0;
					else if (loadedInstruments[editor.selectedInstrument].glide > 0.99)
						loadedInstruments[editor.selectedInstrument].glide = 0.99;

					loadedSong.unsavedChanges = true;
					DrawSampleDisplay();
					return;
				}
				else if (int(clickPos.y) == 20) // Set instrument scatter.
				{
					loadedInstruments[editor.selectedInstrument].scatter = (float(int((clickPos.x - 9) * 2.0f)) / 16.0f);

					if (loadedInstruments[editor.selectedInstrument].glide < 0)
						loadedInstruments[editor.selectedInstrument].glide = 0;
					else if (loadedInstruments[editor.selectedInstrument].glide > 0.99)
						loadedInstruments[editor.selectedInstrument].glide = 0.99;

					loadedSong.unsavedChanges = true;
					DrawSampleDisplay();
					return;
					}
			}
			if (sampleDisplay.displayType == 0) // Sample display
			{
				if (clickPos.x > 14 && clickPos.x < 23)
				{
					if (int(clickPos.y) == 38) // Set sample display subdivisions.
					{
						sampleDisplay.snapSubdivisions = (float(int((clickPos.x - 14) * 4.0f)) / 32.0f) * 64.0f;

						if (sampleDisplay.snapSubdivisions < 0.0f)
							sampleDisplay.snapSubdivisions = 0.0f;
						if (sampleDisplay.snapSubdivisions > 64.0f)
							sampleDisplay.snapSubdivisions = 64.0f;

						DrawSampleDisplay();
						return;
					}
				}
			}
			else if (sampleDisplay.displayType == 1) // Envelope display
			{
				if (clickPos.x > 12 && clickPos.x < 32)
				{
					if (int(clickPos.y) == 38)
					{
						float len = float(clickPos.x - 12) * 4.0f + 1.0f;
						if (len < 1.0f) len = 1.0f;
						if (len > 80.0f) len = 80.0f;

						loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength = len;
						DrawSampleDisplay();
						return;
					}
				}
			}

			if (clickPos.x > 13 && clickPos.x < 22) // Scaling  parameter for mods.
			{
				if (clickPos.y > 21 && clickPos.y < 25)
				{
					loadedInstruments[editor.selectedInstrument].modScale[int(clickPos.y) - 21] = (float(int((clickPos.x - 13) * 2.0f)) / 16.0f);

					if (loadedInstruments[editor.selectedInstrument].modScale[int(clickPos.y) - 21] < 0)
						loadedInstruments[editor.selectedInstrument].modScale[int(clickPos.y) - 21] = 0;
					else if (loadedInstruments[editor.selectedInstrument].modScale[int(clickPos.y) - 21] > 0.9375)
						loadedInstruments[editor.selectedInstrument].modScale[int(clickPos.y) - 21] = 0.9375;

					loadedSong.unsavedChanges = true;
					DrawSampleDisplay();
					return;
				}
			}

			if (clickPos.x > 18 && clickPos.x < 33 && clickPos.y > 3 && clickPos.y < 18.0f)
			{
				if (sampleDisplay.displayArp) // Set arp pitches.
				{
					// 0 - 14
					if (clickPos.y > 3.25f && clickPos.y < 15.5f)
					{
						loadedInstruments[editor.selectedInstrument].arpPitches[int(clickPos.x) - 18] = 17.0f - float(int(clickPos.y * 4.0f)) * 0.25f;

						if (loadedInstruments[editor.selectedInstrument].arpPitches[int(clickPos.x) - 18] < 0.0f)
							loadedInstruments[editor.selectedInstrument].arpPitches[int(clickPos.x) - 18] = 0.0f;

						// Reset arpeggiation in channels using this instrument.
						for (int ch = 0; ch < 8; ch++)
						{
							if (channels[ch].instrument == editor.selectedInstrument)
							{
								channels[ch].arpIndex = -1;
								channels[ch].arpTimer = 0.0f;

								for (int i = 0; i < loadedInstruments[editor.selectedInstrument].arpLength + 1; i++)
								{
									channels[ch].arpP[i] = loadedInstruments[editor.selectedInstrument].arpPitches[i];
								}
							}
						}
						
						loadedSong.unsavedChanges = true;
					}
					else if (clickPos.y > 15.5f && clickPos.y < 17.0f) // Set arp length.
					{
						loadedInstruments[editor.selectedInstrument].arpLength = int(clickPos.x - 17.5f);
						if (loadedInstruments[editor.selectedInstrument].arpLength > 14)
							loadedInstruments[editor.selectedInstrument].arpLength = 14;
						else if (loadedInstruments[editor.selectedInstrument].arpLength < 0)
							loadedInstruments[editor.selectedInstrument].arpLength = 0;
						loadedSong.unsavedChanges = true;
					}
				}
				else if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].operatorType != 1) // Change frequencies.
				{
					if (int(clickPos.x) > 21)
					{
						float newFreqVal = 17.0f - float(int(clickPos.y * 4.0f)) * 0.25f;
						if (newFreqVal < 0.0f) newFreqVal = 0.0f;

						if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[int(clickPos.x) - 18 - 4] != newFreqVal)
						{
							loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].frequencies[int(clickPos.x) - 18 - 4] = newFreqVal;

							// Update additive operator frequencies.
							GenerateAdditiveWave(&loadedInstruments[editor.selectedInstrument], sampleDisplay.selectedOperator);
							DrawSampleDisplay();
							

							loadedSong.unsavedChanges = true;
						}
					}
				}
			}


			if (sampleDisplay.displayType == 0) // Sample display
			{
				// Drag loop points.
				float posX = ((gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x) * 16.0f - 8.0f) * (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() / 528.0f);

				if (sampleDisplay.enableSnap)
				{
					posX /= float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
					posX *= sampleDisplay.snapSubdivisions;
					posX += 0.5f;
					posX = int(posX);
					posX /= sampleDisplay.snapSubdivisions;
					posX *= (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
				}


				if (posX > loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
					posX = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();
				else if (posX < 0)
					posX = 0;

				if (sampleDisplay.dragLoopStart)
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart = posX;
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
				else if (sampleDisplay.dragLoopEnd)
				{
					loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd = posX;
					DrawSampleDisplay();
					loadedSong.unsavedChanges = true;
				}
				if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart > loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd)
				{
					std::swap(sampleDisplay.dragLoopStart, sampleDisplay.dragLoopEnd);
					std::swap(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart, loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd);
				}

				loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].clampLoopPoints();

				if (clickPos.y > 26 && clickPos.y < 38) // Move sample selection / draw.
				{
					/////////////////////////////////////////////////////////////////////////////////////////////////////////

					float posX = (gui.floatHoveredTile.x - windowController.windows[windowIndex].position.x - 0.5f) / 33.0f;

					if (posX > 1.0f)
						posX = 1.0f;
					else if (posX < 0)
						posX = 0;

					//////////////////////////////////////////////////////////////////////////////////////////////////////////


					if (sampleDisplay.drawing)
					{
						float posY = -(gui.floatHoveredTile.y - windowController.windows[windowIndex].position.y - 32.0f) / 6.0f;

						int frameIndex = (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * posX;



						sampleDisplay.drawWavePos = { float(frameIndex), posY };
						DrawSamplePoint(sampleDisplay.drawWavePos);
					}
					else
					{
						if (sampleDisplay.enableSnap)
						{
							posX *= sampleDisplay.snapSubdivisions;
							posX = int(posX);
							posX /= sampleDisplay.snapSubdivisions;
						}

						int frameIndex = (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) * posX;

						DrawSampleDisplay();
					}
					loadedSong.unsavedChanges = true;
				}
			}
		}
		else if (wind->name == "Copy Instrument")
		{
			if (clickPos.y > 1.0f && clickPos.y < 10.0f)
			{
				if (clickPos.x < 1.0f)
				{
					instrumentFloatingWindow.instrumentListScrollBar.position = (clickPos.y - 2.5f) / 7.0f;
					if (instrumentFloatingWindow.instrumentListScrollBar.position < 0.0f) instrumentFloatingWindow.instrumentListScrollBar.position = 0.0f;
					if (instrumentFloatingWindow.instrumentListScrollBar.position > 1.0f) instrumentFloatingWindow.instrumentListScrollBar.position = 1.0f;
					instrumentFloatingWindow.instrumentListScroll = instrumentFloatingWindow.instrumentListScrollBar.position * (256.0f - 10.0f);
					gui.drawUIThisFrame = true;
				}
			}
		}
	}



	return;
}



void RightHoldFloatingWindow(FloatingWindow* wind, int windowIndex, Vector2 clickPos, GLFWwindow* windMain)
{
	clickPos.x = int(clickPos.x);
	clickPos.y = int(clickPos.y);





	



	return;
}




void ChangeTheme(int theme)
{
	gui.uiColorTheme = theme;

	int sizeX, sizeY, comps;
	glBindTexture(GL_TEXTURE_2D, gui.uiTexture);
	unsigned char* data;

	std::string currentPath;

	

	if (gui.lightMode)
		currentPath = fileNavigator.getRelativePath() + "/GUI/TilesLight.png";
	else
		currentPath = fileNavigator.getRelativePath() + "/GUI/Tiles.png";

	data = stbi_load(&currentPath[0], &sizeX, &sizeY, &comps, 3);

	if (!data)
	{
		std::cout << "Tiles not found. ";
	}

	GUITheme currentTheme = gui.themes[theme];
	/*
	if (gui.uiBrightMode)
	{
		for (int i = 0; i < 6; i++)
			currentTheme.uiColors[i] = gui.themes[theme].uiColors[5 - i];
	}*/

	for (int i = 0; i < 512 * 512; i++)
	{
		unsigned char* colPos = data + i * 3;
		if (colPos[0] == 0 && colPos[1] == 0 && colPos[2] == 0)
		{
			data[i * 3] = currentTheme.uiColors[0].r;
			data[i * 3 + 1] = currentTheme.uiColors[0].g;
			data[i * 3 + 2] = currentTheme.uiColors[0].b;
		}
		else if (colPos[0] == 0 && colPos[1] == 0 && colPos[2] == 40)
		{
			data[i * 3] = gui.themes[theme].uiColors[1].r * 0.5f + gui.themes[theme].uiColors[0].r * 0.5f;
			data[i * 3 + 1] = gui.themes[theme].uiColors[1].g * 0.5f + gui.themes[theme].uiColors[0].g * 0.5f;
			data[i * 3 + 2] = gui.themes[theme].uiColors[1].b * 0.5f + gui.themes[theme].uiColors[0].b * 0.5f;
		}
		else if (colPos[0] == 30)
		{
			data[i * 3] = currentTheme.uiColors[1].r;
			data[i * 3 + 1] = currentTheme.uiColors[1].g;
			data[i * 3 + 2] = currentTheme.uiColors[1].b;
		}
		else if (colPos[0] == 50)
		{
			data[i * 3] = currentTheme.uiColors[2].r;
			data[i * 3 + 1] = currentTheme.uiColors[2].g;
			data[i * 3 + 2] = currentTheme.uiColors[2].b;
		}
		else if (colPos[0] == 90)
		{
			data[i * 3] = currentTheme.uiColors[3].r;
			data[i * 3 + 1] = currentTheme.uiColors[3].g;
			data[i * 3 + 2] = currentTheme.uiColors[3].b;
		}
		else if (colPos[0] == 150)
		{
			data[i * 3] = currentTheme.uiColors[4].r;
			data[i * 3 + 1] = currentTheme.uiColors[4].g;
			data[i * 3 + 2] = currentTheme.uiColors[4].b;
		}
		else if (colPos[0] == 255 && colPos[1] == 255 && colPos[2] == 255)
		{
			data[i * 3] = currentTheme.uiColors[5].r;
			data[i * 3 + 1] = currentTheme.uiColors[5].g;
			data[i * 3 + 2] = currentTheme.uiColors[5].b;
		}
		// Accent colors
		else if (colPos[0] == 63 && colPos[1] == 100 && colPos[2] == 100)
		{
			data[i * 3] = currentTheme.uiColors[6].r;
			data[i * 3 + 1] = currentTheme.uiColors[6].g;
			data[i * 3 + 2] = currentTheme.uiColors[6].b;
		}
		else if (colPos[0] == 127 && colPos[1] == 190 && colPos[2] == 0)
		{
			data[i * 3] = currentTheme.uiColors[7].r;
			data[i * 3 + 1] = currentTheme.uiColors[7].g;
			data[i * 3 + 2] = currentTheme.uiColors[7].b;
		}
		else if (colPos[0] == 255 && colPos[1] == 255 && colPos[2] == 0)
		{
			data[i * 3] = currentTheme.uiColors[8].r;
			data[i * 3 + 1] = currentTheme.uiColors[8].g;
			data[i * 3 + 2] = currentTheme.uiColors[8].b;
		}
		else if (colPos[0] == 210 && colPos[1] == 210 && colPos[2] == 230)
		{
			data[i * 3] = gui.themes[theme].uiColors[5].r * 0.5f + gui.themes[theme].uiColors[4].r * 0.5f;
			data[i * 3 + 1] = gui.themes[theme].uiColors[5].g * 0.5f + gui.themes[theme].uiColors[4].g * 0.5f;
			data[i * 3 + 2] = gui.themes[theme].uiColors[5].b * 0.5f + gui.themes[theme].uiColors[4].b * 0.5f;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	// Set the ui colors.
	gui.uiColors[0] = gui.themes[theme].uiColors[0].r / 255.0f;
	gui.uiColors[1] = gui.themes[theme].uiColors[0].g / 255.0f;
	gui.uiColors[2] = gui.themes[theme].uiColors[0].b / 255.0f;

	gui.uiColors[3] = gui.themes[theme].uiColors[1].r / 255.0f;
	gui.uiColors[4] = gui.themes[theme].uiColors[1].g / 255.0f;
	gui.uiColors[5] = gui.themes[theme].uiColors[1].b / 255.0f;

	gui.uiColors[6] = gui.themes[theme].uiColors[2].r / 255.0f;
	gui.uiColors[7] = gui.themes[theme].uiColors[2].g / 255.0f;
	gui.uiColors[8] = gui.themes[theme].uiColors[2].b / 255.0f;

	gui.uiColors[9] = gui.themes[theme].uiColors[3].r / 255.0f;
	gui.uiColors[10] = gui.themes[theme].uiColors[3].g / 255.0f;
	gui.uiColors[11] = gui.themes[theme].uiColors[3].b / 255.0f;

	gui.uiColors[12] = gui.themes[theme].uiColors[4].r / 255.0f;
	gui.uiColors[13] = gui.themes[theme].uiColors[4].g / 255.0f;
	gui.uiColors[14] = gui.themes[theme].uiColors[4].b / 255.0f;

	gui.uiColors[15] = gui.themes[theme].uiColors[5].r / 255.0f;
	gui.uiColors[16] = gui.themes[theme].uiColors[5].g / 255.0f;
	gui.uiColors[17] = gui.themes[theme].uiColors[5].b / 255.0f;


	gui.uiColors[45] = gui.themes[theme].uiColors[7].r / 255.0f;
	gui.uiColors[46] = gui.themes[theme].uiColors[7].g / 255.0f;
	gui.uiColors[47] = gui.themes[theme].uiColors[7].b / 255.0f;

	gui.uiColors[48] = gui.themes[theme].uiColors[7].r / 255.0f;
	gui.uiColors[49] = gui.themes[theme].uiColors[7].g / 255.0f;
	gui.uiColors[50] = gui.themes[theme].uiColors[7].b / 255.0f;

	gui.uiColors[51] = gui.themes[theme].uiColors[8].r / 255.0f;
	gui.uiColors[52] = gui.themes[theme].uiColors[8].g / 255.0f;
	gui.uiColors[53] = gui.themes[theme].uiColors[8].b / 255.0f;

	gui.uiColors[54] = gui.themes[theme].uiColors[9].r / 255.0f;
	gui.uiColors[55] = gui.themes[theme].uiColors[9].g / 255.0f;
	gui.uiColors[56] = gui.themes[theme].uiColors[9].b / 255.0f;

	

	if (sampleDisplay.visible)
	{
		DrawSampleDisplay();
		
		gui.drawFrameThisFrame = true;
		DrawFrameBorder();
		//DrawEverything();
		
	}
}


void selectAlgorithmOperator(Vector2 pos)
{
	int selectedOp = -1;

	switch (loadedInstruments[editor.selectedInstrument].algorithmType)
	{
	case 0:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		break;
	}

	case 1:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		break;
	}

	case 2:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 1;
		break;
	}

	case 3:
	{
		if (pos.x == 1 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 5 && pos.y == 3)
			selectedOp = 2;
		break;
	}

	case 4:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 2;
		break;
	}

	case 5:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 2;
		break;
	}

	case 6:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 0)
			selectedOp = 2;
		break;
	}

	case 7:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 2;
		break;
	}

	case 8:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 2)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 5 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 9:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 0)
			selectedOp = 2;
		else if (pos.x == 4 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 10:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 2)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 2)
			selectedOp = 2;
		else if (pos.x == 2 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 11:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 12:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 3 && pos.y == 2)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 13:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 0)
			selectedOp = 3;
		break;
	}

	case 14:
	{
		if (pos.x == 3 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 1 && pos.y == 1)
			selectedOp = 1;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 5 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 15:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 2 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 4 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 16:
	{
		if (pos.x == 1 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 5 && pos.y == 3)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 17:
	{
		if (pos.x == 1 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 3 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 5 && pos.y == 3)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 18:
	{
		if (pos.x == 2 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 1 && pos.y == 1)
			selectedOp = 2;
		else if (pos.x == 3 && pos.y == 1)
			selectedOp = 3;
		break;
	}

	case 19:
	{
		if (pos.x == 0 && pos.y == 3)
			selectedOp = 0;
		else if (pos.x == 2 && pos.y == 3)
			selectedOp = 1;
		else if (pos.x == 4 && pos.y == 3)
			selectedOp = 2;
		else if (pos.x == 6 && pos.y == 3)
			selectedOp = 3;
		break;
	}

	default:
		break;
	}

	

	if (selectedOp > -1)
	{
		

		loadedInstruments[editor.selectedInstrument].operatorMapping[selectedOp]++;
		if (loadedInstruments[editor.selectedInstrument].operatorMapping[selectedOp] > 3)
			loadedInstruments[editor.selectedInstrument].operatorMapping[selectedOp] = 0;


		
	}


	return;
}