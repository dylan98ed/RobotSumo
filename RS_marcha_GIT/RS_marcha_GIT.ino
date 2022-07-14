
//declaro variables..
int OUT1 = 6;    //"OUT1" es un nombre que vos elegis, 5 es el numero de pin de la placa que va a ejecutar la accion
int OUT2 = 7;
int OUT3 = 8;
int OUT4 = 9;
// 5 y 6 motor 1 --- MOTOR IZQ
// 7 y 7 motor 2 --- MOTOR DER

int IN1 = 2; //sensor de linea Frente-Derecho
int IN2 = 3; //sensor de linea Trasero-Derecho
int IN3 = 4; //sensor de linea Frente-Izquierdo
int IN4 = 5; //sensor de linea Trasero-Izquierdo

const int Trigger = 10;   //Pin digital 10 para el Trigger del sensor
const int Echo = 11;   //Pin digital 11 para el Echo del sensor

char comando = 's', modo = 'a'; //comando y modo iniciales -- MOTOR PARADO (S) Y MODO AUTOMATICO (A)
char aux; //variable auxiliar para actualizar el comando entrante por el monitor serie
int detectado = 0; //DETECCION DE LINEA BLANCA = 0

const int LLAVE = 12;


void setup() {

  pinMode(LLAVE, INPUT); //SI CONECTAMOS ESTA LLAVE A 5V EL MOTOR AVANZA ATR
  
  //seteo pines de entrada de motores
  pinMode(OUT1, OUTPUT); //motor1
  pinMode(OUT2, OUTPUT); //motor1
  pinMode(OUT3, OUTPUT); //motor2
  pinMode(OUT4, OUTPUT); //motor2

  pinMode(IN1, INPUT); //sensor de linea Frente-Derecho
  pinMode(IN2, INPUT); //sensor de linea Trasero-Derecho
  pinMode(IN3, INPUT); //sensor de Frente-Izquierdo
  pinMode(IN4, INPUT); //sensor de Trasero-Izquierdo
  
  //seteo pines de entrada del sensor ultrasonido
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

  Serial.begin(9600); //Seteo los baudios para comunicarme con el monitor serial. 9600 es la vieja confiable.

  delay (5000);
}

