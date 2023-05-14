# Tostados
Trabajo realizado en la asignatura ELCO, de la ETSIT durante el curso 2022-23

Este trabajo implementa el funcionamiento de un módulo de grabado para tostadas usando un láser. Este codigo controla mediante un ESP32,
el movimiento de dos motores y el encendido y apagado de un láser. 

Aunque el prototipo se puede hacer con un ESP32, se proporciona en la carpeta PCB/ los archivos necesarios para fabricar una PCB que lo 
sustituiría.

Con el código que se encuentra en Python/ se realiza el procesado de la imagen y se establece la comunicación con el modulo ESP32.
Lista de comandos disponibles y su sintaxis:
  - ping -> devuelve pong.
  - printdata -> devuelve el contenido en array de la imagen pasada al ESP32.
  - movex# -> # representa un número de pasos que se dan en el ejeX.
  - movey# -> # representa un número de pasos que se dan en el ejeY.
  - draw -> realiza el grabado de la imagen. 
  - laser=# -> establece la potencia del laser al número #, # puede tomar valores entre 0 y 255.
  - shut -> apaga el laser.
  - time=# -> establece el delay en el recorrido del array a # en ms.

Con el código que se encuentra en Arduino/ se seleccionan las conexiones del ESP32 con el resto de componentes. Además se controla el
movimiento/encendido/apagado de estos.

Con el código que se encuentra en Matlab/ se implementa un modo de visualización del láser.
