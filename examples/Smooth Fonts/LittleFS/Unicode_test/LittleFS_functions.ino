/*====================================================================================
  This sketch supports the ESP6266 with LittleFS Flash filing system

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/

//====================================================================================
//                 Print a Flash FS directory list (root directory)
//====================================================================================

void listFiles(void) {
  Serial.println();
  Serial.println("Flash FS files found:");
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);
#ifdef ESP32 // LittleFS is implemented in a different way on ESP32
  File dir = LittleFS.open("/"); // Root directory
  File f = dir.openNextFile(); 

  while (f) {
    String fileName = f.name(); 
    Serial.print(fileName);
    int spaces = 25 - fileName.length(); // Tabulate nicely
    if (spaces < 0) spaces = 1;
    while (spaces--) Serial.print(" ");
    Serial.print(f.size()); Serial.println(" bytes"); 
    f = dir.openNextFile(); 
    yield();
  }
#else
  fs::Dir dir = LittleFS.openDir("/"); // Root directory

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 25 - fileName.length(); // Tabulate nicely
    if (spaces < 0) spaces = 1;
    while (spaces--) Serial.print(" ");
    fs::File f = dir.openFile("r");
    Serial.print(f.size()); Serial.println(" bytes");
    yield();
  }
 #endif

  Serial.println(line);

  Serial.println();
  delay(1000);
}
//====================================================================================

