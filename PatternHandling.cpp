#pragma once


#include "GlobalVariables.h"


RolledChannel rollPatternChannel(UnrolledPattern pattern, int ch); // Rolls and returns the frame given.

void unrollPatternChannel(UnrolledPattern* pattern, RolledChannel channel, int ch); // Unrolls and returns the unrolled frame given. (for viewing and editing).

void saveCurrentPattern();

void loadCurrentPattern();

void resizeUnrolledChannelRows(UnrolledChannel* ch, int newSize);

void copyNotes();

void pasteNotes();

void deleteNotes();

void transposeNotes();

void setNoteSamples();




RolledChannel rollPatternChannel(UnrolledPattern pattern, int ch)
{
	// Frame Format:

	// For each channel
	//	1 byte: Note
	//	1 byte: Instrument
	//	1 byte: To next note

	//	1 byte:	Volume
	//	1 byte: To next volume

	//	5 bytes: Voice sample
	//	1 byte: To next voice sample

	//	1 byte: Effect
	//	1 byte: Effect value
	//	1 byte: To next effect (This is set to 0 for multiple effects per beat.)

	RolledChannel newChannel;

	newChannel.rows = pattern.channels[ch].rows;

	newChannel.beatsPerMeasure = pattern.channels[ch].beatsPerMeasure;

	newChannel.stereo = channels[ch].stereo;

	newChannel.loopAtEnd = channels[ch].loopAtEnd;
	


	newChannel.notes.clear();
	newChannel.volumes.clear();
	newChannel.effects.clear();
	int toNextNote = 0;
	int toNextVolume = 0;
	int toNextVoiceSample = 0;
	int toNextEffect = 0;
	for (int i = 0; i < pattern.channels[ch].data.size(); i++)
	{
		if (pattern.channels[ch].data[i].note > -1)
		{
			newChannel.notes.emplace_back(toNextNote);
			newChannel.notes.emplace_back(pattern.channels[ch].data[i].note);
			if (pattern.channels[ch].data[i].note != 255) // No instruments for stop notes.
				newChannel.notes.emplace_back(pattern.channels[ch].data[i].instrument);
			toNextNote = 0;
		}
		else
			toNextNote++;
		if (pattern.channels[ch].data[i].volume > -1)
		{
			newChannel.volumes.emplace_back(toNextVolume);
			newChannel.volumes.emplace_back(pattern.channels[ch].data[i].volume);
			toNextVolume = 0;
		}
		else
			toNextVolume++;

		if (pattern.channels[ch].data[i].effect > -1)
		{
			newChannel.effects.emplace_back(toNextEffect);
			newChannel.effects.emplace_back(pattern.channels[ch].data[i].effect);
			toNextEffect = 0;
		}
		else
			toNextEffect++;


		if (i == pattern.channels[ch].data.size() - 1)
		{
			if (newChannel.notes.size() == 0)
				newChannel.notes.emplace_back(toNextNote);
			if (newChannel.volumes.size() == 0)
				newChannel.volumes.emplace_back(toNextVolume);
			if (newChannel.effects.size() == 0)
				newChannel.effects.emplace_back(toNextEffect);
		}
	}




	return newChannel;
}



void unrollPatternChannel(UnrolledPattern* pattern, RolledChannel channel, int ch)
{
	// Frame Format:

	// Each channel
	//	1 byte: To next note
	//	1 byte: Note
	//	1 byte: Instrument

	//	1 byte: To next volume
	//	1 byte:	Volume
	
	//	1 byte: To next effect
	//	1 byte: Effect
	//	1 byte: Effect value



	channels[ch].stereo = channel.stereo;

	channels[ch].loopAtEnd = channel.loopAtEnd;


	pattern->channels[ch].rows = channel.rows;
	pattern->channels[ch].data.clear();
	pattern->channels[ch].data.resize(channel.rows);

	pattern->channels[ch].beatsPerMeasure = channel.beatsPerMeasure;


	int unrolledNoteIndex = 0;

	for (int i = 0; i < channel.notes.size(); i += 3)
	{
		int toNextNote = channel.notes[i];

		for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
		{
			pattern->channels[ch].data[j].note = -1;
			pattern->channels[ch].data[j].instrument = -1;
		}

		unrolledNoteIndex += toNextNote;


		if (i < channel.notes.size() - 1)
		{
			int nextNote = channel.notes[i + 1];
			pattern->channels[ch].data[unrolledNoteIndex].note = nextNote;
			if (nextNote == 255) // No instruments for stop notes.
			{
				i--;
			}
			else
			{
				nextNote = channel.notes[i + 2];
				pattern->channels[ch].data[unrolledNoteIndex].instrument = nextNote;
			}
		}

		unrolledNoteIndex++;
	}


	unrolledNoteIndex = 0;

	for (int i = 0; i < channel.volumes.size(); i += 2)
	{
		int toNextNote = channel.volumes[i];

		for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
		{
			pattern->channels[ch].data[j].volume = -1;
		}

		unrolledNoteIndex += toNextNote;


		if (i < channel.volumes.size() - 1)
		{
			int nextNote = channel.volumes[i + 1];
			pattern->channels[ch].data[unrolledNoteIndex].volume = nextNote;
		}

		unrolledNoteIndex++;
	}


	unrolledNoteIndex = 0;

	for (int i = 0; i < channel.effects.size(); i += 2)
	{
		int toNextNote = channel.effects[i];

		for (int j = unrolledNoteIndex; j < unrolledNoteIndex + toNextNote; j++)
		{
			pattern->channels[ch].data[j].effect = -1;
		}

		unrolledNoteIndex += toNextNote;


		if (i < channel.effects.size() - 1)
		{
			int nextNote = channel.effects[i + 1];
			pattern->channels[ch].data[unrolledNoteIndex].effect = nextNote;
		}

		unrolledNoteIndex++;
	}

	

	return;
}



