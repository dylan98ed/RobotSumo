/****************************************************************************************************************
 * DESCRIPCION: Este programa hace mover al robot-sumo hacia adelante y cuando detecta que se esta por salir del 
 *              tatami gira y vuelve a avanzar hacia adelante
 *              -IGNORA LOS SENSORES DE LINEA SI DETECTA ALGO EL ULTRASONIDO!
 *              -Implemento temporizadores en vez de delays gg
 *              -Contiene una interrupcion en el tiempo de giro si detecta al enemigo
 *              -INTENTO DE CAMBIO DE PINES SI DETECTA ALGO
 * 
 * COMPONENTES:
 *    Arduino Uno.
 *    4 Sensor IR seguidor de línea.
 *    4 Motores DC con caja reductora.
 *    2 Doble puente H.
 *    
 * ESTADO: EN PROCESO
 ****************************************************************************************************************/

//LIBRERIA PARA UTILIZAR EL Temporizador DE LA PLACA
#include <TimerOne.h>;
 
const int MOT1_C1 = 14;        //motor1 conector 1 lado izquierdo
const int MOT1_C2 = 15;        //motor1 conector 2
const int MOT2_C1 = 16;        //motor2 conector 1 lado derecho
const int MOT2_C2 = 17;        //motor2 conector 2
//salidas analogicas A0-A7 -- pines 14-22 ?

int SENS_LIN_FDER = 2;  //sensor de linea Frente-Derecho
int SENS_LIN_TDER = 3;  //sensor de linea Trasero-Derecho
int SENS_LIN_FIZQ = 4;  //sensor de linea Frente-Izquierdo
int SENS_LIN_TIZQ = 5;  //sensor de linea Trasero-Izquierdo

int SENS_ULTRASON_TRIGGER_1 = 8; //Pin digital 10 para el TRIGGER del sensor ultrasonido
int SENS_ULTRASON_ECHO_1 = 9;    //Pin digital 11 para el ECHO del sensor ultrasonido
int SENS_ULTRASON_TRIGGER_2 = 10; //Pin digital 10 para el TRIGGER del sensor ultrasonido
int SENS_ULTRASON_ECHO_2 = 11;    //Pin digital 11 para el ECHO del sensor ultrasonido

const int T_GIRO360 = 8; //tiempo que tarda en dar un giro completo 8 segundos -- aprox

//variable que utilizo para verificar cuando hubo una interrupcion en un bucle
const int Flag_Interrupcion = 0; 

const int LINEA_DETECTADA = 1; //DETECCION DE LINEA BLANCA = 0
 
//Variable para la interrupcion
volatile long int tempo=0;

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
  
  pinMode(SENS_ULTRASON_TRIGGER_1, OUTPUT);
  pinMode(SENS_ULTRASON_ECHO_1, INPUT);
  digitalWrite(SENS_ULTRASON_TRIGGER_1, LOW);//Inicializamos el pin con 0

  pinMode(SENS_ULTRASON_TRIGGER_1, OUTPUT);
  pinMode(SENS_ULTRASON_ECHO_1, INPUT);
  digitalWrite(SENS_ULTRASON_TRIGGER_1, LOW);//Inicializamos el pin con 0

  Timer1.initialize(1000000);            //configura el timer en 1 segundo
  //1000000 microsegundos = 1 segundo
  Timer1.attachInterrupt(Temporizador);   //configura la interrupcion del timer

  delay(5000); //5 segundos iniciales de quietud
}

void loop() 
{ 
  for(;;) 
  {  
    checkEnemy();
  }
}

void maquinaEstados(char comando)
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

long medirDistancia(int trigger, int echo) 
{
  long t; //tiempo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);                      //Enviamos un pulso de 10us
  digitalWrite(trigger, LOW);
  
  t = pulseIn(echo, HIGH);      //obtenemos el ancho del pulso
  // 1/59 = 0.0169492
  d = t * 0.0169492;                          //escalamos el tiempo a una distancia en cm

  return d;
}

//calculo de promedio de 10 distancias sensadas por el ultrasonido
long promedioDistancia(int trigger, int echo)
{
  long distancia;
  long distancia_total = 0;

  for(int i = 10 ; i>0; i--)
  {
    distancia = medirDistancia(trigger, echo); 
    distancia_total = distancia_total + distancia;
  }
  // 1/10 = 0.1
  return (distancia_total * 0.1);
 }

