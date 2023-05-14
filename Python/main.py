from PIL import Image
import numpy as np
import sys

import time
import serial
import winsound

def sonido():
    # Reproducir el sonido de un beep
    duration = 1000  # Duración en milisegundos
    frequency = 2000  # Frecuencia en Hz
    for i in range(4):
        winsound.Beep(frequency, duration)
        time.sleep(0.5)

# Valores para cofigurar el tamaño de la imagen a grabar.
ANCHO = 60
ALTO = 60
# Abrir la imagen y convertirla a escala de grises
# Valor 'L' para escala de grises.
# Valor '1' para imagenes que sean mas "pixelart".
imagen = Image.open('C:/Users/Usuario/OneDrive - Universidad Politécnica de Madrid/Entrega/CodigoPyImagenes/sb_logo.png').convert('1')

# imagen.show() # Descomenta para visualizar la imagen
imagen = imagen.resize((ANCHO, ALTO), resample=Image.BICUBIC)
# imagen.show() # Descomenta para visualizar la imagen
# Si quieres guardar la imagen descomenta la linea inferior y asegurate de poner una ruta valida.
#imagen.save("imagen_guardada.jpg")

# Elige la configuracion de comunicación.
ser = serial.Serial('COM9', 115200)

# Obtener la anchura y la altura de la imagen
ancho, alto = imagen.size

if ANCHO != ancho or ALTO != alto:
    print(f'ancho = {ancho} y alto = {alto}')
    exit()

# Crear una matriz vacía para almacenar los valores de los píxeles
matriz_pixels = np.zeros((alto, ancho))

# Recorrer cada píxel de la imagen y asignar 1 a la matriz binaria si el píxel no es blanco
for y in range(alto):
    for x in range(ancho):
        valor_pixel = imagen.getpixel((x, y))
        if valor_pixel < 220:  # hay que comprobar esto
            matriz_pixels[y][x] = 1

# Guardar la matriz en un archivo CSV
np.savetxt('matriz.csv', matriz_pixels, delimiter=',')

# Imprimir la matriz binaria resultante por 
np.set_printoptions(threshold=np.inf)


# PROCESADO DE LA MATRIZ

NUM_FILAS = alto
NUM_COL = ancho

matriz = matriz_pixels
matriz_final = np.zeros((NUM_FILAS, NUM_COL))
media_mascara = 0

# Recorremos las filas de la matriz
for fila in range(NUM_FILAS):

    # Si estamos en la fila 0, recorremos de izquierda a derecha
    if fila == 0:
        for col in range(NUM_COL):
            if col > 0:
                mascara = [matriz[fila][col - 1]]
                media_mascara = sum(mascara) / len(mascara)

            valor = matriz[fila][col]
            if valor == 1:
                # print(f'Encontré un 1 en: {fila,col}')
                matriz_final[fila][col] = 1
                if media_mascara > 0:
                    matriz_final[fila][col] = matriz_final[fila][col] * (5 - sum(mascara))
                    # print(f'MASK: {sum(mascara)}')

    # Si estamos en una fila impar, recorremos de derecha a izquierda
    elif fila % 2 != 0:
        for col in range(NUM_COL - 1, -1, -1):
            if col == NUM_COL - 1:
                mascara = [matriz[fila - 1][col], matriz[fila - 1][col - 1], matriz[fila][col - 1]]
            elif col == 0:
                mascara = [matriz[fila][col + 1], matriz[fila - 1][col], matriz[fila - 1][col + 1]]
            else:
                mascara = [matriz[fila][col + 1], matriz[fila - 1][col], matriz[fila - 1][col + 1],
                           matriz[fila - 1][col - 1]]
            media_mascara = sum(mascara) / len(mascara)
            valor = matriz[fila][col]
            if valor == 1:
                # print(f'Encontré un 1 en: {fila,col}')
                matriz_final[fila][col] = 1
                if media_mascara > 0:
                    matriz_final[fila][col] = matriz_final[fila][col] *  (5 - sum(mascara))
                    # print(f'MASK: {sum(mascara)}')

    # Si estamos en una fila par, recorremos de derecha a izquierda
    else:
        for col in range(NUM_COL - 1, -1, -1):
            if col == 0:
                mascara = [matriz[fila - 1][col], matriz[fila - 1][col + 1], matriz[fila][col + 1]]
            elif col == NUM_COL - 1:
                mascara = [matriz[fila - 1][col], matriz[fila - 1][col - 1], matriz[fila][col - 1]]
            else:
                mascara = [matriz[fila - 1][col], matriz[fila - 1][col - 1], matriz[fila - 1][col + 1],
                           matriz[fila][col + 1]]
            media_mascara = sum(mascara) / len(mascara)
            valor = matriz[fila][col]
            if valor == 1:
                # print(f'Encontré un 1 en: {fila,col}')
                matriz_final[fila][col] = 1
                if media_mascara > 0:
                    matriz_final[fila][col] = matriz_final[fila][col] *  (5 - sum(mascara))
                    # print(f'MASK: {sum(mascara)}')
                else:
                    matriz_final[fila][col] = 5

# Descomenta la linea inferior para guardar la matriz en un archivo csv, asegurate de que la ruta sea correcta.                   
# np.savetxt('matriz.csv', matriz_final, delimiter=',')
matriz_final = matriz_final.astype(int)

cadena = list()
cont_fila = 0
for f in matriz_final:
    cadena.append("RecvData {:02d}=".format(cont_fila) + ','.join(str(v) for v in f))
    cont_fila += 1

for i in cadena:
    print(i)

time_ms = 200
time.sleep(1)
ser.flushInput()
while True:
    print("Inserte un comando")
    print("Comandos posibles: ping, printdata, send, done, draw, laser, movex, movey")
    entradaTeclado = input("").strip()
    if entradaTeclado == "done":
        print("Programa finalizado")
        break
    if entradaTeclado == "send":
        for i in cadena:
            ser.write((i + "\r\n").encode())
            time.sleep(time_ms / 1000)
            continue

    ser.write((entradaTeclado + "\r\n").encode())

    if entradaTeclado == "draw":
        while True:
            print(ser.readline().decode("ascii"))
            if 'Tosta2' in ser.readline().decode("ascii"):
                print('Drawing done!')
                sonido()
                break 
    elif entradaTeclado == "movex":
        while True:
            print(ser.readline().decode("ascii"))
            if 'stopX' in ser.readline().decode("ascii"):
                print("motorX has stopped spining")
                break  
    elif entradaTeclado == "movey":
        while True:
            print(ser.readline().decode("ascii"))
            if 'stopY' in ser.readline().decode("ascii"):  
                print("motorY has stopped spining")
                break

    else:
        time.sleep(time_ms / 1000)
        line = 0
        while ser.in_waiting:
            print(str(line) + " " + ser.readline().decode("ascii"))
            line = line + 1

ser.close()
