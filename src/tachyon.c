#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

void leer_red()
{
    FILE* f = fopen("/proc/net/dev", "r");
    if (f == NULL)
    {
        printf("error, no hay archivo\n");
        return;
    }
    else
    {
        char linea[256];
        while (fgets(linea, sizeof(linea), f) != NULL)
        {

            char interfaz[32];
            unsigned long long rx_bytes, tx_bytes;

            int campos = sscanf(linea, " %31[^:]: %llu %*u %*u %*u %*u %*u %*u %*u %llu",
                interfaz, &rx_bytes, &tx_bytes);
            if (campos == 3 && strcmp(interfaz, "lo") != 0)
            {
                printf("Interfaz: %s\n", interfaz);
                printf("  Recibidos: %llu bytes\n", rx_bytes);
                printf("  Enviados:  %llu bytes\n", tx_bytes);
            }
        }
    }
}

int main(void)
{
    leer_red();
    return 0;
}