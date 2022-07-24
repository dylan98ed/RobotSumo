/****************************************************************************************************************
 * DESCRIPCION: Este programa hace mover al robot-sumo hacia adelante y cuando detecta que se esta por salir del 
 *              tatami gira y vuelve a avanzar hacia adelante
 * 
 * COMPONENTES:
 *    Arduino Uno.
 *    4 Sensor IR seguidor de línea.
 *    4 Motores DC con caja reductora.
 *    2 Doble puente H.
 *    
 * ESTADO: EN PROCESO
 ****************************************************************************************************************/
 
int MOT1_C1 = 6;        //motor1 conector 1 lado izquierdo
int MOT1_C2 = 7;        //motor1 conector 2
int MOT2_C1 = 8;        //motor2 conector 1 lado derecho
int MOT2_C2 = 9;        //motor2 conector 2

int SENS_LIN_FDER = 2;  //sensor de linea Frente-Derecho
int SENS_LIN_TDER = 3;  //sensor de linea Trasero-Derecho
int SENS_LIN_FIZQ = 4;  //sensor de linea Frente-Izquierdo
int SENS_LIN_TIZQ = 5;  //sensor de linea Trasero-Izquierdo

const int SENS_ULTRASON_TRIGGER = 10; //Pin digital 10 para el TRIGGER del sensor ultrasonido
const int SENS_ULTRASON_ECHO = 11;    //Pin digital 11 para el ECHO del sensor ultrasonido

int LINEA_DETECTADA = 1; //DETECCION DE LINEA BLANCA = 0
const int LLAVE = 12;

char comando = 's'; //variable de la maquina de estado.

void setup() 
{

  pinMode(LLAVE, INPUT); //SI CONECTAMOS ESTA LLAVE A 5V EL MOTOR AVANZA ATR
  
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

  delay(5000); //5 segundos iniciales de quietud
}

void loop() 
{ 
  for(;;) 
  {
    delay(50);
    comandos(comando);
    
    if(digitalRead(LLAVE) == HIGH)  //el robot avanza ignorando los sensores
    {
      comandos('m');
    }
    else
    {
      if(checkSenLineaTodos() == LINEA_DETECTADA)
      {
        comandos('s');
        checkSensorLinea();
      }
      else 
      {
        comandos('m');
        checkEnemy();
      }
    }
  }
}

