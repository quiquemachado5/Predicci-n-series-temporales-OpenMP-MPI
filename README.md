# Prediccion-series-temporales-OpenMP-MPI
Predicción series temporales OpenMP / MPI 

PREDICCIÓN DE SERIES TEMPORALES
           Hecho por: ENRIQUE MACHADO DOMÍNGUEZ y
			      STEPHAN PÉREZ MARÍN

A)	Descripción de la implementación.
Para la implementación de este problema de series temporales, hemos empleado OpenMP y MPI con los conocimientos que hemos aprendido en teoría y prácticas

Para empezar, leemos el fichero donde se encuentran los datos para ir almacenándolos en una matriz llamada días, que guarda solo los últimos 1000 días, con sus horas correspondientes.
(Hemos notado, que en algunos ficheros faltan filas pero aun así hemos modificado nuestra parte del código para que lea bien todos los ficheros)

Declaramos e inicializamos el número de procesos MPI para poder hacer el juego maestro-esclavo. Una vez comprobado que estamos en el proceso maestro (0), el maestro envía a cada uno de sus esclavos (prn) los diferentes parámetros. En nuestro caso, 3 vectores con los que iremos trabajando.

El esclavo los recibe y hace una serie de operaciones con ellos. 
A continuación, declaramos la parte donde paralelizamos con OpenMP y declaramos el número de hilos que queremos usar para calcular el MAPE de cada día (el cual se calcula ayudándonos de una función aparte, para simplificar el código), y la media aritmética de los vecinos

Después, se envían los resultados de los esclavos al maestro; donde este los recibe, almacena los resultados y después con cada uno de ellos crea y escribe en los ficheros correspondientes.



Para empezar a distribuir los procesos tuvimos claro que un proceso maestro iba a:
-	mandar a sus esclavos los datos con los que íbamos a trabajar (algunos rellenos y otros vacíos) 
-	volcar los datos de los últimos 1000 días en un vector
y los esclavos serían los encargados de:
-	calcular los resultados numéricos que servirían como solución al problema propuesto.

OpenMP se usa para el cálculo del vector de MAPE, en donde está declarado que haya 4 hilos que lo han simultáneamente. No hay otro lugar en el código en el que paralelizar sea y se vea tan claro (quizás en la función calculaMAPE también se podría haber paralelizado el cálculo del MAPEActual pero hemos preferido paralelizar únicamente una vez)

En resumen, los procesos se comunican entre ellos constantemente con los SEND y RECV en donde se van distribuyendo (pasando y recibiendo) datos en función de quien es maestro y quien esclavo. Los hilos por su parte, en una determinada parte del código de los esclavos, paralelizan el cálculo de la solución.













B)	Análisis y discusión de los tiempos de ejecución
Como se ha dicho en el punto C, en los equipos donde se ha probado el código, la velocidad de ejecución ha sido relativamente rápida por lo que los tiempos también lo han sido:


i. Tamaño de fichero
1x-			Tiempo de ejecución programa(s): 0.005969
10x- 			Tiempo de ejecución programa(s): 0.021810
100x-	             		Tiempo de ejecución programa(s): 0.023474
1000x-	             		Tiempo de ejecución programa(s): 0.025680

Como vemos, a mayor número de datos a leer en el fichero, mayor tiempo de ejecución

      ii.  Para cada fichero: 1,2,3 y 4 procesos
1x - 	4 procesos	Tiempo de ejecución programa(s):0.022081
1 proceso 	Tiempo de ejecución programa(s):0.000454
2 procesos 	Tiempo de ejecución programa(s):0.003362
3 procesos 	Tiempo de ejecución programa(s):0.018147

10x -	4 procesos 	Tiempo de ejecución programa(s):0.018686
1 proceso 	Tiempo de ejecución programa(s):0.000176
2 procesos 	Tiempo de ejecución programa(s):0.001170
3 procesos	Tiempo de ejecución programa(s):0.017401

100x  -	4 procesos 	Tiempo de ejecución programa(s):0.017807
1 proceso 	Tiempo de ejecución programa(s):0.000258
2 procesos 	Tiempo de ejecución programa(s):0.001338
3 procesos	Tiempo de ejecución programa(s):0.020200

1000x- 4 procesos	Tiempo de ejecución programa(s):0.025573
1 proceso 	Tiempo de ejecución programa(s):0.000436
2 procesos 	Tiempo de ejecución programa(s):0.002415
3 procesos 	Tiempo de ejecución programa(s):0.015433

Aquí ocurre lo mismo, a mayor datos a leer, más tarda y además se le añade que a mayor número de procesos, mayor tiempo. Así, el fichero 1000x con 4 procesos es el que más tarda
     
      



  iii. Para cada fichero: 1,2,3 y 4 hilos
1x – 4 hilos 		Tiempo de ejecución programa(s): 0.062334
        1 hilo 		Tiempo de ejecución programa(s): 0.001117
        2 hilos 		Tiempo de ejecución programa(s): 0.021295
        3 hilos 		Tiempo de ejecución programa(s): 0.029653

10x – 4 hilos 		Tiempo de ejecución programa(s): 0.015857
          1 hilo 		Tiempo de ejecución programa(s): 0.010603
          2 hilos		Tiempo de ejecución programa(s): 0.009712
          3 hilos		Tiempo de ejecución programa(s): 0.028935



100x - 4 hilos 		Tiempo de ejecución programa(s): 0.016791
            1 hilo 		Tiempo de ejecución programa(s): 0.010296 
            2 hilos 		Tiempo de ejecución programa(s): 0.021390
            3 hilos		Tiempo de ejecución programa(s): 0.016838

1000x -4 hilos 		Tiempo de ejecución programa(s): 0.031030
1 hilo 		Tiempo de ejecución programa(s): 0.005425
2 hilos 		Tiempo de ejecución programa(s): 0.015109
3 hilos 		Tiempo de ejecución programa(s): 0.020557

Lo mismo que ocurría con el tiempo en los procesos también ocurre con los hilos 

Cabe decir que hemos notado que depende de las otras tareas que este en ese momento realizando el ordenador los tiempos suben ligeramente

C)	Descripción hardware de los equipos utilizados para el desarrollo.
Para llevar acabo este trabajo, hemos usado dos equipos diferentes:
-	Lenovo usando una máquina virtual para Linux
-	Apple Macbook Air M1 
En donde se han descargado en ambos equipos OpenMP y MPI para poder ir probando cada cambio o novedad que introducíamos en el código.
Ambos equipos han ejecutado con relativa velocidad nuestro programa, como se ha visto en el punto B.
![image](https://user-images.githubusercontent.com/73646452/146823758-7798925b-2eb9-4d10-9d39-363d4bb7725c.png)

