#include "display_handler.h"
#include "system_variables.h"


#define LIGHT_BLUE 0x073F
#define LIGHT_GREEN 0x2F0D
#define DARK_GREEN 0x0ce5
#define WHITE TFT_WHITE
#define BLACK TFT_BLACK
#define BLUE TFT_BLUE
#define GREEN TFT_GREEN
#define RED TFT_RED
#define X_OFFSET 10
#define Y_OFFSET 10
#define SELECTION_OFFSET 60

TFT_eSPI tft = TFT_eSPI();
int previousSelection = -1;
String previousTitle = "";
int width;
int height;
int pageSelectionLabelWidth;
int previousStartPage = 0;
int previousEndPage = 0;

void Pad(int pixels);


void initDisplay()
{
    tft.init();
    tft.setRotation(1);
    width = tft.width();
    height = tft.height();
    tft.fillScreen(WHITE);
}


void showSplashScreen(const char *message)
{   
    tft.fillScreen(WHITE);
    tft.setFreeFont(&FreeSansBold24pt7b);
    tft.setTextColor(BLUE);
    const char *name = "PrintEase Solutions";
    int nameWidth = tft.textWidth(name);
    tft.setCursor((width - nameWidth)/2, 100);
    tft.print(name);

    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextColor(BLACK);
    const char *slogan = "Printing Made Simple.";
    int sloganWidth = tft.textWidth(slogan);
    tft.setCursor((width - sloganWidth)/2, 150);
    tft.print(slogan);

    if (message != nullptr)
    {
        tft.setFreeFont(&FreeSansBold12pt7b);
        tft.setTextColor(BLACK);
        int messageWidth = tft.textWidth(message);
        tft.setCursor((width - messageWidth)/2, 250);
        tft.print(message);
    }
}


void showNumber(char key)
{   
    tft.fillScreen(WHITE);
    tft.setFreeFont(&FreeSansBold24pt7b);
    tft.setTextColor(BLUE);
    tft.setCursor(200, 200);
    tft.println(key);
}

void showLocation(String message)
{   
    tft.fillScreen(WHITE);
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.setTextColor(BLUE);
    const char *name = "PrintEase Solutions";
    int nameWidth = tft.textWidth(name);
    tft.setCursor((width - nameWidth)/2, 100);
    tft.print(name);

    tft.setFreeFont(&FreeSansBold12pt7b);
    const char *note;
    const char *location;
    const char *finalNote;
    int finalNotePosition = 250;

    if (message == "None")
    {
        tft.setTextColor(RED);
        note = "Sorry. We couldn't find a suitable printer.";
        finalNote = "Please try again";
    }
    else
    {
        tft.setTextColor(DARK_GREEN);
        note = "Please collect your document at";
        location = message.c_str();
        finalNotePosition = 270;
        finalNote = "Thank you for using PrintEase!";
    }

    int noteWidth = tft.textWidth(note);
    tft.setCursor((width - noteWidth)/2, 175);
    tft.print(note);

    if (message != "None")
    {
        tft.setFreeFont(&FreeSansBold18pt7b);
        int locationWidth = tft.textWidth(location);
        tft.setCursor((width - locationWidth)/2, 220);
        tft.print(location);
    }

    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setTextColor(BLACK);
    int finalNoteWidth = tft.textWidth(finalNote);
    tft.setCursor((width - finalNoteWidth)/2, finalNotePosition);
    tft.print(finalNote);
}


void highlight(TFTMessage message, int selection)
{
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSans18pt7b);
    int textHeight = tft.fontHeight();
    int highlightY = Y_OFFSET + textHeight * (selection + 1.3);
    String title = message.title;
    if (title != previousTitle)
    {
        previousSelection = -1;
    }
    if (previousSelection != -1 && previousSelection != selection)
    {
        int oldY = Y_OFFSET + textHeight * (previousSelection + 1.3);
        tft.fillRect(0, oldY, width, textHeight, WHITE);
        tft.setCursor(0, Y_OFFSET + textHeight * (previousSelection + 2));
        Pad(SELECTION_OFFSET);
        tft.print(String(previousSelection + 1) + ": ");
        tft.println(message.selections[previousSelection]);
    }

    tft.fillRect(0, highlightY, width, textHeight, LIGHT_BLUE);
    tft.setTextColor(BLACK);
    tft.setCursor(0, Y_OFFSET + textHeight * (selection + 2));
    Pad(SELECTION_OFFSET);
    tft.print(String(selection + 1) + ": ");
    tft.println(message.selections[selection]);

    previousSelection = selection;
    previousTitle = title;
}