void saveCurrentPattern()
{
	loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].rows = loadedPattern.rows;

	for (int ch = 0; ch < 8; ch++)
	{
		int patternIndex = loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch];

		while (patternIndex >= loadedSong.channelPatterns[ch].patterns.size())
		{
			RolledChannel newPat;
			loadedSong.channelPatterns[ch].patterns.emplace_back(newPat);
		}

		loadedSong.channelPatterns[ch].patterns[patternIndex] = rollPatternChannel(loadedPattern, ch);
	}

	
	gui.drawUIThisFrame = true;
	gui.drawFrameThisFrame = true;
}



void loadCurrentPattern()
{
	loadedPattern.rows = loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].rows;

	for (int ch = 0; ch < 8; ch++)
	{
		int patternIndex = loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch];

		unrollPatternChannel(&loadedPattern, loadedSong.channelPatterns[ch].patterns[patternIndex], ch);
	}


	gui.patternScroll = 0;

	// The interface has changed, and must be redrawn.
	gui.drawUIThisFrame = true;
	gui.drawFrameThisFrame = true;
}



void resizeUnrolledChannelRows(UnrolledChannel* ch, int newSize)
{
	if (newSize > ch->data.size())
	{
		for (int fr = 0; fr < newSize; fr++)
		{
			UnrolledRow newR;
			ch->data.emplace_back(newR);
		}
	}
	else
	{
		ch->data.resize(newSize);
		ch->data.shrink_to_fit();
	}

	ch->rows = newSize;

	return;
}








void copyNotes()
{
	
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}



	// Create mouse selection frame.
	int rows = int(editor.noteSelectionEnd.y + 1 - editor.noteSelectionStart.y);
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;
	int rightMostChannel = findFrameTileByPosition(editor.noteSelectionEnd.x).x;


	for (int ch = leftMostChannel; ch <= rightMostChannel; ch++)
	{
		int copyCh = ch - leftMostChannel;

		patternSelection.channels[copyCh].data.clear();
		resizeUnrolledChannelRows(&patternSelection.channels[copyCh], rows);

		for (int y = 0; y < rows; y++)
		{
			patternSelection.channels[copyCh].data[y] = { -2, -2, -2, -2 };
		}



		for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
		{
			int copyY = y - editor.noteSelectionStart.y;

			// Add any parts of the channel inside the selected area.
			int notePos = ch * 11;
			if (notePos + 4 >= editor.noteSelectionStart.x && notePos < editor.noteSelectionEnd.x)
			{
				patternSelection.channels[copyCh].data[copyY].note = loadedPattern.channels[ch].data[y].note;
				patternSelection.channels[copyCh].data[copyY].instrument = loadedPattern.channels[ch].data[y].instrument;
			}

			if (notePos + 6 >= editor.noteSelectionStart.x && notePos + 5 < editor.noteSelectionEnd.x)
				patternSelection.channels[copyCh].data[copyY].volume = loadedPattern.channels[ch].data[y].volume;

			if (notePos + 8 >= editor.noteSelectionStart.x && notePos + 7 < editor.noteSelectionEnd.x)
				patternSelection.channels[copyCh].data[copyY].effect = loadedPattern.channels[ch].data[y].effect;
		}
	}


	for (int ch = rightMostChannel - leftMostChannel + 1; ch < 8; ch++)
	{
		resizeUnrolledChannelRows(&patternSelection.channels[ch], 0);
	}



	return;
}




