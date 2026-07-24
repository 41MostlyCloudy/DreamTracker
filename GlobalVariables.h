#pragma once

#include "Structures.cpp"


// miniaudio
//----------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
//----------------------------------------------------------------------------------


float calculateSongLength(); // Returns the song's length in seconds.



float delta = 0.0f; // Time between frames.



Channel channels[8];

Screen screen;
GUI gui;
Editor editor;
WindowController windowController;

Song loadedSong;
UnrolledPattern loadedPattern; // The frame currently in the editor.
std::vector <std::string> fileNameList; // The names of the samples in the "Samples" file.
//std::vector <Sample> loadedSamples; // The names of the samples used in the song.
Instrument loadedInstruments[256]; // The names of the samples used in the song.

UnrolledPattern patternSelection; // A frame containing the portion of the frame that is selected, with that number of rows. All values not used are set to -2.

FileNavigator fileNavigator;

SampleDisplay sampleDisplay;
PresetMenu presetMenu;
InstrumentFloatingWindow instrumentFloatingWindow;


int programVersionNumber = 0;





Vector2 findFrameTileByPosition(int pos) // Returns the channel and channel part as a Vector2
{
	int selectedPart = pos;
	int selectedChannel = 0;
	bool inChannel = false;
	while (!inChannel)
	{
		int channelSize = 11;

		if (selectedPart >= channelSize)
		{
			selectedPart -= channelSize;
			selectedChannel++;
			if (selectedChannel >= 8)
			{
				return { -1, -1 }; // Outside of the frame.
			}
		}
		else if (selectedPart == channelSize - 1)
		{
			return { float(selectedChannel), -1 }; // Outside of the frame.
		}
		else
		{
			inChannel = true;
		}
	}

	return { float(selectedChannel), float(selectedPart) };
}



float calculateSongLength() // Returns the song's length in seconds.
{
	float length = 0;
	float calcTempo = loadedSong.startingBPM;

	

	for (int fr = 0; fr < loadedSong.patternSequence.size(); fr++)
	{
		for (int i = 0; i < loadedSong.patterns[loadedSong.patternSequence[fr]].rows; i++)
		{
			// Add row to time.
			length += ((60000.0f / (calcTempo * 4.0f)) * 48.0f / 48000.0f);
		}
	}

	return length;
}



float calculateSongPosition() // Returns the song's current position in seconds.
{
	float length = 0;
	float calcTempo = loadedSong.startingBPM;



	for (int fr = 0; fr < loadedSong.currentPattern + 1; fr++)
	{
		for (int i = 0; i < loadedSong.patterns[loadedSong.patternSequence[fr]].rows; i++)
		{
			// Add row to time.
			length += ((60000.0f / (calcTempo * 4.0f)) * 48.0f / 48000.0f);

			if (fr == loadedSong.currentPattern && i >= loadedSong.currentNote)
				i = loadedSong.patterns[loadedSong.patternSequence[fr]].rows;
		}
	}

	return length;
}






