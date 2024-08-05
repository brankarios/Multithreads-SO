# Proyecto #2: Multithreads

## Integrantes: 

César Carios (30136117) & Jhonatan Homsany (30182893)

## Pasos para compilar y ejecutar:

La función de este programa es recibir archivos de texto por consola que contienen cualquier cantidad de palabras, ordenar esas palabras y luego unir todas las palabras de los archivos de texto en un solo archivo. Por pantalla se imprimen algunas estadísticas del ordenamiento hecho. Lo importante es que todo el manejo de archivos de este programa se implementó usando hilos, y donde fue necesario, semáforos. Pasos para ejecutar el programa:

1. Abrir la terminal de Linux en la carpeta donde se descargaron/descomprimieron los 5 archivos entregados.

2. Ejecutar el comando _make_ o _make all_ para compilar el código fuente.

3. Ejecuta el comando _./pf1_, seguido de los nombres de los archivos de texto con los que se trabajará, por ejemplo: a1.txt a2.txt...

4. El programa se ejecutará y tendrá como salida algunas estadísticas del ordenamiento hecho y generará por cada archivo de texto, ese mismo archivo de texto ordenado de forma lexicográfica decreciente y un archivo llamado _.sorted.txt_ que contiene todas las palabras ordenadas de los arhcivos suministrados.

5. Ejecutar el comando _make clean_ para remover los archivos .o y el ejecutable que genera el compilado.

## IMPORTANTE: Consideraciones sobre la ejecución del proyecto

Es importante destacar que originalmente este código fue testeado y creado usando WSL (Windows Subsystem Linux). Específicamente WLS: Ubuntu versión 22.04.4 LTS. Se testeó en Mint y Lubuntu y no compila correctamente, por favor, ejecutar en Ubuntu.