// contains funciton declarations for editing files

void beginSD(const int CS);
void newFileHeader(const char *name, const char *header);
void writeFile(const char *name, String line);
void beginRTC();
String getTimestamp();