void loop() 
{ 
  for (;;) { //Función loop() está anidado en un ciclo externo con algunas comprobaciones adicionales -- optimiza tiempo si la salteamos

  
    delay (500);
    
      aux = Serial.read();
      if (aux == 'm' || aux == 'c' || aux == 's' || aux == 'd' || aux == 'i' || aux == 'w' || aux == 'a'){
        comando = aux;
      }
    
      //ejecuto funcion CASE
      comandos(comando);
    
      if (digitalRead(LLAVE) == HIGH)
      {
        comandos('m');
        }
        else
        {
          if (modo == 'a' )
          {
            if (checkSenLineaTodos () == detectado)
            {
              comandos('s');
              Serial.println("LINEA BLANCA A LA VISTA");
              checkSensorLinea ();
              }
              else 
              {
                comandos('m');
                //checkEnemy ();
                }
            }
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

long ultrasonido() 
{
  long t; //tiempo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  //delay(100);          //Hacemos una pausa de 100ms
  
  return d;
}

long promedioDistancia ()
{
  long distancia, distancia_prom;
  long distancia_total = 0;

  //calculo de promedio de 10 distancias sensadas por el ultrasonido
  for(int i = 10 ; i>0; i--){
    distancia = ultrasonido(); 
    distancia_total = distancia_total + distancia;
  }
  
  distancia_prom = distancia_total/10;
  Serial.println("distancia promedio:");
  Serial.println(distancia_prom);
  delay(1000);
    
  return distancia_prom;
  }

//busqueda del contrincante
void checkEnemy ()
{ 
  long distancia_promedio = promedioDistancia ();
  Serial.println("distancia promedio en CHECK ENEMY:");
  Serial.print(distancia_promedio);
  
  if (distancia_promedio <= 75) // el ring tiene un area de 175cm o 154cm -- alrededor hay 100cm vacio
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

int checkSenLineaTodos () //VERIFICO SI ALGUN SENSOR DETECTA ALGO
{
  int linea_cerca, sensor1, sensor2, sensor3, sensor4;
  sensor1 = leerSensorLinea(IN1);
  sensor2 = leerSensorLinea(IN2);
  sensor3 = leerSensorLinea(IN3);
  sensor4 = leerSensorLinea(IN4);
  linea_cerca = sensor1 & sensor2 & sensor3 & sensor4;
  Serial.println("resul linea cerca:");
  Serial.println(linea_cerca);
  return linea_cerca; //linea_cerca = 0 significa LINEA BLANCA CERCA AHHHH CORRAN
  }

//leo sensor
int leerSensorLinea (int sensor)
{
  int value = 1;
  value = digitalRead(sensor);
  return value;
  }

//verifico cual sensor detecto la linea blanca
void checkSensorLinea ()
{
  /*
   * int IN1 = 1; //sensor de linea Frente-Derecho
      int IN2 = 2; //sensor de linea Trasero-Derecho
      int IN3 = 3; //sensor de linea Frente-Izquierdo
      int IN4 = 4; //sensor de linea Trasero-Izquierdo
   */
  int sensorPin;
  sensorPin = leerSensorLinea (IN1); //sensor de linea Frente-Derecho
  if (sensorPin == detectado) //sensor de linea Frente-Derecho
  {
    Serial.println("sensor de linea Frente-Derecho");
    reaccionFrenteDer();
    }
    else
    {
      sensorPin = leerSensorLinea (IN2); //sensor de linea Trasero-Derecho
      if (sensorPin == detectado) //sensor de linea Trasero-Derecho
      {
        Serial.println("sensor de linea Trasero-Derecho");
        reaccionTraseroDer();
        }
        else 
        {
          sensorPin = leerSensorLinea (IN3); //sensor de linea Frente-Izquierdo
          if (sensorPin == detectado) //sensor de linea Frente-Izquierdo
          {
            Serial.println("sensor de linea Frente-Izquierdo");
            reaccionFrenteIzq();
            }
            else
            {
              sensorPin = leerSensorLinea (IN4); //sensor de linea Trasero-Izquierdo
              if (sensorPin == detectado) //sensor de linea Trasero-Izquierdo
              {
                Serial.println("sensor de linea Trasero-Izquierdo");
                reaccionTraseroIzq();
                }
              }
          }
      }
  }

//reaccion a sensor frente-derecho
void reaccionFrenteDer ()
{
  comandos('c');
  delay (2);
  comandos('i');
  tiempoGiro(45)
  comandos('m');
  }

//reaccion a sensor frente-izquierdo
void reaccionFrenteIzq ()
{
  comandos('c');
  delay (2);
  comandos('d');
  tiempoGiro(45)
  comandos('m');
  }

//reaccion a sensor trasero-derecho
void reaccionTraseroDer ()
{
  //hay que chekar!!
  
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE DERECHO -- DOBLA LAS RUEDAS PARA LA DER ?
  comandos('d');
  delay (2);
  
  //UNA VEZ QUE SALE DE PELIGRO INTENTA TIRAR AL ENEMIGO GIRANDO PARA LA IZQUIERDA (asumo que van a quedar en paralelo mirando al borde -- el enemy estaria a la izq)
  comandos('i');
  }

//reaccion a sensor trasero-izquierdo
void reaccionTraseroIzq ()
{
  //hay que chekar!!
  
  //SI LO ESTAN EMPUJANDO Y ESTA POR CAERSE EN EL BORDE IZQUIERDO -- DOBLA LAS RUEDAS PARA LA IZQ ?
  comandos('i');
  delay (2);
  //UNA VEZ QUE SALE DE PELIGRO INTENTA TIRAR AL ENEMIGO GIRANDO PARA LA DERECHA (asumo que van a quedar en paralelo mirando al borde -- el enemy estaria a la der)
  comandos('d');
  }

int tiempoGiro(int angulo) //calculo el tiempo que requiere el robot para girar cierto angulo
{
  int tgiro360 = 8; //tiempo que tarda en dar un giro completo 8 segundos -- aprox

  //REGLA DE 3 SIMPLES --- esto capaz podria estar en una funcion aparte
  return ((tgiro360 * angulo)/ 360);
  }

void comandos(char comando)
{
  switch (comando) {
    case 'm':                           //si pongo "m" en el monitor serial el motor avanza.
      Serial.println("marcha de motor");  //Serial.write() = escribe en el monitor serial.
      marcha(OUT1,OUT2); //motor1
      marcha(OUT3,OUT4); //motor2
      break;
    case 'c':                           //si pongo "c" en el monitor serial el motor retrocede.
      Serial.println("contramarcha de motor");
      contramarcha(OUT1,OUT2); //motor1
      contramarcha(OUT3,OUT4); //motor2
      break;
    case 's':                           //si pongo "s" en el monitor serial el motor se detiene
      Serial.println("motor detenido");
      parada(OUT1,OUT2); //motor1
      parada(OUT3,OUT4); //motor2
      break;
    case 'd':                           //si pongo "d" en el monitor serial el der avanza, izq retrocede
      Serial.println("motor giro derecha hacia adelante");
      giroDer(OUT1,OUT2,OUT3,OUT4);
      break;
    case 'i':                           //si pongo "i" en el monitor serial motor izq avanza, der retrocede
      Serial.println("motor giro izquierda hacia adelante");
      giroIzq(OUT1,OUT2,OUT3,OUT4);
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
      parada(OUT1,OUT2); //motor1
      parada(OUT3,OUT4); //motor2
      break;
  }
 }
