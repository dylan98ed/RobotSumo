//LIBRERIA PARA UTILIZAR EL Temporizador DE LA PLACA
#include <TimerOne.h>;

//declaro variables..
int MOT1_C1 = 6;    //"MOT1_C1" es un nombre que vos elegis, 5 es el numero de pin de la placa que va a ejecutar la accion
int MOT1_C2 = 7;
int MOT2_C1 = 8;
int MOT2_C2 = 9;
// MOT 1 --- MOTOR IZQ
// MOT 2 --- MOTOR DER

int SENS_LIN_FDER = 2; //sensor de linea Frente-Derecho
int SENS_LIN_TDER = 3; //sensor de linea Trasero-Derecho
int SENS_LIN_FIZQ = 4; //sensor de linea Frente-Izquierdo
int SENS_LIN_TIZQ = 5; //sensor de linea Trasero-Izquierdo

const int SENS_ULTRASON_TRIGGER = 10;   //Pin digital 10 para el TRIGGER del sensor ultrasonido
const int SENS_ULTRASON_ECHO = 11;   //Pin digital 11 para el ECHO del sensor ultrasonido

char comando = 's', modo = 'a'; //comando y modo iniciales -- MOTOR PARADO (S) Y MODO AUTOMATICO (A)
char aux; //variable auxiliar para actualizar el comando entrante por el monitor serie

int LINEA_DETECTADA = 1; //DETECCION DE LINEA BLANCA = 0

const int LLAVE = 12;

//Variable para la interrupcion
volatile long int time_1=0;

void setup() 
{
  pinMode(LLAVE, INPUT); //SI CONECTAMOS ESTA LLAVE A 5V EL MOTOR AVANZA ATR
  
  //seteo pines de entrada de motores
  pinMode(MOT1_C1, OUTPUT); //motor1
  pinMode(MOT1_C2, OUTPUT); //motor1
  pinMode(MOT2_C1, OUTPUT); //motor2
  pinMode(MOT2_C2, OUTPUT); //motor2

  pinMode(SENS_LIN_FDER, INPUT); //sensor de linea Frente-Derecho
  pinMode(SENS_LIN_TDER, INPUT); //sensor de linea Trasero-Derecho
  pinMode(SENS_LIN_FIZQ, INPUT); //sensor de Frente-Izquierdo
  pinMode(SENS_LIN_TIZQ, INPUT); //sensor de Trasero-Izquierdo
  
  //seteo pines de entrada del sensor ultrasonido
  pinMode(SENS_ULTRASON_TRIGGER, OUTPUT); //pin como salida
  pinMode(SENS_ULTRASON_ECHO, INPUT);  //pin como entrada
  digitalWrite(SENS_ULTRASON_TRIGGER, LOW);//Inicializamos el pin con 0

  Timer1.initialize(1000000);            //configura el timer en 1 segundo
  //1000000 microsegundos = 1 segundo
  Timer1.attachInterrupt(Temporizador);   //configura la interrupcion del timer

  Serial.begin(9600); //Seteo los baudios para comunicarme con el monitor serial. 9600 es la vieja confiable.

  delay(5000); //5 segundos iniciales de quietud
}

