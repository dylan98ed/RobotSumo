/****************************************************************************************************************
 * DESCRIPCION: Este programa ejecuta un test del robot. para esto realiza los siguientes movimientos.
 *              2s robot detenido
 *              2s motor derecho hacia adelante
 *              2s motor derecho hacia atras
 *              2s motor izquierdo hacia adelante
 *              2s motor izquierdo hacia atras
 *              1s motores detenidos
 *              2s motores giro derecha
 *              1s motores detenidos
 *              2s motores giro izquierda
 *              1s motores detenidos
 *              prueba seguidores de linea:
 *              ninguno detecta: motores detenidos
 *              del der detecta: motor derecho hacia adelante
 *              tra der detecta: motor derecho hacia atras
 *              del izq detecta: motor izquierdo hacia adelante
 *              tra izq detecta: motor izquierdo hacia adelante
 *              
 * 
 * COMPONENTES:
 *    Arduino Uno.
 *    4 Sensor IR seguidor de línea.
 *    4 Motores DC con caja reductora.
 *    2 Doble puente H.
 *    
 * ESTADO: FUNCIONA
 ****************************************************************************************************************/
 
int MOT1_C1 = 10;        //motor1 conector 1 lado izquierdo
int MOT1_C2 = 11;        //motor1 conector 2
int MOT2_C1 = 12;        //motor2 conector 1 lado derecho
int MOT2_C2 = 13;        //motor2 conector 2

int SENS_LIN_FDER = 2;  //sensor de linea Frente-Derecho
int SENS_LIN_TDER = 3;  //sensor de linea Trasero-Derecho
int SENS_LIN_FIZQ = 4;  //sensor de linea Frente-Izquierdo
int SENS_LIN_TIZQ = 5;  //sensor de linea Trasero-Izquierdo

const int SENS_ULTRASON_TRIGGER = 6; //Pin digital 10 para el TRIGGER del sensor ultrasonido
const int SENS_ULTRASON_ECHO = 7;    //Pin digital 11 para el ECHO del sensor ultrasonido

int LINEA_DETECTADA = 0; //DETECCION DE LINEA BLANCA = 0

void setup() 
{
  //seteo pines de entrada de motores, sensores de linea y ultrasonido
  pinMode(MOT1_C1, OUTPUT);
  pinMode(MOT1_C2, OUTPUT);
  pinMode(MOT2_C1, OUTPUT);
  pinMode(MOT2_C2, OUTPUT);
  
  pinMode(SENS_LIN_FDER, INPUT);
  pinMode(SENS_LIN_TDER, INPUT);
  pinMode(SENS_LIN_FIZQ, INPUT);
  pinMode(SENS_LIN_TIZQ, INPUT);
  
  pinMode(SENS_ULTRASON_TRIGGER, OUTPUT);
  pinMode(SENS_ULTRASON_ECHO, INPUT);
  
  digitalWrite(SENS_ULTRASON_TRIGGER, LOW);//Inicializamos el pin con 0

  delay(2000); //5 segundos iniciales de quietud
}

void loop() 
{
  marcha(MOT2_C1,MOT2_C2);        //motor derecho
  delay(2000);
  contramarcha(MOT2_C1,MOT2_C2);
  delay(2000);
  parada(MOT2_C1,MOT2_C2);
  marcha(MOT1_C1,MOT1_C2);        //motor izquierdo
  delay(2000);
  contramarcha(MOT1_C1,MOT1_C2);
  delay(2000);
  parada(MOT2_C1,MOT2_C2);
  parada(MOT1_C1,MOT1_C2);
  delay(1000);
  giroDer(MOT1_C1,MOT1_C2,MOT2_C1,MOT2_C2);
  delay(2000);
  parada(MOT2_C1,MOT2_C2);
  parada(MOT1_C1,MOT1_C2);
  delay(1000);
  giroIzq(MOT1_C1,MOT1_C2,MOT2_C1,MOT2_C2);
  delay(2000);
  parada(MOT2_C1,MOT2_C2);
  parada(MOT1_C1,MOT1_C2);
  delay(1000);
  for(;;) 
  {
    if(leerSensorLinea(SENS_LIN_FDER) == LINEA_DETECTADA)
      marcha(MOT2_C1,MOT2_C2);
    else
    {
      if(leerSensorLinea(SENS_LIN_TDER) == LINEA_DETECTADA)
        contramarcha(MOT2_C1,MOT2_C2);
      else
        parada(MOT2_C1,MOT2_C2);
    }

    if(leerSensorLinea(SENS_LIN_FIZQ) == LINEA_DETECTADA)
      marcha(MOT1_C1,MOT1_C2);
    else
    {
      if(leerSensorLinea(SENS_LIN_TIZQ) == LINEA_DETECTADA)
        contramarcha(MOT1_C1,MOT1_C2);
      else
        parada(MOT1_C1,MOT1_C2);
    }
  }
}
 
void marcha(int b1,int b2) 
{
  digitalWrite(b1, HIGH); 
  digitalWrite(b2, LOW); 
}

void contramarcha(int b1, int b2) 
{
  digitalWrite(b1, LOW); 
  digitalWrite(b2, HIGH); 
}

void parada(int b1, int b2) 
{
  digitalWrite(b1, LOW); 
  digitalWrite(b2, LOW); 
}

void giroIzq(int b1, int b2, int b3, int b4) 
{
  contramarcha(b1,b2); //motor1 - izq
  marcha(b3,b4); //motor2 - der
}

void giroDer(int b1, int b2, int b3, int b4) 
{
  marcha(b1,b2); //motor1 - izq
  contramarcha(b3,b4); //motor2 - der
}

//leo sensor
int leerSensorLinea(int sensor)
{
  int value = 1;
  value = digitalRead(sensor);
  return value;
}