void updateDisplay(TFTMessage message)
{
    tft.fillScreen(WHITE);
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.setCursor(X_OFFSET, Y_OFFSET + tft.fontHeight());
    tft.println(message.title);

    tft.setFreeFont(&FreeSans18pt7b);

    for (int i = 0; i < MAX_SELECTIONS; i++)
    {
        if (message.selections[i] == nullptr)
        {
            break;
        }
        Pad(SELECTION_OFFSET);
        tft.print(String(i + 1) + ": ");
        tft.println(message.selections[i]);
    }
  }


void Pad(int pixels)
{
    int y = tft.getCursorY();
    tft.setCursor(pixels, y);
}


void showQR(const char *url) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(4)];
    qrcode_initText(&qrcode, qrcodeData, 4, 0, url);
    
    int scale = 6;
    int xOffset = (width - qrcode.size * scale) / 2;
    int yOffset = (height - qrcode.size * scale) / 2;
  
    tft.fillScreen(WHITE);
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setCursor(X_OFFSET, Y_OFFSET + tft.fontHeight());
    tft.println("Scan the QR code to upload your file.");
    for (uint8_t y = 0; y < qrcode.size; y++) {
      for (uint8_t x = 0; x < qrcode.size; x++) {
        int color = qrcode_getModule(&qrcode, x, y) ? BLACK : WHITE;
        tft.fillRect(xOffset + x * scale, yOffset + y * scale, scale, scale, color);
      }
    }
  }


void showFileInfo(const char *fileName, int numberOfPages)
{
    tft.fillScreen(WHITE);
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setCursor(0, Y_OFFSET + tft.fontHeight());
    Pad(SELECTION_OFFSET);
    tft.println("File received successfully.");

    tft.setFreeFont(&FreeSans12pt7b);
    tft.println();
    tft.println();

    Pad(SELECTION_OFFSET);
    tft.print("File Name: ");
    int availableWidth = width - tft.getCursorX() - 10;
    String fname(fileName);
    while (tft.textWidth(fname.c_str()) > availableWidth && fname.length() > 3)
    {
        fname.remove(fname.length() - 1);
        fname.setCharAt(fname.length() - 1, '.');
        fname.setCharAt(fname.length() - 2, '.');
        fname.setCharAt(fname.length() - 3, '.');
    }
    tft.println(fname);
    tft.println();

    Pad(SELECTION_OFFSET);
    tft.print("Number of Pages: ");
    tft.println(numberOfPages);

    tft.println();
    tft.println();
    tft.setTextColor(BLUE);
    tft.setFreeFont(&FreeSansBold12pt7b);
    Pad(SELECTION_OFFSET);
    tft.println("Press Enter to continue...");
}


void showError(const char *message)
{
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setTextColor(RED);
    int messageWidth = tft.textWidth(message);
    tft.setCursor((width - messageWidth)/2, height - Y_OFFSET * 1.5);
    tft.print(message);

    delay(1000);

    int textHeight = tft.fontHeight();
    tft.fillRect(0, height - textHeight - Y_OFFSET * 1.5, width, textHeight + Y_OFFSET * 1.5, WHITE);
}