std::vector <std::string> helpPageText =
{
	"WELCOME TO DUALTRACKER!",
	"",
	"",
	"",
	"#",
	"ABOUT",
	"",
	"",
	"",
	"DUALTRACKER is a music making program ",
	"with a tracker-styled interface.",
	"",
	"It allows for the creation of music",
	"from sequenced samples and simple.",
	"synthesized sounds.",
	"",
	"Tracker programs are actually quite",
	"intuitive, but there is a lot to look",
	"at, so watching a tutorial on the basic",
	"layout of trackers would be helpful.",
	"",
	"",
	"",
	"#",
	"HEXADECIMALS",
	"",
	"",
	"",
	"As is usual in tracker programs,",
	"hexidecimal numbers are used in",
	"DUALTRACKER as a compact way of",
	"displaying numbers.",
	"",
	"While regular numbers have 10 different",
	"numerals, hexadecimals have 16,",
	"represented by the numbers",
	"0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B,",
	"C, D, E, and F.",
	"",
	"For example, the number 10 would be",
	"represented as 0A in hexidecimal,",
	"and the number 255 is represented",
	"as FF.",
	"",
	"",
	"",
	"#",
	"FILES",
	"",
	"",
	"",
	"Song projects are stored as .song",
	"files.",
	"A .song file contains all of the",
	"patterns and instruments in the song.",
	"",
	"Individual instruments can also be",
	"stored as .inst files so they can be",
	"loaded into new songs.",
	"",
	".wav Samples loaded can be into the",
	"program and used in songs.",
	"",
	"",
	"",
	"#",
	"TOP GUI",
	"",
	"",
	"",
	"The top of the screen has a selection of",
	"tools and properties.",
	"",
	"At the far left is the pattern window.",
	"This contains all of the patterns used",
	"in the song.",
	"A pattern is a section of the song",
	"denoted by its number.",
	"Patterns can be repeated like loops.",
	"The buttons on the side can add new",
	"patterns or change the number of the",
	"pattern currently selected.",
	"",
	"",
	"To its right is the settings window.",
	"This has a few properties:",
	"",
	" -BPM:",
	"    This number specifies the speed of",
	"    the song in beats per minute.",
	"",
	" -EDO:",
	"    This number specifies the number",
	"    of notes between octaves. Usually",
	"    you use 12, but different numbers",
	"    can be set for different tuning",
	"    systems.",
	"    I couldn't find a good notation",
	"    systems for all EDO scales, so",
	"    I came up with one by tacking",
	"    some of them together.",
	"    That would explain why you can't",
	"    find anything about the symbols",
	"    online.",
	"",
	" -OCT:",
	"",
	"    This number specifies the octave",
	"    of the notes that are played on",
	"    the keyboard and added to the song.",
	"",
	" -ROW:",
	"",
	"    This number specifies the number",
	"    of rows in the frame that you are",
	"    currently editing.",
	"",
	" -BEAT:",
	"",
	"    This number specifies number of",
	"    beats per measure in the frame that",
	"    you are currently editing.",
	"",
	"",
	"In the center are buttons to access",
	"different windows:",
	"",
	" -FILE:",
	"",
	"    Saving and loading functions.",
	"",
	" -SETTINGS:",
	"",
	"    Various program properties.",
	"",
	" -HELP:",
	"",
	"    The help page.",
	"",
	" -INSTRUMENT:",
	"",
	"    The instrument editor, where you",
	"    make instruments for the song.",
	"",
	"",
	"",
	"To the right of the title, you can",
	"choose from a few different displays:",
	"",
	" -SONG:",
	"",
	"    This shows song properties.",
	"",
	" -KEYS:",
	"",
	"    This shows how the keyboard lines.",
	"    up with a piano keyboard.",
	"",
	" -EFFECT:",
	"",
	"    This shows a list of all channel",
	"    effects, explained in EFFECTS."
	"",
	" -MIX:",
	"",
	"    Here, the volume of each channel",
	"    can be set.",
	"",
	"",
	"To the right is the instrument menu.",
	"Here, you can create, select or delete",
	"instruments denoted by numbers.",
	"",
	"",
	"",
	"#",
	"WRITING PATTERNS",
	"",
	"",
	"",
	"Patterns are the building blocks of",
	"songs.",
	"Each pattern contains a number of",
	"channels, sources that play one sound",
	"at a time.",
	"",
	"CHANNELS",
	"",
	"In a channel, notes are read and",
	"played from top to bottom like a set",
	"of instructions.",
	"",
	"Channels appear as a vertical list of",
	"numbers with the following meanings",
	"left to right:",
	"",
	" -NOTES:",
	"",
	"    The leftmost value in a channel is",
	"    the note pitch.",
	"    When selected, a key is pressed",
	"    to enter notes at the",
	"    corresponding pitch.",
	"",
	"    Next is the note instrument.",
	"    This number specifies the",
	"    instrument to be played on the note",
	"    as an index in the list of loaded",
	"    instruments.",
	"    The instrument number is in",
	"    hexadecimal, and ranges from 00",
	"    to FF.",
	"",
	"    After that is the volume value.",
	"    This sets the volume for the note",
	"    and all future notes until changed.",
	"    Volume is in hexidecimal, and",
	"    ranges from 00 to FF.",
	"",
	" -EFFECTS:",
	"",
	"    Each channel has an effect column,",
	"    although up to 7 more can be added",
	"    for simultaneous effects.",
	"",
	"    The first part is the effect letter.",
	"    This sets the effect for the note.",
	"    Different effects will make ",
	"    changes to the note or song.",
	"    These changes are explained later in",
	"    the EFFECT TYPES section.",
	"    ",
	"    The colon to the right of the letter",
	"    can be set to 'i' to increase the",
	"    effect value over time according to"
	"    the parameter value, or 'd'to",
	"    decrease it.",
	"    Not setting this will instead set the",
	"    value of the effect.",
	"",
	"    To the furthest right is the effect",
	"    value.",
	"    This specifies a parameter for",
	"    the specified effect.",
	"    What is does depends on the effect.",
	"    The effect value is in hexidecimal,",
	"    and ranges from 00 to FF.",
	"",
	"",
	"",
	"EFFECT TYPES",
	"",
	"Since trackers play sequenced music,",
	"they can run into the problem of sounding",
	"flat.",
	"Effects are really the key to making",
	"songs dynamic and interesting,",
	"although they can be used in all sorts of",
	"different ways.",
	"",
	" -(V) Volume",
	"",
	"    This effect sets the volume of the",
	"    channel.",
	"",
	" -(P) Pitch",
	"",
	"    This effect sets the pitch of the",
	"    channel.",
	"    It can be used to bend notes or set",
	"    pitch mid-sample.",
	"",
	" -(L) Low-pass filter",
	"",
	"    This effect sets a low-pass filter",
	"    on the channel, removing high-",
	"    pitched frequencies.",
	"",
	" -(H) High-pass filter",
	"",
	"    This effect sets a high-pass",
	"    filter on the channel, removing",
	"    low-pitched frequencies.",
	"",
	" -(J) Jump to point",
	"",
	"    This effect sets the position of",
	"    the currently playing instrument",
	"    sample.",
	"    It uses the jump points in the",
	"    instrument samples, which can be",
	"    set in the instrument editor.",
	"    This can be used for chopping long",
	"    drum loops or whatever else you",
	"    want.",
	"",
	" -(R) Retrigger",
	"",
	"    This effect sets the current note",
	"    to restart on an interval.",
	"    It can be used for stuttering sounds",
	"    or in conjunction with J and E for",
	"    some granular effects.",
	"",
	" -(M) Mute",
	"",
	"    Setting the effect value above 0",
	"    mutes the channel.",
	"    Set back to 0 to unmute.",
	"",
	" -(E) Extend",
	"",
	"    This effect toggles on and off",
	"    like M.",
	"    When set, effects that alter notes",
	"    continue to future notes until",
	"    changed.",
	"    Try using it to set stereo or make",
	"    a long pitch or volume change.",
	"",
	" -(F) Fuzz",
	"",
	"    This effect increases the",
	"    amplitude of the channel, then",
	"    flattens it into a smaller range.",
	"    It adds distortion to the sound.",
	"",
	" -(S) Stereo",
	"",
	"    This effect sets the stereo of",
	"    the channel.",
	"    If you are wearing headphones,",
	"    this is how much of the sound is",
	"    playing out of each ear.",
	"",
	" -(D) Delay note",
	"",
	"    This effect delays the note before",
	"    starting it.",
	"    Use it for more natural note",
	"    playing, small arpeggios, or any",
	"    other case between channel rows.",
	"",
	" -(G) Go to pattern",
	"",
	"    This effect sets the position of",
	"    the song to the start of the",
	"    pattern of the index in the song",
	"    given by the effect value.",
	"",
	" -(T) Tempo",
	"",
	"    This effect sets the song's tempo,",
	"    the speed at which it plays.",
	"",
	" -(O) Operator volume",
	"",
	"    This effect sets the volume of a",
	"    single sample operator.",
	"    The first number chooses the",
	"    operator, (0-3), and the second",
	"    sets the volume, (0-F)."
	"",
	" -(A) Change arpeggio",
	"",
	"    This effect works with the",
	"    arpeggio property built into",
	"    instruments.",
	"    The first number chooses the",
	"    index in the arpeggio, and the",
	"    second sets its pitch.",
	"    This is a more limited effect,",
	"    but it can be used to change the",
	"    arpeggio to different chords.",
	"",
	" -(B) Sample rate",
	"",
	"    This effect sets the channel",
	"    sample rate.",
	"    A lower sample rate sounds more",
	"    compressed.",
	"",
	" -(C) Clear all",
	"",
	"    Since effects might start to stack",
	"    up with E, this effect clears all",
	"    past effects.",
	"",
	"",
	"",
	"#",
	"INSTRUMENTS",
	"",
	"",
	"",
	"Before you can start making notes, you",
	"will need to create some instruments",
	"in the instrument editor.",
	"",
	"Instruments in DUALTRACKER use 4",
	"operators, which can be arranged in a",
	"number of algorithms for different",
	"sounds.",
	"",
	"Instrument can be either a waveform",
	"with more editable parameters, or a",
	"sample, which can be drawn, edited,",
	"or loaded in from a .wav file.",
	"",
	"Sample loading is done through the",
	"FILE button in the top GUI, and",
	"the sound is loaded to the currently",
	"selected operator of the selected",
	"instrument.",
	"",
	"You can toggle between waveforms and",
	"samples at the top of the editor.",
	"",
	"Messing around with the instrument",
	"presets might give a better idea of",
	"how it functionss.",
	"You should also look into how synth",
	"modulation works.",
	"",
	"",
	"The instrument editor has a few",
	"important parts:",
	"",
	" -PARAMETERS:",
	"",
	"    On the left of the editor are a",
	"    number of parameters for the",
	"    operator or instrument.",
	"",
	" -SAMPLE DISPLAY:",
	"",
	"    At the bottom is the sample",
	"    display, which shows the current",
	"    operator.",
	"    ",
	"    In the yellow area at the top,",
	"    jump points can be added or,",
	"	 removed. These allow the sample",
	"    to move to different points using",
	"    the J effect.",
	"    In the blue area at the bottom,",
	"    loop points points can be set.",
	"",
	" -MODULATION ALGORITHM:",
	"",
	"    At the right above the sample",
	"    display is the mod algorithm.",
	"    ",
	"    Operators are different samples",
	"    or waveforms that can either play",
	"    a sound or modulate others sounds.",
	"    ",
	"    Arrows between operators represent",
	"    modulation paths.",
	"    Each path has its own path number.",
	"",
	" -MODULATION TYPES:",
	"",
	"    To the left of the algorithm are",
	"    the modulator types.",
	"    ",
	"    Each modulation path can be assigned",
	"    a modulation type, which determines",
	"	 how it affects the sound it modulates.",
	"    modulates.",
	"    ",
	"    The paths also include a parameter",
	"    for the strength of the modulation.",
	"",
	" -FREQUENCY/ARP EDITOR:",
	"",
	"    At the top right of the editor is the",
	"    frequency/arp editor.",
	"    The two functions can be toggled",
	"    between with the button underneath.",
	"    ",
	"    The frequency editor is used to draw",
	"    the frequencies in the harmonic",
	"    series, similar to organ drawbars.",
	"   ",
	"    The arp editor sets the pith of each",
	"    note in the instrument's arpeggio.",
	"",
	"",
	"",
	"#",
	"KEY FUNCTIONS",
	"",
	"",
	"",
	"Certain keys are pressed to preform the",
	"actions below:"
	"",
	" -SPACE:",
	"",
	"    Start or stop the song.",
	"",
	" -ENTER:",
	"",
	"    Return the cursor to the beginning",
	"    of the song.",
	"",
	" -ESC:",
	"",
	"    Close the program.",
	"",
	" -TAB:",
	"",
	"        Make a stop note.",
	"        Stops notes stop the",
	"        current note.",
	"",
	"    DELETE",
	"",
	"        Delete the currently",
	"        selected notes.",
	"",
	"    LEFT SHIFT",
	"",
	"        Copy the currently",
	"        selected notes.",
	"",
	"    RIGHT SHIFT",
	"",
	"        Paste notes at the",
	"        selected area.",
	"",
	"",
	"",
	"#",
};