void comandos(char comando)
{
  switch(comando) 
  {
    case 'm':                           //el robot avanza.
      marcha(MOT1_C1,MOT1_C2);
      marcha(MOT2_C1,MOT2_C2);
      break;
    case 'c':                           //el robot retrocede.
      contramarcha(MOT1_C1,MOT1_C2);
      contramarcha(MOT2_C1,MOT2_C2);
      break;
    case 's':                           //el robot se detiene
      parada(MOT1_C1,MOT1_C2);
      parada(MOT2_C1,MOT2_C2);
      break;
    case 'd':                           //el motor der avanza, izq retrocede
      giroDer(MOT1_C1,MOT1_C2,MOT2_C1,MOT2_C2);
      break;
    case 'i':                           //el motor izq avanza, der retrocede
      giroIzq(MOT1_C1,MOT1_C2,MOT2_C1,MOT2_C2);
      break;
    default:                            //el robot se detiene
      parada(MOT1_C1,MOT1_C2); //motor1
      parada(MOT2_C1,MOT2_C2); //motor2
      break;
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

long medirDistancia() 
{
  long t; //tiempo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(SENS_ULTRASON_TRIGGER, HIGH);
  delayMicroseconds(10);                      //Enviamos un pulso de 10us
  digitalWrite(SENS_ULTRASON_TRIGGER, LOW);
  
  t = pulseIn(SENS_ULTRASON_ECHO, HIGH);      //obtenemos el ancho del pulso
  // 1/59 = 0.0169492
  d = t * 0.0169492;                          //escalamos el tiempo a una distancia en cm

  return d;
}

//calculo de promedio de 10 distancias sensadas por el ultrasonido
long promedioDistancia()
{
  long distancia;
  long distancia_total = 0;

  for(int i = 10 ; i>0; i--)
  {
    distancia = medirDistancia(); 
    distancia_total = distancia_total + distancia;
  }
  // 1/10 = 0.1
  return (distancia_total * 0.1);
 }

//busqueda del contrincante
void checkEnemy()
{ 
  long distancia_promedio = promedioDistancia();
  
  if(distancia_promedio <= 75) // el ring tiene un area de 175cm o 154cm -- alrededor hay 100cm vacio
  {
    comandos('m');
  }
  else
  {
    comandos('i');
  }
}

int checkSenLineaTodos() //VERIFICO SI ALGUN SENSOR DETECTA ALGO
{
  int sensor1, sensor2, sensor3, sensor4;
  sensor1 = leerSensorLinea(SENS_LIN_FDER);
  sensor2 = leerSensorLinea(SENS_LIN_TDER);
  sensor3 = leerSensorLinea(SENS_LIN_FIZQ);
  sensor4 = leerSensorLinea(SENS_LIN_TIZQ);
  return (sensor1 & sensor2 & sensor3 & sensor4); //linea_cerca = 0 significa LINEA BLANCA CERCA AHHHH CORRAN
}

//leo sensor
int leerSensorLinea(int sensor)
{
  int value = 1;
  value = digitalRead(sensor);
  return value;
}

//verifico cual sensor detecto la linea blanca
void checkSensorLinea()
{
  int sensorPin;
  sensorPin = leerSensorLinea(SENS_LIN_FDER); //sensor de linea Frente-Derecho
  if(sensorPin == LINEA_DETECTADA)
  {
    reaccionFrenteDer();
  }
  else
  {
    sensorPin = leerSensorLinea(SENS_LIN_TDER); //sensor de linea Trasero-Derecho
    if(sensorPin == LINEA_DETECTADA)
    {
      reaccionTraseroDer();
    }
    else
    {
      sensorPin = leerSensorLinea(SENS_LIN_FIZQ); //sensor de linea Frente-Izquierdo
      if(sensorPin == LINEA_DETECTADA)
      {
        reaccionFrenteIzq();
      }
      else
      {
        sensorPin = leerSensorLinea(SENS_LIN_TIZQ); //sensor de linea Trasero-Izquierdo
        if(sensorPin == LINEA_DETECTADA)
        {
          reaccionTraseroIzq();
        }
      }
    }
  }
}

//reaccion a sensor frente-derecho
void reaccionFrenteDer()
{
  comandos('c');
  delay(2000);
  comandos('i');
  tiempoGiro(45);
  comandos('m');
}

//reaccion a sensor frente-izquierdo
void reaccionFrenteIzq()
{
  comandos('c');
  delay(2000);
  comandos('d');
  tiempoGiro(45);
  comandos('m');
  }

//reaccion a sensor trasero-derecho
void reaccionTraseroDer()
{
  //hay que chekar!!
  
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE DERECHO -- DOBLA LAS RUEDAS PARA LA DER ?
  comandos('d');
  delay(2000);
  
  //UNA VEZ QUE SALE DE PELIGRO INTENTA TIRAR AL ENEMIGO GIRANDO PARA LA IZQUIERDA (asumo que van a quedar en paralelo mirando al borde -- el enemy estaria a la izq)
  comandos('i');
  }

//reaccion a sensor trasero-izquierdo
void reaccionTraseroIzq()
{
  //hay que chekar!!
  
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE IZQUIERDO -- DOBLA LAS RUEDAS PARA LA IZQ ?
  comandos('i');
  delay(2000);
  //UNA VEZ QUE SALE DE PELIGRO INTENTA TIRAR AL ENEMIGO GIRANDO PARA LA DERECHA (asumo que van a quedar en paralelo mirando al borde -- el enemy estaria a la der)
  comandos('d');
  }

int tiempoGiro(int angulo) //calculo el tiempo que requiere el robot para girar cierto angulo
{
  int tgiro360 = 8; //tiempo que tarda en dar un giro completo 8 segundos -- aprox

  //REGLA DE 3 SIMPLES --- esto capaz podria estar en una funcion aparte
  // 1/360 = 0.00277778
  return ((tgiro360 * angulo) * 0.00277778);
}
