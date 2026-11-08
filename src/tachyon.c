#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void leer_gateway()
{
    FILE* f = fopen("/proc/net/route", "r");
    if (f == NULL)
    {
        printf("no se pudo encontrar el archivo\n");
        return;
    }
    else
    {
        char linea[256];
        char buffer[4096] = "";
        while (fgets(linea, sizeof(linea), f) != NULL)
        {
            char interfaz[32], destino[16], gateway_hex[16];

            if (sscanf(linea, "%31s %15s %15s", interfaz, destino, gateway_hex) ==
                3)
            {
                if (strcmp(destino, "00000000") == 0)
                {

                    unsigned int gateway_num;
                    sscanf(gateway_hex, "%x", &gateway_num);
                    struct in_addr addr;
                    addr.s_addr = gateway_num;
                    printf("Gateway: %s\n", inet_ntoa(addr));
                }
            }
        }
    }
}

void leer_ip()
{
    FILE* f = fopen("/proc/net/fib_trie", "r");
    if (f == NULL)
    {
        printf("no se pudo encontrar el archivo\n");
        return;
    }
    else
    {
        char linea[256];
        char buffer[4096] = "";
        int ya_impresa = 0;
        while (fgets(linea, sizeof(linea), f) != NULL)
        {
            int a, b, c, d;
            if (sscanf(linea, "%*[^0-9]%d.%d.%d.%d", &a, &b, &c, &d) == 4)
            {
                if (d == 255)
                {
                    continue;
                }

                if (d == 0)
                {
                    continue;
                }

                if (a == 0)
                {
                    continue;
                }

                if (a != 127)
                {
                    if (!ya_impresa)
                    {
                        printf("IP: %d.%d.%d.%d\n", a, b, c, d);
                        ya_impresa = 1;
                    }
                }
            }
        }
    }
}

void leer_red(unsigned long long* rx, unsigned long long* tx)
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

            int campos =
                sscanf(linea, " %31[^:]: %llu %*u %*u %*u %*u %*u %*u %*u %llu",
                       interfaz, &rx_bytes, &tx_bytes);
            if (campos == 3 && strcmp(interfaz, "lo") != 0)
            {
                *rx = rx_bytes;
                *tx = tx_bytes;
            }
        }
    }
}

int main(void)
{
    unsigned long long rx_viejo, tx_viejo, rx_nuevo, tx_nuevo;

    leer_gateway();
    leer_ip();

    leer_red(&rx_viejo, &tx_viejo);

    while (1)
    {
        sleep(1);

        leer_red(&rx_nuevo, &tx_nuevo);

        double rx_vel = (rx_nuevo - rx_viejo) / 1024.0;
        double tx_vel = (tx_nuevo - tx_viejo) / 1024.0;

        printf("\033[H\033[J");
        printf("Gateway: 192.168.1.254\n");
        printf("IP: 192.168.1.85\n");
        printf("RX: %.2f KB/s\n", rx_vel);
        printf("TX: %.2f KB/s\n", tx_vel);

        rx_viejo = rx_nuevo;
        tx_viejo = tx_nuevo;
    }

    return 0;
}