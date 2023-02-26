#include "lwl.h"

int main()
{
    lwlInitWindow(200, 200, "Window");
    int running;
    while (lwlWindowIsOpen()) {
        lwlClearWindow();
    }
    lwlTerminateWindow();
    return 0;
}
