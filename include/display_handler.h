#pragma once

#include <SPI.h>
#include <TFT_eSPI.h>
#include <QRCode_Library.h>
#include <qrcode.h>

#define MAX_SELECTIONS 5

struct TFTMessage
{
    const char *title;
    const char *selections[MAX_SELECTIONS] = {nullptr};
};

void initDisplay();
void showSplashScreen(const char *message = nullptr);
void highlight(TFTMessage message, int selection);
void updateDisplay(TFTMessage message);
void showQR(const char *url);
void showFileInfo(const char *fileName, int numberOfPages);
void showError(const char *message);
void showPageRangeOptions(int pageSelection = 0, int startPage = 1, int endPage = 1);
void getPageRangeOptions(int pageSelection = 0, int startPage = 1, int endPage = 1);
void showCopies(int copies = 1);
void showPrintSettings();
void showLocation(String message);
void showNumber(char key);