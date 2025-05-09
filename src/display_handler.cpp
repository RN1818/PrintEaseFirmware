#include "display_handler.h"


#define LIGHT_BLUE 0x073F
#define LIGHT_GREEN 0x2F0D
#define WHITE TFT_WHITE
#define BLACK TFT_BLACK
#define BLUE TFT_BLUE
#define RED TFT_RED
#define X_OFFSET 10
#define Y_OFFSET 10
#define SELECTION_OFFSET 60

TFT_eSPI tft = TFT_eSPI();
int previousSelection = -1;
int width;
int height;

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
        tft.setCursor((width - messageWidth)/2, 150);
        tft.print(message);
    }
}


void highlight(TFTMessage message, int selection)
{
    tft.setTextColor(BLACK);
    tft.setFreeFont(&FreeSans18pt7b);
    int textHeight = tft.fontHeight();
    int highlightY = Y_OFFSET + textHeight * (selection + 1.3);

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
}


void updateDisplay(TFTMessage message) {
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
    int spaceWidth = tft.textWidth("e");
    int selectionPadding = pixels / spaceWidth;
    String padding = " ";
    for (int i = 0; i < selectionPadding; i++)
    {
        padding += " ";
    }
    tft.print(padding);
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