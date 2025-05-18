#include "file_handler.h"
#include "system_variables.h"
#include "display_handler.h"


const char* paperSizes[] = {"A4", "A3", "A5", "Letter", "Legal"};
const char* modeTypes[] = {"Duplex", "Simplex"};
const char* colorTypes[] = {"Black & White", "Color"};

float prices[5][2][2];


int getIndex(const String& value, const char* arr[], int size) {
  for (int i = 0; i < size; i++) {
    if (value == arr[i]) return i;
  }
  return -1;
}


void storeRates(JsonDocument doc)
{
    for (JsonPair paper : doc.as<JsonObject>()) {
    int pi = getIndex(paper.key().c_str(), paperSizes, 5);
    if (pi == -1) continue;

    JsonObject modeTypesObj = paper.value().as<JsonObject>();
    for (JsonPair printType : modeTypesObj) {
      int ti = getIndex(printType.key().c_str(), modeTypes, 2);
      if (ti == -1) continue;

      JsonObject colorObj = printType.value().as<JsonObject>();
      for (JsonPair color : colorObj) {
        int ci = getIndex(color.key().c_str(), colorTypes, 2);
        if (ci == -1) continue;

        prices[pi][ti][ci] = color.value().as<float>();
      }
    }
  }
}


void calculatePrice()
{
    int paperSize = getIndex(printSettings.paperSize, paperSizes, 5);
    int orientation = printSettings.orientation;
    int mode = printSettings.mode;
    int color = printSettings.color;
    int copies = printSettings.copies;
    int numberOfPages = numberOfPagesGlobal;

    Serial.println(printSettings.range);
    if (printSettings.range  != "All")
    {
        Serial.println("Inside range");
        int dashIndex = printSettings.range.indexOf('-');
        int startPage = printSettings.range.substring(0, dashIndex).toInt();
        int endPage = printSettings.range.substring(dashIndex + 1).toInt();
        numberOfPages = endPage - startPage + 1;
    }

    int duplexSheets = numberOfPages / 2;
    int simplexSheets = numberOfPages % 2;

    if (mode == 1)
    {
        duplexSheets = 0;
        simplexSheets = numberOfPages;
    }

    printSettings.price = (prices[paperSize][0][color] * duplexSheets + prices[paperSize][1][color] * simplexSheets) * copies;

    Serial.println("Paper Size: " + String(paperSize));
    Serial.println("Orientation: " + String(orientation));
    Serial.println("Mode: " + String(mode));
    Serial.println("Color: " + String(color));
    Serial.println("Copies: " + String(copies));
    Serial.println("Duplex Sheets: " + String(duplexSheets));
    Serial.println("Simplex Sheets: " + String(simplexSheets));
    Serial.println("Price: " + String(printSettings.price, 2));
}


void processFile(JsonDocument doc)
{
    const char *fileName = doc["file_name"];
    const int remainingFiles = (int)doc["remaining_files"];
    const int numberOfPages = (int)doc["number_of_pages"];
    numberOfPagesGlobal = numberOfPages;
    reaminingFilesGlobal = remainingFiles;
    showFileInfo(fileName, numberOfPagesGlobal);
}