// Pines del laser
#define laserControlPin 26
#define laserFanPin     27

// Pines del motor 1 / X
#define dir2_pin  16  //16 amarillo
#define step2_pin 17  //17 amarillo
#define MS21_pin  18  //18 rojo
#define MS22_pin  19  //19 rojo
#define MS23_pin  21  //21 rojo

// Pines del motor 2 / Y
#define dir1_pin  22  //22 blanco
#define step1_pin 23  //23 blanco
#define MS11_pin  25  //25 azul
#define MS12_pin  33  //33 azul
#define MS13_pin  32  //32 azul

// Defines de la imagen
#define OFFSET 12
#define img_size 60 // Definir el tamaño (cuadrado) de la matriz a dibujar
byte img_mat[img_size][img_size];

// Defines de tiempo
#define TIME_MS 1000

//Defines motor
enum driver_Conf {FULL_STEP, HALF_STEP, QUARTER_STEP, EIGHTH_STEP, SIXTEENTH_STEP};

int dir1 = 0;
int dir2= 0;
//Variables globales
int power = 0;
int acc = 0;
int time_delay= 0;

// Tipo instruccion
enum instruction {IDLE, PONG, MOVEX, MOVEY, RECVDATA, PRINTDATA, DRAW, LASER, INVALID,SHUT,TIME};

// Estructura para asociar una string con una instruccion
typedef struct{
  String key; 
  instruction val;
} t_stringdic;

// Lista de mensajes que podemos recibir por serial, incluyendo su equivalente en tipo instruccion

t_stringdic stringdic[] = {
  {"idle", IDLE},
  {"ping", PONG},
  {"RecvData", RECVDATA},
  {"printdata", PRINTDATA},
  {"movex", MOVEX},
  {"movey", MOVEY},
  {"draw", DRAW},
  {"laser", LASER},
  {"shut", SHUT},
  {"time", TIME}
};

// Longitud de la lista de mensajes
#define NKEYS (sizeof(stringdic)/sizeof(t_stringdic))

//control del puerto serie sacado de Arduino
String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
char bufferChar[10];
int bufferSize=0;

// Inicializamos
void setup() {
  Serial.begin(115200);
  Serial.flush(); // Asegurarse de que el buffer esté vacío
  inputString.reserve(1024); 

  pinMode(dir1_pin,OUTPUT);
  pinMode(dir2_pin,OUTPUT);
  pinMode(step1_pin,OUTPUT);
  pinMode(step2_pin,OUTPUT);
  pinMode(MS11_pin,OUTPUT);
  pinMode(MS12_pin,OUTPUT);
  pinMode(MS13_pin,OUTPUT);
  pinMode(MS21_pin,OUTPUT);
  pinMode(MS22_pin,OUTPUT);
  pinMode(MS23_pin,OUTPUT);

  digitalWrite(dir1_pin, LOW);
  digitalWrite(dir2_pin, LOW);
  digitalWrite(step1_pin, LOW);
  digitalWrite(step2_pin, LOW);
  digitalWrite(MS11_pin, LOW);
  digitalWrite(MS12_pin, LOW);
  digitalWrite(MS13_pin, LOW);
  digitalWrite(MS21_pin, LOW);
  digitalWrite(MS22_pin, LOW);
  digitalWrite(MS23_pin, LOW);
  digitalWrite(laserFanPin, LOW);
  digitalWrite(laserControlPin, LOW);

  config(QUARTER_STEP); // FULL_STEP, HALF_STEP, QUARTER_STEP, EIGHTH_STEP, SIXTEENTH_STEP 


}

// Bucle infinito, se activa cuando detectamos un mensaje en el serial
void loop() {
    if (stringComplete) {
      executeInstruction(decodeSerial());
      inputString = "";
      stringComplete = false;
    }
}

