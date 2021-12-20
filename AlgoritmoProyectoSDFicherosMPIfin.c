#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define FIL 1000
#define COL 24
#define TAG 0

float calculaMAPE();

int main(int argc, char **argv)
{
    char nombreFicheroProcesado[] = "Datos_1x.txt"; //Escribir aquí nombre fichero que se quiere leer

    // ** Declaraci�n de variables
    char res[1001][192], filas[10];
    int col = 24;
    float datos[641664];
    MPI_Status stat;
    MPI_File mfile;

    // ** Inicio del entorno MPI
    MPI_Init(&argc, &argv);
    MPI_File_open(MPI_COMM_WORLD, "Datos_1x.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &mfile);
    int cont = 0;
    int cuenta = 192;
    int i = 0;
    MPI_File_read_at(mfile, 0, filas, 9, MPI_CHAR, MPI_STATUS_IGNORE);
    int fil = atoi(filas);
    int p = 1;
    for (int i = 0; i < 1001; i++)
    { //65.237.962(1000) //451.629(1)+cont//4.965.307(10)+cont//61.063.885(100)+cont
        MPI_File_read_at_all(mfile, 451629 + cont + (cuenta * cont), res[i], cuenta - 1, MPI_CHAR, MPI_STATUS_IGNORE);
        cont++;
        p++;
    }
    float dias[1001][24];
    for (int s = 0; s < 1001; s++)
    {
        char *token = NULL;
        token = strtok(res[s], ",");

        while (token)
        {
            //printf("token:%s\n",token);
            dias[s][i] = atoi(token);
            token = strtok(NULL, ",");
            i++;
        }
        i = 0;
    }
    
    printf("\n filas:%d  Usadas: %d-%d\n", fil, fil + 1 - 1000, fil + 1);

    
    int j, indiceDiaMAPEMasBajo = 0, k = 2, spid; //Última línea
    float ultDia[FIL], diaActual[FIL], vectorMAPESRealizados[FIL], mediaAritmeticaVecinos[FIL];

    int pid = 0, prn, splitSize, restSize, rows = 0, offset = 0;

    //Calculo porcentaje MAPE
    float MAPEActual = 0, MAPEGeneral = 500.00; //MAPEGeneral se puede ajustar, dependiendo del valor que se le ponga, saldrán unos resultados u otros


    // ** Obtención del PID
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // ** Obtención del número de PRN
    MPI_Comm_size(MPI_COMM_WORLD, &prn);

    clock_t begin = clock(); //Empiezo a calcular el tiempo de ejecucción

    if (pid == 0)
    { //hilo maestro

        printf("\n[%d]: Volcando en un vector, los datos del último día", pid); //Paso el último día a un vector propio
        for (j = 0; j < COL; j++)
        {
            ultDia[j] = dias[999][j];
            printf("%f", ultDia[j]);
        }
        //** Envío a los esclavos:
        for (spid = 1; spid <= prn - 1; spid++)
        {
            MPI_Send(&ultDia, COL, MPI_FLOAT, spid, 0, MPI_COMM_WORLD);
            MPI_Send(&diaActual, COL, MPI_FLOAT, spid, 0, MPI_COMM_WORLD);
            MPI_Send(&dias, FIL * COL, MPI_FLOAT, spid, 0, MPI_COMM_WORLD);
        }

        //** Recepción de resultados
        for (spid = 1; spid <= prn - 1; spid++)
        {

            MPI_Recv(&vectorMAPESRealizados,COL , MPI_FLOAT, spid, 0, MPI_COMM_WORLD, NULL);
            MPI_Recv(&mediaAritmeticaVecinos, COL, MPI_FLOAT, spid, 0, MPI_COMM_WORLD, NULL);
        }

        //** Impresión
        printf("\nPredicciones para el día actual");
        for (j = 0; j < COL; j++)
        {
            printf("\n Hora: %d --> %f", j + 1, mediaAritmeticaVecinos[j]);
        }
         clock_t end = clock(); //Paro el tiempo de ejecucción
         double tiempo = (double)(end - begin) / CLOCKS_PER_SEC;
        
        //FICHERO TIEMPO
        printf("Procediendo a crear el fichero tiempo");
        FILE *fichero;
        fichero = fopen("tiempos.txt", "w"); 
        fprintf(fichero,"%s","\n Nombre fichero:");
        fprintf(fichero,"%s",nombreFicheroProcesado);
        fprintf(fichero,"%s","\nTiempo de ejecucción programa(s):");
        fprintf(fichero,"%f",tiempo);
        fclose(fichero);
        printf("\nFichero de tiempo creado con éxito");
       

        //FICHERO PREDICCIONES
        printf("Procediendo a crear el fichero prediciones");
        int j;
        FILE *fichero1;
        fichero1 = fopen("predicciones.txt", "w");
        for (j = 0; j < COL; j++)
        {
            fprintf(fichero1,"%s%d%s","\n Prediccion",j+1,":");
            fprintf(fichero1,"%f",mediaAritmeticaVecinos[j]);
        
       
        }
        fclose(fichero1);
        printf("\nFichero de predicciones creado con éxito");

        //FICHERO MAPE
        printf("Procediendo a crear el fichero mape");
        FILE *fichero2;
        fichero2 = fopen("mape.txt", "w"); 
        for (j = 0; j < sizeof(vectorMAPESRealizados); j++)
        {
            fprintf(fichero2,"%s%d%s","\nMape de la fila",j+1,":");
            fprintf(fichero2,"%f",vectorMAPESRealizados[j]);
        }
        fclose(fichero2);
        printf("\nFichero de mape creado con éxito");
       
        //Limpiamos memoria
        fflush(stdout);
        fflush(stdin);
        
    }
    else
    { //hilo esclavo

        //** Recepción de resultados
        MPI_Recv(&ultDia, COL, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(&diaActual, COL, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(&dias, FIL * COL, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, NULL);
        // ** REGION PARALELA. Cálculo del vector vectorMAPE en paralelo con 4 hilos
        #pragma omp parallel for num_threads(4)
        
        for (i = 0; i < FIL; i++)
        {
            for (j = 0; j < COL; j++)
            {
                diaActual[j] = dias[i][j];
            }
            MAPEActual = calculaMAPE(diaActual, ultDia);
            vectorMAPESRealizados[i] = MAPEActual;
            printf("PID: [%d]/[%d] calculandoo el día [%d]  con mape:%f\n", pid, omp_get_thread_num(),i,MAPEActual);
            if (MAPEActual < MAPEGeneral)
            {
                MAPEGeneral = MAPEActual;
                indiceDiaMAPEMasBajo = i;
            }
        }
        
        //Calculo la media aritmética de los dos días siguientes al del MAPE más bajo
        for (j = 0; j < FIL; j++)
        {
            mediaAritmeticaVecinos[j] = (dias[indiceDiaMAPEMasBajo + 1][j] + dias[indiceDiaMAPEMasBajo + 2][j]) / 2;
        }

        //** Envío al maestro:
        MPI_Send(&mediaAritmeticaVecinos, COL, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&vectorMAPESRealizados, COL, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    // ** Finalización del entorno MPI
    MPI_Finalize();

    return 0;
    //FIN
}

float calculaMAPE(float diaActual[], float ultDia[])
{
    float MAPEActual = 0;

    for (int j = 0; j < 24; j++)
    {
        MAPEActual += (fabs((ultDia[j] - diaActual[j]) / ultDia[j]));
    }
    MAPEActual *= 100 / 24;
    MAPEActual *= 100; //Para calcular el porcentaje

    return MAPEActual;
}



