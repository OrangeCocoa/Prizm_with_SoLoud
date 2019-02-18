// public domain
bool jo_write_wav(const char *filename, short numChannels, short bitsPerSample, int sampleRateHz, const void *data, 
int size) {
	FILE *fp = fopen(filename, "wb");
	if(!fp) {
		return false;
	}
	fwrite("RIFF", 1, 4, fp);
	int length = size + 44 - 8;
	fwrite(&length, 1, 4, fp);
	fwrite("WAVEfmt \x10\x00\x00\x00\x01\x00", 1, 14, fp);
	fwrite(&numChannels, 1, 2, fp);
	fwrite(&sampleRateHz, 1, 4, fp);
	int bpsec = numChannels * sampleRateHz * bitsPerSample/8;
	fwrite(&bpsec, 1, 4, fp);
	short bpsamp = numChannels * bitsPerSample/8;
	fwrite(&bpsamp, 1, 2, fp);
	fwrite(&bitsPerSample, 1, 2, fp);
	fwrite("data", 1, 4, fp);
	fwrite(&size, 1, 4, fp);
	fwrite(data, 1, size, fp);
	fclose(fp);
	return true;
}