// Lectura de los eventos en el serial
void serialEvent() {
  while (Serial.available() && !stringComplete) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

// Cuando detectamos un mensaje en el serial, buscamos si hay alguna instruccion asociada a ese mensaje
// https://stackoverflow.com/questions/4014827/how-can-i-compare-strings-in-c-using-a-switch-statement
instruction decodeSerial(){

  for(int i=0; i<NKEYS; i++){
    t_stringdic *stringPage = &stringdic[i];
    if(inputString.indexOf(stringPage->key)>-1)
      return stringPage->val;
  }
  return INVALID;
}

// Ejecutamos codigo dependiendo de la instruccion que recibimos
void executeInstruction(instruction ins){
  switch (ins){
    case PONG: Serial.println("pong"); break;
    case RECVDATA: recvData(); break;
    case PRINTDATA: printData(); break;
    case DRAW: draw(); break;
    case MOVEX: moveXuart(); break;
    case MOVEY: moveYuart(); break;
    case LASER: laser_power(); break;
    case SHUT: shut(); break;
    case TIME: time_set(); break;
    case IDLE: break;
    
    default: break;
  }
}


void recvData(){
  // Serial.println("Receiving data...");
  int fila = inputString.substring(9,11).toInt();
  
  for(int i=0; i<img_size; i++){
    img_mat[fila][i] = (byte)atoi(&inputString[OFFSET+2*i]);
  }
  Serial.println("Reception complete");
}

void printData(){
  for(int i=0; i<img_size; i++){
    for(int j=0; j<img_size; j++){
      if(j==img_size-1)
        Serial.println(String(img_mat[i][j]));
      else
        Serial.print(String(img_mat[i][j]) + " ");
    }
  }
}

void config(driver_Conf driver){
  switch (driver) {
    case FULL_STEP:
      digitalWrite(MS11_pin, LOW);
      digitalWrite(MS12_pin, LOW);
      digitalWrite(MS13_pin, LOW);
      digitalWrite(MS21_pin, LOW);
      digitalWrite(MS22_pin, LOW);
      digitalWrite(MS23_pin, LOW);
      Serial.println("Full_step 000");
      break;
    case HALF_STEP:
      digitalWrite(MS11_pin, HIGH);
      digitalWrite(MS12_pin, LOW);
      digitalWrite(MS13_pin, LOW);
      digitalWrite(MS21_pin, HIGH);
      digitalWrite(MS22_pin, LOW);
      digitalWrite(MS23_pin, LOW);
      Serial.println("Full_step 100");
      break;
    case QUARTER_STEP:
      digitalWrite(MS11_pin, LOW);
      digitalWrite(MS12_pin, HIGH);
      digitalWrite(MS13_pin, LOW);
      digitalWrite(MS21_pin, LOW);
      digitalWrite(MS22_pin, HIGH);
      digitalWrite(MS23_pin, LOW);
      Serial.println("Full_step 010");
      break;
    case EIGHTH_STEP:
      digitalWrite(MS11_pin, HIGH);
      digitalWrite(MS12_pin, HIGH);
      digitalWrite(MS13_pin, LOW);
      digitalWrite(MS21_pin, HIGH);
      digitalWrite(MS22_pin, HIGH);
      digitalWrite(MS23_pin, LOW);
      Serial.println("Full_step 110");
      break;
    case SIXTEENTH_STEP:
      digitalWrite(MS11_pin, HIGH);
      digitalWrite(MS12_pin, HIGH);
      digitalWrite(MS13_pin, HIGH);
      digitalWrite(MS21_pin, HIGH);
      digitalWrite(MS22_pin, HIGH);
      digitalWrite(MS23_pin, HIGH);
      Serial.println("Full_step 111");
      break;

    default:
      digitalWrite(MS11_pin, LOW);
      digitalWrite(MS12_pin, LOW);
      digitalWrite(MS13_pin, LOW);
      digitalWrite(MS21_pin, LOW);
      digitalWrite(MS22_pin, LOW);
      digitalWrite(MS23_pin, LOW);
      Serial.println("Error: valor de driver no válido");
      break;
  }
}

void moveX(int pasos, int dir){
  //dir = 0 giro horario dir = 1 antihorario
  if(dir == 1){
    digitalWrite(dir1_pin, LOW);
  }else if (dir == 0){
    digitalWrite(dir1_pin, HIGH);
  }else{
    Serial.println("Comando No valido, dir mal seleccionado");
  }
  for(int pointer = 0; pointer < pasos ; pointer ++){
    digitalWrite(step1_pin,HIGH);
    delayMicroseconds(TIME_MS);
    digitalWrite(step1_pin,LOW);
    delayMicroseconds(TIME_MS);
    delay(10);
  }
}

void moveY(int pasos, int dir){
  if(dir == 1){
    digitalWrite(dir2_pin, HIGH);
  }else if (dir == 0){
    digitalWrite(dir2_pin, LOW);
  }else{
    Serial.println("Comando No valido, dir mal seleccionado");
  }
  for(int pointer = 0; pointer < pasos ; pointer ++){
    digitalWrite(step2_pin,HIGH);
    delayMicroseconds(TIME_MS);
    digitalWrite(step2_pin,LOW);
    delayMicroseconds(TIME_MS);
    delay(10);
  }
}

void moveXuart(){
  //dir = 0 giro horario dir = 1 antihorario
  int pasos = inputString.substring(5,19).toInt();
  (pasos<0) ? dir1 = 1:dir1 = 0;
  (pasos<0) ? pasos = -pasos : pasos = pasos;
  Serial.println(pasos);
  Serial.println(dir1);
  moveX(pasos,dir1);
  Serial.println("stopX");
}

void moveYuart(){
  int pasos = inputString.substring(5,19).toInt();
  (pasos<0) ? dir2 = 1 : dir2 = 0;
  (pasos<0) ? pasos = -pasos : pasos = pasos;
  Serial.println(pasos);
  Serial.println(dir2);
  moveY(pasos,dir2);
  Serial.println("stopY");
}

void draw(){

  Serial.println("Drawing, await for completion...");
  power=1;
  dir2 = 1;
  for(int pointerFila = 0; pointerFila < img_size; pointerFila++){

    if((pointerFila == 0) || (pointerFila % 2 == 0)){
      dir1 = 0;
      for(int pointerCol = 0; pointerCol < img_size; pointerCol++){
        if(img_mat[pointerFila][pointerCol] > 0){
          burn(img_mat[pointerFila][pointerCol],dir1);
        }
        moveX(1,dir1);
    }
    } else{
      dir1 = 1;
      for(int pointerCol = img_size -1 ; pointerCol > -1; pointerCol--){
        if(img_mat[pointerFila][pointerCol] > 0){
          burn(img_mat[pointerFila][pointerCol],dir1);
        }
        moveX(1,dir1);
    }
    } 


    moveY(1,dir2);
  }
  moveY(img_size,0);
  Serial.println("Tosta2");
}

void laser_power(){
  power =  inputString.substring(6,9).toInt();
  acc=power;
  (power>255) ? power = 255 : power = power;
  (power<0) ? power = 0 : power = power;
  Serial.print("Laser's Power setted to: ");
  Serial.println(power);
  analogWrite(laserControlPin, 1);
  
}

void burn(byte celda, int dir){
  analogWrite( laserControlPin, acc);
  delay(celda*time_delay);
  analogWrite( laserControlPin, 0);
}

void shut(){
  analogWrite( laserControlPin, 0);
}
void time_set(){
  time_delay =  inputString.substring(5,9).toInt();
  Serial.print("Time setted to: ");
  Serial.println(time_delay);
  
}