void pasteNotes()
{
	if (patternSelection.channels[0].data.size() == 0) // Don't paste if nothing is selected.
		return;


	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	

	// Create mouse selection frame.
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;


	for (int ch = 0; ch < 8; ch++)
	{
		int pasteCh = ch + leftMostChannel;

		if (pasteCh >= 8)
			break;

		
		for (int y = 0; y < patternSelection.channels[ch].rows; y++)
		{
			int pasteY = y + editor.noteSelectionStart.y;

			if (pasteY >= int(loadedPattern.channels[pasteCh].rows))
				break;


			if (patternSelection.channels[ch].data[y].note != -2)
			{
				loadedPattern.channels[pasteCh].data[pasteY].note = patternSelection.channels[ch].data[y].note;
				loadedPattern.channels[pasteCh].data[pasteY].instrument = patternSelection.channels[ch].data[y].instrument;
			}
			if (patternSelection.channels[ch].data[y].volume != -2)
				loadedPattern.channels[pasteCh].data[pasteY].volume = patternSelection.channels[ch].data[y].volume;
			if (patternSelection.channels[ch].data[y].effect != -2)
				loadedPattern.channels[pasteCh].data[pasteY].effect = patternSelection.channels[ch].data[y].effect;
				
		}
	}

	loadedSong.unsavedChanges = true;

	return;
}


void deleteNotes()
{
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	// Create mouse selection frame.
	int rows = int(editor.noteSelectionEnd.y + 1 - editor.noteSelectionStart.y);
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;
	int rightMostChannel = findFrameTileByPosition(editor.noteSelectionEnd.x).x;


	for (int ch = leftMostChannel; ch <= rightMostChannel; ch++)
	{
		for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
		{
			
			// Add any parts of the channel inside the selected area.
			int notePos = ch * 11;
			if (notePos + 4 >= editor.noteSelectionStart.x && notePos < editor.noteSelectionEnd.x)
			{
				loadedPattern.channels[ch].data[y].note = -1;
				loadedPattern.channels[ch].data[y].instrument = -1;
			}

			if (notePos + 6 >= editor.noteSelectionStart.x && notePos + 5 < editor.noteSelectionEnd.x)
				loadedPattern.channels[ch].data[y].volume = -1;

			if (notePos + 8 >= editor.noteSelectionStart.x && notePos + 7 < editor.noteSelectionEnd.x)
				loadedPattern.channels[ch].data[y].effect = -1;
		}
	}

	return;
}



void transposeNotes()
{
	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	// Create mouse selection frame.
	int rows = int(editor.noteSelectionEnd.y + 1 - editor.noteSelectionStart.y);
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;
	int rightMostChannel = findFrameTileByPosition(editor.noteSelectionEnd.x).x;


	for (int ch = leftMostChannel; ch <= rightMostChannel; ch++)
	{
		for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
		{

			// Add any parts of the channel inside the selected area.
			int notePos = ch * 11;
			if (notePos + 4 >= editor.noteSelectionStart.x && notePos < editor.noteSelectionEnd.x)
			{
				if (loadedPattern.channels[ch].data[y].note != -1)
					loadedPattern.channels[ch].data[y].note += editor.transposeValue;
			}
		}
	}

	
	return;
}



void setNoteSamples()
{

	if (editor.playingSong) // Set the selected area to the playing position.
	{
		editor.noteSelectionStart.y = loadedSong.currentNote;
		editor.noteSelectionEnd.y = loadedSong.currentNote;
	}

	// Create mouse selection frame.
	int rows = int(editor.noteSelectionEnd.y + 1 - editor.noteSelectionStart.y);
	int leftMostChannel = findFrameTileByPosition(editor.noteSelectionStart.x).x;
	int rightMostChannel = findFrameTileByPosition(editor.noteSelectionEnd.x).x;


	for (int ch = leftMostChannel; ch <= rightMostChannel; ch++)
	{
		for (int y = editor.noteSelectionStart.y; y <= editor.noteSelectionEnd.y; y++)
		{

			// Add any parts of the channel inside the selected area.
			int notePos = ch * 11;
			if (notePos + 4 >= editor.noteSelectionStart.x && notePos < editor.noteSelectionEnd.x)
			{
				if (loadedPattern.channels[ch].data[y].note != -1)
					loadedPattern.channels[ch].data[y].instrument = editor.selectedInstrument;
			}
		}
	}


	return;
}