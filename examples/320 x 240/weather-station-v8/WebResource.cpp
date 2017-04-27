/**The MIT License (MIT)
Copyright (c) 2015 by Daniel Eichhorn
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
See more at http://blog.squix.ch
*/

#include "WebResource.h"

WebResource::WebResource(){
  
}

void WebResource::downloadFile(String url, String filename) {
  downloadFile(url, filename, nullptr);
}

void WebResource::downloadFile(String url, String filename, ProgressCallback progressCallback) {

    if (SPIFFS.exists(filename) == true) {
      Serial.println("Found " + filename);
      return;
    }
    else Serial.println("Downloading "  + filename + " from " + url);

    // wait for WiFi connection
    if((_wifiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");

        // configure server and url
        http.begin(url);

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        if(httpCode > 0) {
            //SPIFFS.remove(filename);
            fs::File f = SPIFFS.open(filename, "w+");
            if (!f) {
                Serial.println("file open failed");
                return;
            }
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {

                // get length of document (is -1 when Server sends no Content-Length header)
                int total = http.getSize();
                int len = total;
                progressCallback(filename, 0,total);
                // create buffer for read
                uint8_t buff[128] = { 0 };

                // get tcp stream
                WiFiClient * stream = http.getStreamPtr();

                // read all data from server
                while(http.connected() && (len > 0 || len == -1)) {
                    // get available data size
                    size_t size = stream->available();

                    if(size) {
                        // read up to 128 byte
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                        // write it to Serial
                        f.write(buff, c);

                        if(len > 0) {
                            len -= c;
                        }
                        progressCallback(filename, total - len,total);
                    }
                    delay(1);
                }

                Serial.println();
                Serial.print("[HTTP] connection closed or file end.\n");

            }
            f.close();
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end();
    }
}