void showPageRangeOptions(int pageSelection, int startPage, int endPage)
{
    tft.fillScreen(WHITE);
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.setCursor(X_OFFSET, Y_OFFSET + tft.fontHeight());
    tft.println("Select the page range:");
    tft.println("");

    tft.setFreeFont(&FreeSans18pt7b);
    int textHeight = tft.fontHeight();

    int valueY0 = Y_OFFSET + textHeight * (2.3);
    int valueY1 = Y_OFFSET + textHeight * (2.3 + 2);
    
    if (pageSelection == 0)
    {
        tft.fillRect(0, valueY0, width, textHeight, GREEN);
        tft.fillRect(0, valueY1, width, textHeight, WHITE);
    } else {
        tft.fillRect(0, valueY0, width, textHeight, WHITE);
        tft.fillRect(0, valueY1, width, textHeight, GREEN);
    }

    pageSelectionLabelWidth = tft.textWidth("Start page");
    Pad(SELECTION_OFFSET);
    tft.print("Start page");
    tft.println(": ");
    tft.println("");
    Pad(SELECTION_OFFSET);
    tft.print("End page");
    Pad(pageSelectionLabelWidth + SELECTION_OFFSET);
    tft.print(": ");

    int valueX = SELECTION_OFFSET + pageSelectionLabelWidth + 20;
    tft.setCursor(valueX, valueY0 + textHeight * 0.7);
    tft.println(String(startPage));
    tft.setCursor(valueX, valueY1 + textHeight * 0.7);
    tft.println(String(endPage));
  }


void getPageRangeOptions(int pageSelection, int startPage, int endPage)
{
    if (pageSelection == 0)
    {
        if (startPage == previousStartPage)
        {
            return;
        }
    }
    else
    {
        if (endPage == previousEndPage)
        {
            return;
        }
    }

    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSans18pt7b);
    int textHeight = tft.fontHeight();

    int valueX = SELECTION_OFFSET + pageSelectionLabelWidth + 20;
    int valueY = Y_OFFSET + textHeight * (2.3 + pageSelection * 2);

    tft.fillRect(valueX, valueY, width - valueX, textHeight, GREEN);
    tft.setCursor(valueX, valueY + textHeight * 0.7);
    tft.println(pageSelection == 0 ? String(startPage) : String(endPage));

    previousStartPage = startPage;
    previousEndPage = endPage;
    return;
}


void showCopies(int copies)
{
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSans18pt7b);
    int textHeight = tft.fontHeight();
    int highlightY = Y_OFFSET + textHeight * 2.3;
    tft.fillRect(0, highlightY, width, textHeight, GREEN);
    tft.setCursor(0, Y_OFFSET + textHeight * 3);
    Pad(SELECTION_OFFSET);
    tft.print("Copies: ");
    tft.print(copies);
}


void showPrintSettings()
{
    tft.fillScreen(WHITE);
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.setCursor(X_OFFSET, Y_OFFSET + tft.fontHeight());
    tft.println("Confirm Print Settings:");

    tft.setFreeFont(&FreeSans12pt7b);
    int textHeight = tft.fontHeight();
    int yOffset = Y_OFFSET + textHeight * 2.3;
    int selectionWidth = tft.textWidth("Page range") + X_OFFSET;

    Pad(X_OFFSET);
    tft.print("Paper Size");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.println(printSettings.paperSize);

    Pad(X_OFFSET);
    tft.setTextColor(BLACK);
    tft.print("Orientation");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.println(availableOrientations[printSettings.orientation]);
    Serial.println(availableOrientations[0]);
    Serial.println(availableOrientations[1]);
    Serial.println(printSettings.orientation);

    Pad(X_OFFSET);
    tft.setTextColor(BLACK);
    tft.print("Mode");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.println(availableModes[printSettings.mode]);

    Pad(X_OFFSET);
    tft.setTextColor(BLACK);
    tft.print("Page range");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.println(printSettings.range);

    Pad(X_OFFSET);
    tft.setTextColor(BLACK);
    tft.print("Color");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.println(availableColors[printSettings.color]);

    Pad(X_OFFSET);
    tft.setTextColor(BLACK);
    tft.print("Copies");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.println(printSettings.copies);

    Pad(X_OFFSET);
    tft.setTextColor(BLACK);
    tft.print("Price");
    Pad(selectionWidth);
    tft.print(": ");
    tft.setTextColor(BLUE);
    tft.print("Rs. ");
    tft.println(String(printSettings.price, 2));

    tft.setTextColor(BLUE);
    tft.setFreeFont(&FreeSansBold12pt7b);
    Pad((width - tft.textWidth("Press Enter to Confirm."))/2);
    tft.println("Press Enter to Confirm.");
}