void loop() 
{ 
  for(;;) 
  { //Función loop() está anidado en un ciclo externo con algunas comprobaciones adicionales -- optimiza tiempo si la salteamos
    
    //reseteo timer
    time_1=0;
    while (time_1 < 1) 
    {
     //waiting 
    }
    //reseteo timer
    time_1=0;
    
      aux = Serial.read();
      if(aux == 'm' || aux == 'c' || aux == 's' || aux == 'd' || aux == 'i' || aux == 'w' || aux == 'a')
      {
        comando = aux;
      }
    
      //ejecuto funcion CASE
      comandos(comando);
    
      if(digitalRead(LLAVE) == HIGH)
      {
        comandos('m');
      }
      else
      {
        if(modo == 'a' )
        {
          if(checkSenLineaTodos() == LINEA_DETECTADA)
          {
            comandos('s');
            Serial.println("LINEA BLANCA A LA VISTA");
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
}
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * 
 * CONTROL DE RUEDAS
 */
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
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * 
 * CONTROL DE SENSOR ULTRASONIDO
 */
 
long medirDistancia() 
{
  long t; //tiempo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(SENS_ULTRASON_TRIGGER, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(SENS_ULTRASON_TRIGGER, LOW);
  
  t = pulseIn(SENS_ULTRASON_ECHO, HIGH); //obtenemos el ancho del pulso
  // 1/59 = 0.0169492
  d = t * 0.0169492;             //escalamos el tiempo a una distancia en cm
  /*
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  //delay(100);          //Hacemos una pausa de 100ms
  */
  return d;
  // return d;
}

long promedioDistancia()
{
  long distancia, distancia_prom;
  long distancia_total = 0;

  //calculo de promedio de 10 distancias sensadas por el ultrasonido
  for(int i = 10 ; i>0; i--)
  {
    distancia = medirDistancia(); 
    distancia_total = distancia_total + distancia;
  }
  // 1/10 = 0.1
  distancia_prom = distancia_total * 0.1 ;
  Serial.println("distancia promedio:");
  Serial.println(distancia_prom);
  //delay(1000);
    
  return distancia_prom;

  //return (distancia_total * 0.1); --- esto lo agrego cuando saque los serial print
  }
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * 
 * BUSQUEDA DEL CONTRINCANTE
 */
//busqueda del contrincante
void checkEnemy()
{ 
  long distancia_promedio = promedioDistancia();
  Serial.println("distancia promedio en CHECK ENEMY:");
  Serial.print(distancia_promedio);
  
  if(distancia_promedio <= 20) // el ring tiene un area de 175cm o 154cm -- alrededor hay 100cm vacio
  {
    Serial.println("enemigo al frente!");
    comandos('m');
    }
    else
    {
      Serial.println("no hay nadie uwu");
      comandos('i');
      }
  }
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * 
 * CONTROL DE SENSORES DE LINEA
 */
int checkSenLineaTodos() //VERIFICO SI ALGUN SENSOR DETECTA ALGO
{
  int linea_cerca, sensor1, sensor2, sensor3, sensor4;
  sensor1 = leerSensorLinea(SENS_LIN_FDER);
  sensor2 = leerSensorLinea(SENS_LIN_TDER);
  sensor3 = leerSensorLinea(SENS_LIN_FIZQ);
  sensor4 = leerSensorLinea(SENS_LIN_TIZQ);
  
  if(LINEA_DETECTADA == 0)
  {
    linea_cerca = sensor1 & sensor2 & sensor3 & sensor4;
    Serial.println("resul PRODUCTO LOGICO linea cerca:");
    Serial.println(linea_cerca);
    return linea_cerca; //linea_cerca = 0 significa LINEA BLANCA CERCA AHHHH CORRAN
    
    //return (sensor1 & sensor2 & sensor3 & sensor4); --- esto lo deberia poner cuando saque los serial print
  }
  else //caso en el que usemos COLORES INVERTIDOS
  {
    linea_cerca = sensor1 | sensor2 | sensor3 | sensor4;
    Serial.println("resul SUMA LOGICA linea cerca:");
    Serial.println(linea_cerca);
    return linea_cerca;
  }
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
  /*
   * int SENS_LIN_FDER = 1; //sensor de linea Frente-Derecho
      int SENS_LIN_TDER = 2; //sensor de linea Trasero-Derecho
      int SENS_LIN_FIZQ = 3; //sensor de linea Frente-Izquierdo
      int SENS_LIN_TIZQ = 4; //sensor de linea Trasero-Izquierdo
   */
  int sensorPin;
  sensorPin = leerSensorLinea(SENS_LIN_FDER); //sensor de linea Frente-Derecho
  if(sensorPin == LINEA_DETECTADA) //sensor de linea Frente-Derecho
  {
    Serial.println("sensor de linea Frente-Derecho");
    reaccionFrenteDer();
    }
    else
    {
      sensorPin = leerSensorLinea(SENS_LIN_TDER); //sensor de linea Trasero-Derecho
      if(sensorPin == LINEA_DETECTADA) //sensor de linea Trasero-Derecho
      {
        Serial.println("sensor de linea Trasero-Derecho");
        reaccionTraseroDer();
        }
        else 
        {
          sensorPin = leerSensorLinea(SENS_LIN_FIZQ); //sensor de linea Frente-Izquierdo
          if(sensorPin == LINEA_DETECTADA) //sensor de linea Frente-Izquierdo
          {
            Serial.println("sensor de linea Frente-Izquierdo");
            reaccionFrenteIzq();
            }
            else
            {
              sensorPin = leerSensorLinea(SENS_LIN_TIZQ); //sensor de linea Trasero-Izquierdo
              if(sensorPin == LINEA_DETECTADA) //sensor de linea Trasero-Izquierdo
              {
                Serial.println("sensor de linea Trasero-Izquierdo");
                reaccionTraseroIzq();
                }
              }
          }
      }
}
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * 
 * REACCIONES ANTE SITUACIONES
 */
//reaccion a sensor frente-derecho
void reaccionFrenteDer()
{
  comandos('c');
  
  //reseteo timer
  time_1=0;
  while (time_1 < 2) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
    
  comandos('i');

  //reseteo timer
  time_1=0;
  while (time_1 < (tiempoGiro(135))) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;

  comandos('m');
}

//reaccion a sensor frente-izquierdo
void reaccionFrenteIzq()
{
  comandos('c');
  
  //reseteo timer
  time_1=0;
  while (time_1 < 2) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
  
  comandos('d');
  
  //reseteo timer
  time_1=0;
  while (time_1 < (tiempoGiro(135))) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
  
  comandos('m');
}

//reaccion a sensor trasero-derecho
void reaccionTraseroDer()
{
  //hay que chekar!!
  
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE DERECHO -- DOBLA LAS RUEDAS PARA LA DER ?
  comandos('d');
  //reseteo timer
  time_1=0;
  while (time_1 < 2) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
  
  //UNA VEZ QUE SE CORRE DEL ABISMO VA PARA ATRAS UN POCO 
  comandos('c');
  //reseteo timer
  time_1=0;
  while (time_1 < 2) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
}

//reaccion a sensor trasero-izquierdo
void reaccionTraseroIzq()
{
  //hay que chekar!!
  
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE IZQUIERDO -- DOBLA LAS RUEDAS PARA LA IZQ ?
  comandos('i');
  //reseteo timer
  time_1=0;
  while (time_1 < 2) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
  
  //UNA VEZ QUE SE CORRE DEL ABISMO VA PARA ATRAS UN POCO 
  comandos('c');
  //reseteo timer
  time_1=0;
  while (time_1 < 2) 
  {
     //waiting 
  }
  //reseteo timer
  time_1=0;
}

int tiempoGiro(int angulo) //calculo el tiempo que requiere el robot para girar cierto angulo
{
  int tgiro360 = 8000; //tiempo que tarda en dar un giro completo 8 segundos -- aprox

  //REGLA DE 3 SIMPLES --- esto capaz podria estar en una funcion aparte
  // 1/360 = 0.00277778
  return ((tgiro360 * angulo) * 0.00277778);
}
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * COMANDOS
 */
void comandos(char comando)
{
  switch(comando) 
  {
    case 'm':                           //si pongo "m" en el monitor serial el motor avanza.
      Serial.println("marcha de motor");  //Serial.write() = escribe en el monitor serial.
      marcha(MOT1_C1,MOT1_C2); //motor1
      marcha(MOT2_C1,MOT2_C2); //motor2
      break;
    case 'c':                           //si pongo "c" en el monitor serial el motor retrocede.
      Serial.println("contramarcha de motor");
      contramarcha(MOT1_C1,MOT1_C2); //motor1
      contramarcha(MOT2_C1,MOT2_C2); //motor2
      break;
    case 's':                           //si pongo "s" en el monitor serial el motor se detiene
      Serial.println("motor detenido");
      parada(MOT1_C1,MOT1_C2); //motor1
      parada(MOT2_C1,MOT2_C2); //motor2
      break;
    case 'd':                           //si pongo "d" en el monitor serial el der avanza, izq retrocede
      Serial.println("motor giro derecha hacia adelante");
      giroDer(MOT1_C1,MOT1_C2,MOT2_C1,MOT2_C2);
      break;
    case 'i':                           //si pongo "i" en el monitor serial motor izq avanza, der retrocede
      Serial.println("motor giro izquierda hacia adelante");
      giroIzq(MOT1_C1,MOT1_C2,MOT2_C1,MOT2_C2);
      break;
    case 'w':                           //si pongo "w" en el monitor serial ACTIVO MODO MANUAL
      Serial.println("motor MODO MANUAL");
      modo = 'w';
      comando = 's';
      break;
    case 'a':                           //si pongo "a" en el monitor serial ACTIVO MODO AUTOMATICO
      Serial.println("motor MODO AUTOMATICO");
      modo = 'a';
      break;
    default:                            //si no pongo ningun comando, el motor se detiene
      Serial.println("motor detenido");
      parada(MOT1_C1,MOT1_C2); //motor1
      parada(MOT2_C1,MOT2_C2); //motor2
      break;
  }
}
/*
 * ************************************************************************************************************************************************
 * ************************************************************************************************************************************************
 * Temporizador 1
 */
void Temporizador()
{
  //incremento el timer
  time_1++;
  Serial.println(time_1);
  //reseteo el contador cuando llega a 1000 segundos -- por si acaso para que no se desborde
  if (time_1>1000)
    time_1=0;
}
  
