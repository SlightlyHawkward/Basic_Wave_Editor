#pragma once

//
// Simple Library for Wave File Manipulation.
//
// Copyright Gustavo Rodriguez-Rivera grr@cs.purdue.edu
//
// Use this code at your own risk.
// You are free to copy this code in any application 
// free or commercial as long as this notice remains in the sources.
//
// See https://ccrma.stanford.edu/courses/422/projects/WaveFormat/ for more
// information about the wave file format.
// 
// Version 1/24/2012 7:15am

typedef struct WaveHeader {
    unsigned char chunkID[4];      // big endian 
    unsigned char chunkSize[4];    // little endian
    unsigned char format[4];       // big endian
    unsigned char subchunk1ID[4];  // big endian
    unsigned char subchunk1Size[4];// little endian
    unsigned char audioFormat[2]; // little endian
    unsigned char numChannels[2];  // little endian
    unsigned char sampleRate[4];   // little endian 
    unsigned char byteRate[4];     // little endian
    unsigned char blockAlign[2];   // little endian
    unsigned char bitsPerSample[2];// little endian
    unsigned char subchunk2ID[4];  // big endian
    unsigned char subchunk2Size[4];// little endian
    unsigned char data[1];         // little endian
} WaveHeader;

class WaveFile
{
	friend class CWaveEditView;

    unsigned long numChannels; // Number of Channels
    unsigned long sampleRate; // Sample Rate
    unsigned long bitsPerSample; //Bits/Sample
    unsigned long bytesPerSample; //Bytes/Sample
    unsigned int lastSample; //Last sample
    unsigned int maxSamples; //Max Samples
    unsigned int fileSize; //File Size
    unsigned int dataSize; //Data Size

    WaveHeader * hdr; // Points to the wave file in memory

public:
    WaveFile(void);// Create an empty wave file.
    WaveFile( int numChannels, int sampleRate, int bitsPerSample); // Create an empty wave file with these parameters.
    ~WaveFile(void); // Destructor
	bool read(CFile * f); // Read a wave file from opened file f
	void play();  // Play wave file.
	void add_sample(int sample);  // Add a new sample at the end of the wave file.
	int get_sample(int i); // Get ith sample
    void save(CFile * f);  // Save wave file in opened file f
    void tone(int frequency, int msecs); // Create a tone with this frequency and this number of msecs.
    void append_wave(WaveFile * src);// Append a wave file src to the end of this wave file. 



    // Update the wave header in memory with the latest number of samples etc.
    // We need to call updateHeader before saving or playing the file after
    // the wave file has been updated.
    void updateHeader();

    // Create a new wave file that is k times the frequency rate of the original up to durationms.
    // If durationms==0 then use the whole wave file.
    WaveFile * multiply_freq(double k, int durationms);

    // Create a new wavefile with echo from the original one.
    // echoAmount is a constant 0 to 1 with the amount of echo
    // delayms is the delay of the echo added to the original.
    WaveFile * echo(float echoAmount, float delayms);

	WaveFile * changeSpeed(float speed);

	WaveFile * get_fragment(int startMs, int endMs); //Gets a portion of a wave file

	WaveFile * remove_fragment(int startMs, int endMs); // Moves a fragment of the wave to another location

	WaveFile * paste_fragment(int startMs, WaveFile * clipboard);

	WaveFile * replace_fragment(int startMs, int endMs, WaveFile * clipping);

	WaveFile * reverse();

	WaveFile * shift(int modulation);
};