//busqueda del contrincante
void checkEnemy()
{ 
  long distancia_promedio = promedioDistancia(SENS_ULTRASON_TRIGGER_1, SENS_ULTRASON_ECHO_1);
  
  if(distancia_promedio <= 30) // el ring tiene un area de 175cm o 154cm -- alrededor hay 100cm vacio
  {
    maquinaEstados('m');
  }
  else
  {
    if(checkSenLineaTodos() == LINEA_DETECTADA)
    {
      maquinaEstados('s');
      checkSensorLinea(SENS_ULTRASON_TRIGGER_1, SENS_ULTRASON_ECHO_1);
    }
    else
    {
      maquinaEstados('m');
    }
  }
}

int checkSenLineaTodos() //VERIFICO SI ALGUN SENSOR DETECTA ALGO
{
  int sensor1, sensor2, sensor3, sensor4;
  sensor1 = leerSensorLinea(SENS_LIN_FDER);
  sensor2 = leerSensorLinea(SENS_LIN_TDER);
  sensor3 = leerSensorLinea(SENS_LIN_FIZQ);
  sensor4 = leerSensorLinea(SENS_LIN_TIZQ);
  
  if(LINEA_DETECTADA == 0)
  {
    return (sensor1 & sensor2 & sensor3 & sensor4); //linea_cerca = 0 significa LINEA BLANCA CERCA AHHHH CORRAN
  }
  else //caso en el que usemos COLORES INVERTIDOS
  {
    return (sensor1 | sensor2 | sensor3 | sensor4);
  }
}

//leo sensor de linea
int leerSensorLinea(int sensor)
{
  int value = 1;
  value = digitalRead(sensor);
  return value;
}

//verifico cual sensor detecto la linea blanca
void checkSensorLinea(int trigger, int echo)
{
  if(leerSensorLinea(SENS_LIN_FDER) == LINEA_DETECTADA)
  {
    reaccionFrenteDer(trigger, echo);
  }
  else
  {
    if(leerSensorLinea(SENS_LIN_FIZQ) == LINEA_DETECTADA)
    {
      reaccionFrenteIzq(trigger, echo);
    }
    else
    {
      if(leerSensorLinea(SENS_LIN_TDER) == LINEA_DETECTADA)
      {
        reaccionTraseroDer();
      }
      else
      {
        if(leerSensorLinea(SENS_LIN_TIZQ) == LINEA_DETECTADA)
        {
          reaccionTraseroIzq();
        }
      }
    }
  }
}

//reaccion a sensor frente-derecho
void reaccionFrenteDer(int trigger, int echo)
{
  maquinaEstados('c');
  tiempoEspera_SIN_Interrup(2);
  maquinaEstados('i');
  tiempoEsperaConInterrup(tiempoGiro(135), trigger, echo);
  maquinaEstados('m');
}

//reaccion a sensor frente-izquierdo
void reaccionFrenteIzq(int trigger, int echo)
{
  maquinaEstados('c');
  tiempoEspera_SIN_Interrup(2);
  maquinaEstados('d');
  tiempoEsperaConInterrup(tiempoGiro(135), trigger, echo);
  maquinaEstados('m');
}

//reaccion a sensor trasero-derecho
void reaccionTraseroDer()
{
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE DERECHO -- DOBLA LAS RUEDAS PARA LA DER ?
  maquinaEstados('d');
  tiempoEspera_SIN_Interrup(10);
  maquinaEstados('m');
}

//reaccion a sensor trasero-izquierdo
void reaccionTraseroIzq()
{
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE IZQUIERDO -- DOBLA LAS RUEDAS PARA LA IZQ ?
  maquinaEstados('i');
  tiempoEspera_SIN_Interrup(10);
  maquinaEstados('m');
}

int tiempoGiro(int angulo) //calculo el tiempo que requiere el robot para girar cierto angulo
{
  //REGLA DE 3 SIMPLES --- esto capaz podria estar en una funcion aparte
  // 1/360 = 0.00277778
  return ((angulo * 0.00277778) * T_GIRO360);
}

void Temporizador()
{
  //incremento el timer
  tempo++;
  //reseteo el contador cuando llega a 1000 segundos -- por si acaso para que no se desborde
  if (tempo>1000)
    tempo=0;
}

void tiempoEsperaConInterrup(int segundos, int trigger, int echo)
{
  long int tiempo_actual;
  //asigno tiempo actual del temporizador "tempo" a la variable
  tiempo_actual=tempo;
  
  while ((tempo-tiempo_actual) < segundos) 
  { 
    long distancia_promedio = promedioDistancia(trigger, echo);
  
    if(distancia_promedio <= 10)
    {
      maquinaEstados('m');
      break;
    }
  }
}

void tiempoEspera_SIN_Interrup(int segundos)
{
  long int tiempo_actual;
  //asigno tiempo actual del temporizador "tempo" a la variable
  tiempo_actual=tempo;
  
  while ((tempo-tiempo_actual) < segundos) 
  {
     //waiting 
  }
}
