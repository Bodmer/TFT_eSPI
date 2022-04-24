---
name: Issue template
about: Guide to content
title: ''
labels: ''
assignees: ''

---

To minimise effort to resolve issues the following should be provided as a minimum:
1. Procesor, e.g RP2040, ESP32 S3 etc
2. TFT_eSPI library version (try the latest, the problem may have been resolved!)
3. IDE (e.g. Arduino or PlatformIO)
4. Board package version (e.g. 2.0.3)
5. TFT driver (e.g. ILI9341), a link to the vendors product web page is useful too.

Plus further information as appropriate to the problem:
6. TFT to processor connections used
7. A zip file containing your setup file (just drag and drop in message window - do not paste in long files!)
8. A zip file  containing a simple and complete example sketch that demonstrates the problem but needs no special hardware sensors or libraries.
9. A description of the problem and the conditions that cause it to occur
10. Screen shot pictures showing the problem  (just drag and drop in message window)

The idea is to provide sufficient information so I can setup the exact same scenario to investigate and resolve the issue.

Put three ticks ( ` chracter on key next to "1") at the start and end of short pasted code segments to avoid format/markup anolmolies and to use a monspaced font. Example:

```
  Serial.begin(115200);
  tft.init();
```
